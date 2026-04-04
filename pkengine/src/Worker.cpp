#include <Perekop/worker.hpp>
#include <thread>
#include <chrono>

#define ACTIVE 1
#define WORKING 2
#define RESIZING 4
#define FROZEN 8
#define uint unsigned int

using callback = std::function<void()>;

inline double now() {
    return std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count();
}

namespace pk {
    thread_local Worker* Worker::current = nullptr;
    void yield() {
        // does work if has work, or just waits
        if (Worker::current && Worker::current->_ready()) return Worker::current->_call();

        std::this_thread::yield();
    }

    bool Worker::_ready() {
        return count_ready || (count_wait && wait_work[front_wait].time < now());
    }

    void Worker::_alloc_ready(uint new_size){
        flag.fetch_or(RESIZING);
        callback* prev_work = thread_work;
        thread_work.store(new callback[new_size]);

        for (uint i = 0; i < count_ready; i++) thread_work.load()[i] = prev_work[(front_ready+i) % size_ready];
        
        size_ready.store(new_size);
        front_ready.store(0);
        delete[] prev_work;
        flag.fetch_and(~RESIZING);
    }

    void Worker::_alloc_wait(uint new_size) {
        flag.fetch_or(RESIZING);
        defered* prev_work = wait_work;
        wait_work.store(new defered[new_size]);

        for (uint i = 0; i < count_wait; i++) wait_work.load()[i] = prev_work[(front_wait+i) % size_wait];
        
        size_wait.store(new_size);
        front_wait.store(0);
        delete[] prev_work;
        flag.fetch_and(~RESIZING);
    }

    void Worker::_call() {
        if (count_wait) {
            double time_now = now();
            defered dwork = std::move(wait_work[front_wait]);
            if (dwork.time <= time_now) {
                front_wait.store((front_wait+1) % size_wait);
                count_wait.fetch_sub(1);
                return dwork.work();
            }
        }
        callback work = std::move(thread_work[front_ready]);
        front_ready.store((front_ready+1) % size_ready);
        count_ready.fetch_sub(1);
        work();
    }

    void Worker::_loop() {
        Worker::current = this;
        while (flag & ACTIVE) {
            flag.fetch_or(WORKING);
            while (_ready() && !(flag & RESIZING)) _call();
            flag.fetch_and(~WORKING);
            std::this_thread::yield();
        }
        Worker::current = nullptr;
    }
    
    void Worker::task(callback work) {
        if (flag & FROZEN) return;
        if (in_worker()) return work();

        while (flag & (WORKING | RESIZING)) pk::yield();
        if (count_ready == size_ready) _alloc_ready(size_ready << 1);

        thread_work.load()[(front_ready+count_ready) % size_ready] = work;
        count_ready.fetch_add(1);
    }

    void Worker::defer(double duration, callback work) {
        if (flag & FROZEN) return;
        double exit_time = duration + now();
        while (!in_worker() && flag & (WORKING | RESIZING)) pk::yield();
        if (count_wait == size_wait) _alloc_wait(size_wait << 1);
        auto index = (front_wait+count_wait) % size_wait;

        defered job = {work, exit_time};
        wait_work.load()[index] = job;
        for (int i = index - 1; i > 0; i--) {
            auto at = (index - i + size_wait) % size_wait;
            auto job_here = wait_work.load()[at];
            if (job_here.time < exit_time) return;
            wait_work.load()[index] = job_here;
            wait_work.load()[at] = job;
            index = at;
        }
        count_wait.fetch_add(1);
    }

    void Worker::start() {
        if (flag & ACTIVE) return;
        flag.fetch_or(ACTIVE);
        thread = std::thread([this](){ _loop(); });
    }

    void Worker::start_here() {
        if (flag & ACTIVE) return;
        flag.fetch_or(ACTIVE);
        _loop();
    }

    void Worker::finish() {
        flag.fetch_or(FROZEN);
        halt(); join();
        while (size_ready) _call();
    }
}