#pragma once
#include <thread>
#include <functional>

// todo: promises i think they're called
// something so that can return to user
namespace pk {
    class WorkPool {
        using callback = std::function<void()>;
        callback* thread_work = new callback[1];
        short cur = -1, cap = 1, flag = 0;
        void _alloc(short new_cap) {
            cap = new_cap;
            callback* old_work = thread_work;
            thread_work = new callback[cap];
            for (int i = 0; i <= cur; i++) {
                thread_work[i] = old_work[i];
            }
            delete[] old_work;
        }
        public:
            std::thread thread;
            void add_work(callback work) {
                if (in_worker()) { work(); return; }
                while (flag & 2) std::this_thread::yield();
                if (cur + 1 == cap) _alloc(cap << 1);
                thread_work[++cur] = work;
            }

            void start() {
                if (flag & 1) return;
                flag |= 1;
                thread = std::thread([this](){
                    while (flag & 1) {
                        flag |= 2;
                        while (cur > -1) thread_work[cur--]();
                        flag &= ~2;
                        std::this_thread::yield();
                    }
                });
            }

            void halt() { flag &= ~1; }

            ~WorkPool() { 
                halt(); 
                if (thread.joinable()) thread.join();
                delete[] thread_work; 
            }

            bool in_worker() const noexcept {
                return thread.get_id() == std::this_thread::get_id();
            }
    };
}