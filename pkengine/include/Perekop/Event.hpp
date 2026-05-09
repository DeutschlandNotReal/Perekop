#pragma once
#include <Perekop/Array.hpp>

namespace pk {
    template <typename... A> class EventPort;
    template <typename... A> class Event {
        using callback = bool(*)(A...);
        friend EventPort<A...>;
        pk::Array<callback> connections;

        public:
            EventPort<A...> port{this};

            void invoke(A... items) {
                for (short i = 0; i < connections.size(); ++i) 
                    // return false: disconnect
                    if (!connections[i](items...)) 
                        connections[i--] = connections.pop();
            }

            void invoke_exitless(A... items) const {
                for (callback& cb : connections) cb(items...);
            }

            void(*connector)(callback) = nullptr;
    };

    template <typename... A> class EventPort {
        using callback = bool(*)(A...);
        friend Event<A...>;
        Event<A...>* event;

        public:
            EventPort(Event<A...>* e): event(e) {};

            void listen(callback callback) const { 
                if (event->connector) return event->connector(callback);

                event->connections.push(callback);
            }
    };
}