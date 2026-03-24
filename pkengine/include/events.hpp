#pragma once
#include "util.hpp"
#include <vector>
#include <functional>
#include <memory>

namespace pk {
    template <typename T> class EventPort;
    template <typename T> class EventLink;

    template <typename T> class Event {
        friend EventLink<T>;

        private:
            std::vector<std::shared_ptr<EventLink<T>>> links;
        public:
            std::function<std::shared_ptr<EventLink<T>>(std::function<void(T)>)> filter = nullptr;

            EventPort<T> port{this};

            void invoke(T item) { for (auto& link : links) link->call(item); };

            void final(T item) {
                invoke(item);
                links.clear();
                filter = [item](auto cb){ cb(item); return nullptr; };
            };

            ~Event<T>() { for (auto& link : links) link->event = nullptr; }

    };

    template <typename T> class EventPort {
        friend Event<T>;
        Event<T>* event;
        public:
            EventPort(Event<T>* ev): event(ev) {}
            EventPort() = delete;
            [[nodiscard]] std::shared_ptr<EventLink<T>> connect(std::function<void(T)> callback) const {
                if (event->filter) { return event->filter(callback); }

                auto link = std::make_shared<EventLink<T>>(event, event->links.size(), callback);
                event->links.push_back(link);
                return link;
            };
    };

    template <typename T> struct EventLink {
        friend Event<T>;
        friend EventPort<T>;
        private:   
            Event<T>* event;
            unsigned short index;
            std::function<void(T)> call;
            EventLink(Event<T>* ev, unsigned short I, std::function<void(T)> C): event(ev), index(I), call(C) {};
        public:
            void disconnect() {
                if (event) pk::util::swappop<T>(event->links, index, [](auto& V, auto I) { V->index = I;});
            };

            ~EventLink() { disconnect(); }

            EventLink() = delete;
    };
}