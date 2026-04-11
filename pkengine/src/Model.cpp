#include <glm/ext.hpp>
#include <Perekop/Model.hpp>

namespace pk {
    void Model::set_mesh(Mesh* M) {
       if (M == mesh) return;
       if (mesh) mesh->users.swappop(ref)->ref = ref;

       mesh = M;
       if (!mesh) return;
       
       mesh->users.push(this);
       ref = mesh->users.length() - 1;
    }

    Model::~Model() { 
        if (mesh) mesh->users.swappop(ref)->ref = ref; 
    }

    glm::mat4 Camera::get_viewproj(int screen_x, int screen_y) const {
        return glm::perspective(
            glm::radians(fov), 
            (float)screen_x / (float)screen_y,
            n, f
        ) * glm::inverse((glm::mat4)transform);
    }
}