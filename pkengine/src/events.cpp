#include "events.hpp"
#include "util.hpp"

using std::function, std::shared_ptr;

namespace pk {
    template <typename T> EventLink<T>::EventLink(Event<T>* E, unsigned short I, function<void(T)> C) 
        : event(E), index(I), call(C) {}

    template <typename T> std::shared_ptr<EventLink<T>> EventPort<T>::connect(function<void(T)> callback) {
        if (event->filter) { return event->filter(callback); }

        auto link = std::make_shared<EventLink<T>>(event, event->links.size(), callback);
        event->links.push_back(link);
        return link;
    }

    template <typename T> void EventLink<T>::disconnect() {
        if (!event) return;
        pk::util::swappop<T>(event->links, index, [](auto& V, auto I) { V->index = I;});
    }

    template <typename T> void Event<T>::invoke(T item) {
        for (auto& link : links) link->call(item);
    }

    template <typename T> void Event<T>::final(T item) {
        invoke(item);
        filter = [item](auto cb){ cb(item); return nullptr; };
        links.clear();
    }

    template <typename T> EventLink<T>::~EventLink() { disconnect(); }

    template <typename T> Event<T>::~Event() {
        for (auto& link : links) link->event = nullptr;
    }
}