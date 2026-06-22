#pragma once
#include <PKCore/vector.hpp>

namespace pk {
    template <typename... T> class Event {
        using callback = void(*)(const T&...);
        vector<callback> listeners;

        public:
            void fire(const T&... items) {
                for (callback call : listeners)
                    call(items...);
            }

            void listen(callback callback) {
                listeners.push_back(callback);
            }
    };
}