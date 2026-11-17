#pragma once
#include <PK/Math//typedef.hpp>

namespace pk {
    template <typename T> class glRefference {
        protected:
            uint index{0};
        
        public:
            uint getIndex() const noexcept { return index; }
    };

    template <typename T> class glResource : public glRefference<T>{
        protected:
            using glRefference<T>::index;
            
        public:
            static T create();
            glResource() noexcept = default;

            glResource(const glResource&) = delete;
            glResource& operator=(const glResource&) = delete;
            
            glResource(glResource&& from) noexcept;
            glResource& operator=(glResource&& from) noexcept;
            ~glResource();
    };
}