#pragma once
#include <PK/Core/string.hpp>
#include <PK/Util/event.hpp>
#include <glm/glm/vec2.hpp>

namespace Perekop::Window {
        inline pk::Event<glm::vec2> on_resize;

        extern glm::vec2 get_size();
        extern void set_size(glm::vec2 size);
        extern void set_title(pk::SView title);

        extern void maximize();
        extern void minimize();
}
