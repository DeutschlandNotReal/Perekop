#include <glm/ext.hpp>
#include <Perekop/Model.hpp>

namespace pk {
    void Model::set_mesh(Mesh* M) {
       if (M == mesh) return;
       if (mesh) { // already has mesh (disconnect)
            mesh->users[ref] = mesh->users[--mesh->users_count];
       }
       mesh = M;
       if (!mesh) return;
       auto& count = mesh->users_count;
       auto& cap = mesh->users_capacity;
       ref = count;
       if (count == cap) { mesh->resize_users(cap << 1); }
       mesh->users[count++] = this;
    }

    Model::~Model() { set_mesh(nullptr); }

    glm::mat4 Camera::get_viewproj(int screen_x, int screen_y) const {
        return glm::perspective(
            glm::radians(fov), 
            (float)screen_x / (float)screen_y,
            n, f
        ) * glm::inverse((glm::mat4)transform);
    }
}