#pragma once
#include <PK/Core/vector.hpp>

namespace pk {
    template <typename... T> class Event {
        using callback = void(*)(constref<T>...);
        Vec<callback> listeners;

        public:
            void fire(constref<T>... items) {
                for (callback call : listeners)
                    call(items...);
            }

            void listen(callback callback) {
                listeners.push_back(callback);
            }
    };
}