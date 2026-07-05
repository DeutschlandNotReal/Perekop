#pragma once
#include <PK/Core/string.hpp>
#include <PK/Util/event.hpp>
#include <PK/Math/transform.hpp>

namespace Perekop::Window {
        inline pk::Event<glm::vec2> on_resize;

        extern vec2 get_size();
        extern void set_size(glm::vec2 size);
        extern void set_title(pk::strview title);

        extern void maximize();
        extern void minimize();
}
