#pragma once

#include <Perekop/Structure.hpp>

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
                    if (!connections[i](items...)) connections[i--] = connections.pop();
            }

            void(*connector)(callback) = nullptr;
    };

    template <typename... A> class EventPort {
        using callback = bool(*)(A...);
        friend Event<A...>;
        Event<A...>* event;
        EventPort(Event<A...>* e): event(e) {};
        public:
            void listen(callback callback) { 
                if (event->connector) return event->connector(callback);

                event->connections.push(callback);
            }

            EventPort() = delete;
    };
}