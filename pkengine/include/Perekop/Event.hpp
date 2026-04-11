#pragma once
namespace pk {
    template <typename... A> class EventPort;
    template <typename... A> class Event {
        using callback = bool(*)(A...);
        friend EventPort<A...>;
        callback* buf = nullptr;

        short cur = 0, size = 0;

        void _alloc() {
            callback* old_buf = buf;
            buf = new callback[size];
            for (short i = 0; i < cur; ++i) buf[i] = old_buf[i];
            delete[] old_buf;
        }
        
        void _append(const callback& cb) {
            if (cur == size) { size = size * 2 + 1; _alloc(); }
            buf[cur++] = cb;
        }

        public:
            EventPort<A...> port{this};
            void invoke(A... items) {
                for (short i = 0; i < cur; ++i) 
                    if (!buf[i](items...)) buf[i--] = buf[--cur];
            }

            void(*connector)(callback) = nullptr;

            ~Event() { delete[] buf; }
    };

    template <typename... A> class EventPort {
        using callback = bool(*)(A...);
        friend Event<A...>;
        Event<A...>* event;
        EventPort(Event<A...>* e): event(e) {};
        public:
            void listen(callback callback) { 
                if (event->connector) return event->connector(callback);
                
                event->_append(callback);
            }

            EventPort() = delete;
    };
}