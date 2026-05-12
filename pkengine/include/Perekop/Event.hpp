#pragma once
#include <Perekop/Array.hpp>

namespace pk {
    inline thread_local pk::Array<short> index(10);
    template <typename... A> class EventPort;
    template <typename... A> class Event {
        using callback = void(*)(A...);
        friend EventPort<A...>;
        pk::Array<callback> connections;

        public:
            void invoke(A... items) const {
                short& i = index.push(0);
                for (i = 0; i < connections.size(); ++i) 
                    connections[i](items...);
                index.pop();
            }

            void(*connector)(callback) = nullptr;
    };

    template <typename... A> class EventPort {
        using callback = void(*)(A...);
        friend Event<A...>;
        Event<A...>* event;

        public:
            EventPort(Event<A...>& e): event(&e) {};
            void listen(callback callback) const { 
                if (event->connector) return event->connector(callback);

                event->connections.push(callback);
            }

            void disconnect() const {
                if (!index.empty() && event) 
                    event->connections[index.back()--] = event->connections.popout();
            }
    };
}