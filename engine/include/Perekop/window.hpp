#pragma once
#include <PKCore/string.hpp>
#include <PKLib/event.hpp>
#include <PKAlias/math.hpp>

namespace Perekop::Window {
        inline pk::Event<vec2> on_resize;

        extern vec2 get_size();
        extern void set_size(vec2 size);
        extern void set_title(pk::strview title);

        extern void maximize();
        extern void minimize();
}
