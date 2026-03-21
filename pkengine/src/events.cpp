#include "events.hpp"

template <typename T>
PKEvent<T>::PKEvent(): port(PKEventPort(this)) {}

template <typename T>
inline PKEventPort<T>& PKEvent<T>::getPort() { return port; }

template <typename T>
PKEventPort<T>::PKEventPort(PKEvent<T>* ev) : event(ev) {}

template <typename T>
PKEventLink<T>::PKEventLink(PKEvent<T>* E, uint16 I, std::function<void(T)> C) : event(E), index(I), call(C) {}

template <typename T>
PKEventLink<T>* PKEventPort<T>::connect(std::function<void(T)> callback) {
    auto* event = this->event;
    auto link = new PKEventLink<T>(event, event->connections.size(), callback);

    event->connections.push_back(link);
    return link;
}

template <typename T>
void PKEventLink<T>::disconnect() {
    if (!event) return;

    auto& connections = event->connections;
    PKEventLink<T>* back = connections.back();

    if (back != this) { back->index = index; connections[index] = back; }
    event->connections.pop_back();
}

template <typename T>
void PKEvent<T>::fire(T item) {
    for (auto* link : connections) link->call(item);
}

template <typename T>
PKEventLink<T>::~PKEventLink() { disconnect(); }


template <typename T>
PKEvent<T>::~PKEvent() {
    for (auto& con : connections) { con->event = nullptr; delete con; }
}