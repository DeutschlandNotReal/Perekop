#include <glm/ext.hpp>
#include <Perekop/Model.hpp>

namespace pk {
    void Model::set_mesh(Mesh* M) {
       if (M == mesh) return;
       if (mesh) mesh->users.swappop(ref)->ref = ref;

       mesh = M;
       if (!mesh) return;
       
       mesh->users.push(this);
       ref = mesh->users.size() - 1;
    }

    Model::~Model() { 
        if (mesh) mesh->users.swappop(ref)->ref = ref; 
    }

    glm::mat4 Camera::get_viewproj(glm::vec2 screen_size) const {
        return glm::perspective(
            glm::radians(fov), 
            (float)screen_size.x / (float)screen_size.y,
            n, f
        ) * glm::inverse((glm::mat4)transform);
    }
}