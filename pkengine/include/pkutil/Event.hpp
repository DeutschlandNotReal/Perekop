#pragma once
#include <pkutil/Array.hpp>

namespace pk {
    template <typename... T> class Event {
        using callback = void(*)(T...);
        Array<callback> listeners;
        short i{0};
        public:
            void fire(T... items) {
                for (i = 0; i < listeners.size(); i++)
                    listeners[i](items...);
            }

            void listen(callback callback) {
                listeners.push(callback);
            }

            // only use in callback
            void exit() {
                listeners[i--] = listeners.popout();
            }
    };
}