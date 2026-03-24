#pragma once
#include <vector>
#include <functional>

namespace pk {
    template <typename T> class EventPort;
    template <typename T> class EventLink;

    template <typename T> class Event {
        friend EventPort<T>;
        friend EventLink<T>;

        private:
                std::vector<EventLink<T>*> links;
                std::function<void(std::function<void(T)>)> backcall;
        public:
            bool frozen;
            void invoke(T item);
            void set_backcall(std::function<void(std::function<void(T)>)> new_backcall);
            
            EventPort<T> port();
            ~Event();
    };

    template <typename T> class EventPort {
        friend Event<T>;
        Event<T>* event; 
        // now multiple eventports can be made all with raw pointers to a single event (not good)
        // if event is deleted, it will connect to absolute rubbish
        // probably should fix one day

        public:
            EventPort() = delete;
            EventLink<T>* connect(std::function<void(T)> callback);
    };

    template <typename T> struct EventLink {
        friend Event<T>;
        friend EventPort<T>;
        private:
            Event<T>* event;
            unsigned short index;
            std::function<void(T)> call;
            EventLink(Event<T>* ev, unsigned short I, std::function<void(T)> C);
        public:
            void disconnect();
            ~EventLink();
            EventLink() = delete;
    };
}