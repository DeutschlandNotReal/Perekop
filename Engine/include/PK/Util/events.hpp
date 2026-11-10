#pragma once

#include <PK/Core/vector.hpp>

#include <functional>
#include <utility>

namespace pk {
    template <typename... T>
    using consumer = std::function<void(T...)>;

    template <typename T, typename... args> class Event {
        friend T;
        using f = consumer<args...>;
        vector<f> listeners;

        void invoke(args... arg) {
            for (auto& fx : listeners) fx(std::forward<args>(arg)...);
        }

        public:
            void bind(f&& fx) {
                listeners.push(std::forward(fx));
            }
    };
}
