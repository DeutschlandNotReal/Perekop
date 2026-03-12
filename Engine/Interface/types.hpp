#pragma once
#include <vector>
#include <glm/glm.hpp>

template <typename T> struct PKEventConnection;

template <typename T> struct PKEvent {
    friend PKEventConnection<T>;
    private:
        std::vector<PKEventConnection<T>> listeners;
    public:
        PKEventConnection<T> listen(void (*callback)(T));
        void fire(T item);
        ~PKEventConnection();
};

template <typename T> struct PKEventConnection {
    friend PKEvent<T>;
    private:
        PKEvent<T>* event;
        uint16_t index;
        void (callback*)(T) callback;

    public:
        void disconnect();
        ~PKEvent();
};

struct PKMesh { unsigned int EBO, VBO; };

struct PKModel { 
    PKMesh* Mesh; 
    glm::mat3x3 transform;
    glm::vec3 position;
};

struct PKPhysicsContext {
    glm::vec3 pos, vel, acc;
};