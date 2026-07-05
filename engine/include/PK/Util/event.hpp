#pragma once
#include <PK/Core/vector.hpp>

namespace pk {
    template <typename... T> class Event {
        using callback = void(*)(pass_t<T>...);
        vector<callback> listeners;

        public:
            void fire(pass_t<T>... items) {
                for (callback call : listeners)
                    call(items...);
            }

            void listen(callback callback) {
                listeners.push_back(callback);
            }
    };
}