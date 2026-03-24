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
            std::vector<EventLink<T>*> links;
            std::shared_ptr<Event<T>> self;
        public:
            void invoke(T item);
            void invoke_once(T item); // invokes then lobotomises event
            std::function<EventLink<T>*(std::function<void(T)>)> filter = nullptr;
            
            const EventPort<T> port();
            Event(): self(std::make_shared(this)) {};
            ~Event();
    };

    template <typename T> class EventPort {
        friend Event<T>;
        std::shared_ptr<Event<T>> event; 
        public:
            EventPort(const Event<T>& ev): event(ev.self) {};
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