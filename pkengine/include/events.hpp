#pragma once
#include <vector>
#include <functional>

using uint16 = unsigned short;

namespace pk {
    template <typename T> class EventPort;
    template <typename T> class EventLink;

    template <typename T> class Event {
        friend EventPort<T>;
        friend EventLink<T>;
        private:
            std::vector<EventLink<T>*> connections;
            EventPort<T> evport;
    public:
        void invoke(T item);
        inline EventPort<T>& port();

        Event();
        ~Event();
    };

    template <typename T> class EventPort {
        friend Event<T>;
        private:
            Event<T>* event;
            EventPort(Event<T>* ev);
        public:
            EventPort() = delete;
            EventLink<T>* connect(std::function<void(T)> callback);
    };

    template <typename T> struct EventLink {
        friend Event<T>;
        friend EventPort<T>;
        private:
            Event<T>* event;
            uint16 index;
            std::function<void(T)> call;
            EventLink(Event<T>* ev, uint16 I, std::function<void(T)> C);
        public:
            void disconnect();
            ~EventLink();
            EventLink() = delete;
    };
}