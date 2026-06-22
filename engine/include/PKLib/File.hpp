#pragma once
#include <PKCore/string.hpp>
#include <PKCore/vector.hpp>

namespace pk { class Mesh; }

namespace pk::file {
    string read_src(strview path);

    void read_obj(strview path, Mesh& mesh);
}