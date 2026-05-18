#pragma once
#include <pkutil/Array.hpp>

namespace pkutil {
    template <typename... T> using func = void(*)(T...);
    template <typename... T> class Event {
        Array<func<T...>> listeners;
        public:
            void invoke(T... items) const {
                for (const func<T...>& f : listeners)
                    f(items...);
            }
            void listen(func<T...> callback) {
                listeners.push(callback);
            }
    };

    template <typename... T> class EventPort {
        Event<T...>* event;
        public:
            EventPort(Event<T...>& e): event(&e) {};
            void listen(func<T...> callback) const { event->listen(callback); }
    };
}