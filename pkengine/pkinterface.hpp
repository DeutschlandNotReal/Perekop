#pragma once
#include <glm/glm.hpp>
#include <vector>

// ======== EVENTS ========
template <typename T> struct PKEventConnection;

template <typename T> struct PKEvent {
    friend PKEventConnection<T>;
    private:
        std::vector<PKEventConnection<T>> listeners;
    public:
        PKEventConnection<T> listen(void (*callback)(T));
        void invoke(T data);
};

template <typename T> struct PKEventConnection {
    friend PKEvent<T>;
    private:
        uint16_t index;
        PKEvent<T>* event;
        void (*callback)(T);
    public:
        void disconnect();
};

// ======== MESH / MODEL ========
struct PKMeshVertex { glm::vec3 pos; glm::vec3 col; };
struct PKMeshTriangle { uint16_t V0, V1, V2; };

struct PKMesh { 
    public:
        std::vector<PKMeshVertex> V;
        std::vector<PKMeshTriangle> T;
        uint16_t add_vertex(glm::vec3 p);
        uint16_t add_vertex(glm::vec3 p, glm::vec3 c);
        void pop_vertex();
        uint16_t add_triangle(uint16_t v0, uint16_t v1, uint16_t v2);
        uint16_t set_triangle(uint16_t t, uint16_t v0, uint16_t v1, uint16_t v2);
        void pop_triangle();
        uint16_t t_count() { return T.size(); }
        uint16_t v_count() { return V.size(); }
};

struct PKModel { 
    PKMesh* Mesh; 
    glm::mat3x3 transform;
    glm::vec3 pos, scl;
};