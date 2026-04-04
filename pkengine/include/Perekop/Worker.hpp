#pragma once
#include <thread>
#include <functional>
#include <atomic>

namespace pk {
    void yield();

    class Worker {
        friend void pk::yield();
        using callback = std::function<void()>;
        struct defered {
            callback work = nullptr;
            double time;
        };

        std::atomic<callback*> thread_work{new callback[2]};
        std::atomic<defered*>   wait_work{new defered[2]};

        std::atomic<unsigned int> front_ready{0}, count_ready{0}, size_ready{2};
        std::atomic<unsigned int> front_wait{0}, count_wait{0}, size_wait{2};
        std::atomic<char> flag{0};

        void _alloc_ready(unsigned int new_size);
        void _alloc_wait(unsigned int new_size);

        void _call();
        void _loop();
        bool _ready();

        public:
            std::thread thread;
            void task(callback work);
            void defer(double time, callback work);
            void start();
            void start_here();
            void finish();

            void halt() noexcept { flag.fetch_and(~1); }

            void join() { if (thread.joinable()) thread.join(); }

            bool in_worker() const noexcept { return Worker::current == this;}

            bool busy() const noexcept { return flag & 2; }
            bool active() const noexcept { return flag & 1; }

            ~Worker() { halt(); join(); }
            static thread_local Worker* current;
    };
}