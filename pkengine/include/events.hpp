#pragma once
#include <functional>

namespace pk {
    template <typename... A> class EventPort;
    template <typename... A> class Event {
        friend EventPort<A...>;
        using callback = std::function<bool(A...)>;
        private:
            callback* buf;
            short cur = 0, size = 10;
            void alloc() {
                callback* old_buf = buf;
                buf = new callback[size];
                for (short i = 0; i < cur; ++i) {
                    buf[i] = old_buf[i];
                }
                delete[] old_buf;
            }
            void append(const callback& cb) {
                if (cur == size) { size <<= 1; alloc(); }
                buf[cur++] = cb;
            }
        public:
            EventPort<A...> port{this};
            void invoke(A... args) {
                for (short i = 0; i < cur; ++i) {
                    bool quit = buf[i](args...);
                    if (quit) { buf[i--] = buf[--cur]; }
                }
            }
            Event(): buf(new callback[10]) {}
            ~Event() { delete[] buf; }
    };

    template <typename... A> class EventPort {
        friend Event<A...>;
        private:
            Event<A...>* event;
            EventPort(Event<A...>* e): event(e) {};
        public:
            void connect(std::function<bool(A...)> callback) { event->append(callback); }
            EventPort() = delete;
            ~EventPort() = delete;
    };
}