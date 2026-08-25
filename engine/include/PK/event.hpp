#pragma once
#include <PKSTL/vector.hpp>
#include <type_traits>

namespace pk {
    template <typename T>
    using pass_t = std::conditional_t<sizeof(T) <= 16 && std::is_trivially_copyable_v<T>, T, const T&>;

    template <typename... T> class Event {
        using callback = void(*)(pass_t<T>...);
        vector<callback> listeners;

        public:
            void fire(pass_t<T>... items) {
                for (callback call : listeners) call(items...);
            }

            void listen(callback callback) {
                listeners.push(callback);
            }
    };
}