#pragma once
#include <Perekop/Worker.hpp>

#define RESIZING 1
#define INVOKING 2

namespace pk {
    template <typename... A> class EventPort;
    template <typename... A> class Event {
        friend EventPort<A...>;
        using callback = bool(*)(A...);
        private:
            callback* buf;
            short cur = 0, size = 2;
            std::atomic<char> flag{0};

            void _alloc() {
                if (flag.load() & RESIZING) return;
                while (flag.load() & INVOKING) pk::yield();
                flag.store(RESIZING);
                callback* old_buf = buf;
                buf = new callback[size];
                for (short i = 0; i < cur; ++i) {
                    buf[i] = old_buf[i];
                }
                delete[] old_buf;
                flag.store(0);
            }
            void _append(const callback& cb) {
                if (cur == size) { size <<= 1; _alloc(); }
                buf[cur++] = cb;
            }

        public:
            EventPort<A...> port{this};
            void invoke(A... items) {
                while (flag.load()) pk::yield();
                flag.store(INVOKING);
                for (short i = 0; i < cur; ++i) {
                    if (!buf[i](items...)) { buf[i--] = buf[--cur]; }
                }
                flag.store(0);
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

#undef INVOKING
#undef RESIZING