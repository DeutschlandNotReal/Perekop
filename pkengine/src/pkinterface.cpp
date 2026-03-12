#include <pkinterface.hpp>

// ======== EVENTS ========
template <typename T>
void PKEventConnection<T>::disconnect() {
    if (event) {
        event->listeners[index] = event->listeners.back();
        event->listeners[index].index = index;
        event->listeners.pop_back();
        event = nullptr;
    }
}

template <typename T>
void PKEvent<T>::invoke(T data) {
    for (auto& listener : listeners) {
        listener.callback(data);
    }
}

template <typename T>
PKEventConnection<T> PKEvent<T>::listen(void (*callback)(T)) {
    listeners.emplace_back((uint16_t)(listeners.size()), this, callback);
    return listeners.back();
}