#include "Engine/Interface/types.hpp"

// ======== EVENTS ========
template <typename T>
void PKEventConnection<T>::disconnect() {
    if (event) {
        if (index == event->listeners.size() - 1) {
            event->listeners.pop_back();
        } else {
            // Swap-Pop removal
            auto back_event = event->listeners.back();
            event->listeners[index] = back_event;
            back_event.index = index;
            event->listeners.pop_back();
            ~PKEventConnection();
        }
    }
};

template <typename T>
void PKEvent<T>::fire(T item) {
    for (auto& listener : listeners) {
        listener.callback(item);
    }
};

template <typename T>
PKEventConnection<T> PKEvent<T>::listen(void (*callback)(T)) {
    listeners.emplace_back(this, (uint16_t)(listeners.size() - 1), callback);
    return listeners.back();
};
