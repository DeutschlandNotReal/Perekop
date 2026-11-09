#pragma once

#include <PK/Core/vector.hpp>

#include <functional>
#include <utility>

namespace pk {
    template <typename... T>
    using consumer = std::function<void(T...)>;

    template <typename... T>
    using listeners = vector<consumer<T...>>;

    template <typename... T>
    struct nconsumer { 
        std::string_view name; 
        consumer<T...> f;
        nconsumer() noexcept = default;
        nconsumer(std::string_view name, consumer<T...> f): name(name), f(f) {}
    };

    template <typename... T>
    using nlisteners = vector<nconsumer<T...>>;

    template <typename... T, typename... A>
    void invokeListeners(const listeners<T...>& callbacks, A&&... args) noexcept {
        for (const auto& callback : callbacks)
            callback(std::forward<A>(args)...);
    }
}
