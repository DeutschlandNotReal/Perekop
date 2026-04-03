#pragma once
#include <thread>
#include <functional>
#include <atomic>

namespace pk {
    void yield();

    class Worker {
        friend void pk::yield();
        using callback = std::function<void()>;
        callback* thread_work = new callback[2];
        std::atomic<unsigned int> front{0}, count{0}, size{2};
        std::atomic<char> flag{0};

        void _alloc( unsigned int new_size );
        callback& _dequeue();

        void one();

        public:
            std::thread thread;
            void task(callback work);
            void start();
            void halt();
            ~Worker();

            bool in_worker() const;
            static thread_local Worker* current;
    };
}