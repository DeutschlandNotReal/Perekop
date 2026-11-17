#pragma once
#include <filesystem>
#include <functional>
#include <PK/Core/vector.hpp>
#include <PK/Physics/model.hpp>

namespace PKG {
    void ImportPlacement(
        const std::filesystem::path& path, 
        pk::vector<pk::Model>& out,
        std::function<void(pk::MeshHandle&, std::string_view)> namer
    );
}