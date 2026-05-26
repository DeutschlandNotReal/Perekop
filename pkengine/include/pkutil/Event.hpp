#pragma once
#include <pkutil/Array.hpp>

namespace pk {
    template <typename... T> class Event {
        using callback = void(*)(T...);
        struct Entry { callback cb; void* data; };
        Array<Entry> listeners;
        short i{0};
        public:
            void fire(T... items) {
                for (i = 0; i < listeners.size(); i++)
                    listeners[i].cb(items...);
            }

            void listen(callback callback, void* data = nullptr) {
                listeners.push({callback, data});
            }

            void exit() {
                listeners[i--] = listeners.popout();
            }
        
            template <typename t> t& value(){
                return *(t*)listeners[i].data;
            }
    };
}