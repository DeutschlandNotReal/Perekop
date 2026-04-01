#pragma once
#include <thread>
#include <atomic>

#define YIELD_WHILE(t) while(t) std::this_thread::yield();

namespace pk {
    template <typename... A> class EventPort;
    template <typename... A> class Event {
        friend EventPort<A...>;
        using callback = bool(*)(A...);
        private:
            callback* buf;
            short cur = 0, size = 2;
            std::atomic<char> flags; // resizing: 1, invoking: 2, both: 3

            void _alloc() {
                if (flags & 1) return;
                YIELD_WHILE(flags & 2)
                flags = 1;
                callback* old_buf = buf;
                buf = new callback[size];
                for (short i = 0; i < cur; ++i) {
                    buf[i] = old_buf[i];
                }
                delete[] old_buf;
                flags = 0;
            }
            void _append(const callback& cb) {
                if (cur == size) { size <<= 1; _alloc(); }
                buf[cur++] = cb;
            }

        public:
            EventPort<A...> port{this};
            void invoke(A... items) {
                YIELD_WHILE(flags)
                flags = 2;
                for (short i = 0; i < cur; ++i) {
                    if (!buf[i](items...)) { buf[i--] = buf[--cur]; }
                }
                flags = 0;
            }

            void(*connector)(callback) = nullptr;

            Event(): buf(new callback[2]) {}
            ~Event() { delete[] buf; }
    };

    template <typename... A> class EventPort {
        friend Event<A...>;
        using callback = bool(*)(A...);
        private:
            Event<A...>* event;
            EventPort(Event<A...>* e): event(e) {};
        public:
            void listen(callback callback) { 
                if (event->connector) { 
                    event->connector(callback); 
                } else {
                   event->_append(callback);  
                }
            }
            EventPort() = delete;
    };
}

#undef YIELD_WHILE