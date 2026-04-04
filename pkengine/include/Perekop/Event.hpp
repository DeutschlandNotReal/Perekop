#pragma once
#include "Worker.hpp"
#include <Perekop/Worker.hpp>

namespace pk {
    template <typename... A> class EventPort;
    template <typename... A> class Event {
        using callback = bool(*)(A...);
        friend EventPort<A...>;
        callback* buf;
        Worker* _worker;
        short cur = 0, size = 2;

        void _alloc() {
            callback* old_buf = buf;
            buf = new callback[size];
            for (short i = 0; i < cur; ++i) buf[i] = old_buf[i];
            delete[] old_buf;
        }
        
        void _append(const callback& cb) {
            if (cur == size) { size <<= 1; _alloc(); }
            buf[cur++] = cb;
        }

        public:
            EventPort<A...> port{this};
            void invoke(A... items) {
                _worker->task([this, items...](){ 
                    for (short i = 0; i < cur; ++i) 
                        if (!buf[i](items...)) buf[i--] = buf[--cur];
                });
            }

            void(*connector)(callback) = nullptr;

            Event(Worker* worker): _worker(worker), buf(new callback[2]) {}
            ~Event() { delete[] buf; }
    };

    template <typename... A> class EventPort {
        using callback = bool(*)(A...);
        friend Event<A...>;
        Event<A...>* event;
        EventPort(Event<A...>* e): event(e) {};
        public:
            void listen(callback callback) { 
                event->_worker->task([this, callback](){ 
                    if (event->connector) return event->connector(callback);
                    event->_append(callback);
                });
            }

            EventPort() = delete;
    };
}