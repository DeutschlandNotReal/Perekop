#pragma once
#include <PKCore/Vector.hpp>

namespace pk {
    template <typename... T> class Event {
        using callback = void(*)(const T&...);
        struct Entry { callback cb; void* data; };
        vector<Entry> listeners;
        short i{-1};

        public:
            void fire(const T&... items) {
                for (i = 0; i < listeners.size(); i++)
                    listeners[i].cb(items...);
                i = -1;
            }

            void listen(callback callback, void* data = nullptr) {
                listeners.push({callback, data});
            }

            void exit() {  
                if (i == -1) return;
                listeners[i--] = listeners.popout();
            }
        
            template <typename t> t* value(){
                if (i == -1) return nullptr;
                return (t*)listeners[i].data;
            }
    };
}