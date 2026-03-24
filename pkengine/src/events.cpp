#include "events.hpp"
#include "util.hpp"

using std::function;

namespace pk {
    template <typename T> EventLink<T>::EventLink(Event<T>* E, unsigned short I, function<void(T)> C) 
        : event(E), index(I), call(C) {}

    template <typename T> EventPort<T> Event<T>::port() { return EventPort<T>{this}; }

    template <typename T> EventLink<T>* EventPort<T>::connect(function<void(T)> callback) {
        if (event->frozen && event->backcall) { 
            event->backcall(callback); 
            return nullptr;
        }
        auto link = new EventLink<T>(event, event->links.size(), callback);

        event->links.push_back(link);
        return link;
    }

    template <typename T> void EventLink<T>::disconnect() {
        if (!event) return;
        pk::util::swappop<T>(event->links, index, [](auto& V, auto I) { V->index = I;});
    }

    template <typename T> void Event<T>::invoke(T item) {
        for (EventLink<T>* link : links) link->call(item);
    }

    template <typename T> EventLink<T>::~EventLink() { disconnect(); }

    template <typename T> Event<T>::~Event() {
        for (EventLink<T>* link : links) { link->event = nullptr; delete link; }
    }
}