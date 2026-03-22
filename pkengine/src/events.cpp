#include "events.hpp"

#define generic template <typename T>

namespace pk {
    generic event<T>::event(): evport(event_port(this)) {}
    generic inline event_port<T>& event<T>::port() { return evport; }
    generic event_port<T>::event_port(event_link<T>* event) : ev(event) {}
    generic event_link<T>::event_link(pk::event<T>* event, uint16 I, std::function<void(T)> C) : ev(event), index(I), call(C) {}
    generic event_link<T>* event_port<T>::connect(std::function<void(T)> callback) {
        auto* event = this->ev;
        auto link = new event_link<T>(event, event->connections.size(), callback);

        event->connections.push_back(link);
        return link;
    }
    generic void event_link<T>::disconnect() {
        if (!ev) return;

        auto& connections = ev->connections;
        event_link<T>* back = connections.back();

        if (back != this) { back->index = index; connections[index] = back; }
        ev->connections.pop_back();
    }
    generic void event<T>::invoke(T item) {
        for (auto* link : connections) link->call(item);
    }
    generic event_link<T>::~event_link() { disconnect(); }
    generic event<T>::~event() {
        for (auto& con : connections) { con->event = nullptr; delete con; }
    }
}