#pragma once
#include <PKCore/string.hpp>
#include <PKCore/vector.hpp>

namespace pk { class Mesh; }

namespace pk::file {
    string read_src(vstring path);

    void read_obj(vstring path, Mesh& mesh);
}