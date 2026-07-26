#pragma once
#include <PK/Core/string.hpp>
#include <PK/Util/event.hpp>
#include <PK/Math/transform.hpp>

namespace Perekop::Window {
        inline pk::Event<pk::vec2> on_resize;

        extern pk::vec2 get_size() noexcept;
        extern void set_size(pk::vec2 size) noexcept;
        extern void set_title(pk::strview title) noexcept;

        extern void maximize() noexcept;
        extern void minimize() noexcept;
}
