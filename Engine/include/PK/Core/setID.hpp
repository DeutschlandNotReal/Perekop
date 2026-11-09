#pragma once
#include <PK/Math/typedef.hpp>

namespace pk {
    static inline constexpr uint nullid{~0u};

    class setID {
        uint id{nullid};
        public:
            setID() noexcept = default;
            setID(uint x) noexcept: id(x) {}
            operator uint() const noexcept { return id; }
            setID& operator=(uint x) noexcept { id = x; return *this; }
    };
}