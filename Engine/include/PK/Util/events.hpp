#pragma once

#include <PK/Core/vector.hpp>
#include <functional>

namespace pk {
    template <typename... T>
    using consumer = std::function<void(T...)>;

    template <typename... T> class Event {
        using callback = consumer<T...>;
        vector<callback> listeners;

        public:
            void bind(callback fx) { 
                listeners.push(fx); 
            }

            void invoke(T... arg) { 
                for (callback& fx : this->listeners) fx(arg...); 
            }
    };
}
