#pragma once
#include "util.hpp"
#include <vector>
#include <functional>
#include <memory>
#include <thread>

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

            void invoke(T item) { 
                auto links_copy = links;
                for (auto& link : links_copy) link->call(item);
            }

            void invoke_async(T item) {
                std::thread([links = links, item](){
                    for (auto& link : links) link->call(item);
                 }).detach();
            }

            void lock(T item) {
                invoke(item);
                filter = [item](auto cb){ cb(item); return nullptr; };
            }

            void unlock() { filter = nullptr; }

            void clear() { links.clear(); }

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
            unsigned short ref;
            std::function<void(T)> call;
            EventLink(Event<T>* ev, unsigned short I, std::function<void(T)> C): event(ev), ref(I), call(C) {};
        public:
            void disconnect() {
                if (event) {
                    pk::util::swappop<T>(event->links, ref, [this](auto& V){ V->index = ref; }); 
                    event = nullptr;
                }
            };

            ~EventLink() { disconnect(); }

            EventLink() = delete;
    };
}