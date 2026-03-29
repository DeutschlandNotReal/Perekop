#pragma once
#include <functional>
#include <thread>
#include <atomic>

#define yield std::this_thread::yield

namespace pk {
    template <typename... A> class EventPort;
    template <typename... A> class Event {
        friend EventPort<A...>;
        using callback = std::function<bool(A...)>;
        private:
            callback* buf;
            short cur = 0, size = 10;
            std::atomic<char> flags; // resizing: 1, invoking: 2, both: 3
            std::function<void(callback)> connector = nullptr;

            void alloc() {
                if (flags & 1) return;
                while(flags & 2) yield();
                flags = 1;
                callback* old_buf = buf;
                buf = new callback[size];
                for (short i = 0; i < cur; ++i) {
                    buf[i] = old_buf[i];
                }
                delete[] old_buf;
                flags = 0;
            }
            void append(const callback& cb) {
                if (cur == size) { size <<= 1; alloc(); }
                buf[cur++] = cb;
            }

        public:
            EventPort<A...> port{this};
            void invoke(A... args) {
                while (flags) yield();
                flags = 2;
                for (short i = 0; i < cur; ++i) {
                    bool remain = buf[i](args...);
                    if (!remain) { buf[i--] = buf[--cur]; }
                }
                flags = 0;
            }

            void freeze(A... args) {
                connector = [args...](callback& cb){ cb(args...); };
            }

            void unfreeze() { connector = nullptr; }

            Event(): buf(new callback[10]) {}
            ~Event() { delete[] buf; }
    };

    template <typename... A> class EventPort {
        friend Event<A...>;
        private:
            Event<A...>* event;
            EventPort(Event<A...>* e): event(e) {};
        public:
            void connect(std::function<bool(A...)> callback) { 
                if (event->connector) { 
                    event->connector(callback); 
                } else {
                   event->append(callback);  
                }
            }
            EventPort() = delete;
            ~EventPort() = delete;
    };
}
#undef yield