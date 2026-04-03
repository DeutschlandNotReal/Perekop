#include <Perekop/worker.hpp>

#define ACTIVE 1
#define WORKING 2
#define uint unsigned int

using callback = std::function<void()>;

namespace pk {
    thread_local Worker* Worker::current = nullptr;
    void yield() {
        if (Worker::current) {
            Worker::current->one();
        } else {
            std::this_thread::yield();
        }
    }

    void Worker::_alloc(uint new_size){
        callback* prev_work = thread_work;
        thread_work = new callback[new_size];

        for (uint i = 0; i < count; i++) thread_work[i] = prev_work[(front+i) % size];
        
        size = new_size;
        front = 0;
        delete[] prev_work;
    }

    callback& Worker::_dequeue() {
        callback& work = thread_work[front];
        front = (front+1) % size;
        count--;
        return work;
    }

    void Worker::task(callback work) {
        if (in_worker()) return work();

        while (flag & WORKING) pk::yield();
        if (count == size) _alloc(size << 1);

        thread_work[(front+count++) % size] = work;
    }

    void Worker::start() {
        if (flag & ACTIVE) return;
        flag.fetch_or(ACTIVE);
        thread = std::thread([this](){
            Worker::current = this;
            while (flag & ACTIVE) {
                flag.fetch_or(WORKING);
                while (count) _dequeue()();
                flag.fetch_and(~WORKING);
                std::this_thread::yield();
            }
            Worker::current = nullptr;
        });
    }

    void Worker::one() {
        if (count) _dequeue()();
    }

    void Worker::halt() {
        flag.fetch_and(~ACTIVE);
    }

    Worker::~Worker() {
        halt();
        if (thread.joinable()) thread.join();
    }

    bool Worker::in_worker() const {
        return Worker::current == this;
    }
}