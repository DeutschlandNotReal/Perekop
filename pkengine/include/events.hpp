#pragma once
#include <vector>
#include <functional>

using uint16 = unsigned short;

template <typename T> struct PKEventPort;
template <typename T> struct PKEventLink;

template <typename T> struct PKEvent {
    friend PKEventPort<T>;
    friend PKEventLink<T>;
    private:
        std::vector<PKEventLink<T>*> connections;
        PKEventPort<T> port;
    public:
        void fire(T item);
        inline PKEventPort<T>& getPort();

        PKEvent();
        ~PKEvent();
};

template <typename T> struct PKEventPort {
    friend PKEvent<T>;
    private:
        PKEvent<T>* event;
        PKEventPort(PKEvent<T>* ev);
    public:
        PKEventPort() = delete;
        PKEventLink<T>* connect(std::function<void(T)> callback);
};

template <typename T> struct PKEventLink {
    friend PKEvent<T>;
    friend PKEventPort<T>;
    private:
        PKEvent<T>* event;
        uint16 index;
        std::function<void(T)> call;
        PKEventLink(PKEvent<T>* E, uint16 I, std::function<void(T)> C);
    public:
        void disconnect();
        ~PKEventLink();
        PKEventLink() = delete;
};