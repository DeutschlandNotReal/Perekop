#pragma once
#include <vector>
#include <functional>
#include <memory>

namespace pk {
    template <typename T> class EventPort;
    template <typename T> class EventLink;

    template <typename T> class Event {
        friend EventPort<T>;
        friend EventLink<T>;

        private:
            std::vector<std::shared_ptr<EventLink<T>>> links;
        public:
            const EventPort<T> port{this};
            void invoke(T item);
            void final(T item); // invokes then lobotomises event
            std::function<std::shared_ptr<EventLink<T>>(std::function<void(T)>)> filter = nullptr;
            ~Event();
    };

    template <typename T> class EventPort {
        friend Event<T>;
        Event<T>* event;
        public:
            EventPort(Event<T>* ev): event(ev) {};
            EventPort() = delete;
            std::shared_ptr<EventLink<T>> connect(std::function<void(T)> callback);
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