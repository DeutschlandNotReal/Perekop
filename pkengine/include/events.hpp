#pragma once
#include <vector>
#include <functional>

using uint16 = unsigned short;

namespace pk {
    template <typename T> struct event_port;
    template <typename T> struct event_link;

    template <typename T> struct event {
        friend event_port<T>;
        friend event_link<T>;
        private:
            std::vector<event_link<T>*> connections;
            event_port<T> evport;
    public:
        void invoke(T item);
        inline event_port<T>& port();

        event();
        ~event();
    };

    template <typename T> struct event_port {
        friend event<T>;
        private:
            event<T>* ev;
            event_port(event_link<T>* event);
        public:
            event_port() = delete;
            event_link<T>* connect(std::function<void(T)> callback);
    };

    template <typename T> struct event_link {
        friend event<T>;
        friend event_port<T>;
        private:
            event<T>* ev;
            uint16 index;
            std::function<void(T)> call;
            event_link(pk::event<T>* event, uint16 I, std::function<void(T)> C);
        public:
            void disconnect();
            ~event_link();
            event_link() = delete;
    };
}