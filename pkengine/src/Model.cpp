#include <glm/ext.hpp>
#include <Perekop/Model.hpp>

namespace pk {
    void Model::apply_mesh(Mesh* new_mesh) {
        if (new_mesh == _mesh) return;

        if (_mesh) {
            // currently has mesh, has to update mesh's list
            _mesh->users[ref] = _mesh->users.pop();
            _mesh->users[ref]->ref = ref;
        }
        _mesh = new_mesh;
        if (_mesh) {
            _mesh->users.push(this);
            ref = _mesh->users.size() - 1;
        }
    }

    Model::~Model() { 
        if (_mesh) {
            _mesh->users[ref] = _mesh->users.pop();
            _mesh->users[ref]->ref = ref;
        }
    }

    glm::mat4 Camera::get_VP(glm::vec2 screen_size) const noexcept {
        return glm::perspective(
            glm::radians(fov), 
            (float)screen_size.x / (float)screen_size.y,
            n, f
        ) * glm::inverse((glm::mat4)transform);
    }
}