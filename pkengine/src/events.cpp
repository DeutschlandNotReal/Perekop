#include "events.hpp"
#include "util.hpp"

#define generic template <typename T>
using std::function;

namespace pk {
    generic Event<T>::Event(): evport(EventPort(this)) {}
    generic EventPort<T>::EventPort(Event<T>* E) : event(E) {}
    generic EventLink<T>::EventLink(Event<T>* E, uint16 I, function<void(T)> C) : event(E), index(I), call(C) {}
    generic EventLink<T>* EventPort<T>::connect(function<void(T)> callback) {
        auto link = new EventLink<T>(event, event->connections.size(), callback);

        event->connections.push_back(link);
        return link;
    }
    generic void EventLink<T> ::disconnect() {
        if (!event) return;

        pk::util::swappop<T>(event->connections, index, [](auto& V, auto I) { V->index = I;});
    }
    generic void Event<T>::invoke(T item) {
        for (EventLink<T>* link : connections) link->call(item);
    }
    generic EventLink<T>::~EventLink() { disconnect(); }
    generic Event<T>::~Event() {
        for (EventLink<T>* con : connections) { con->event = nullptr; delete con; }
    }
}