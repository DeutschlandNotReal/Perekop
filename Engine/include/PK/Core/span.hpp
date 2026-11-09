#pragma once

#include <PK/Core/memory.hpp>

namespace pk {
    template <typename T = void> class span {
        T* data{nullptr}, *cap{nullptr};
        public:
            constexpr auto* begin(this auto&& s) noexcept { return s.data; }
            constexpr auto* end(this auto&& s) noexcept { return s.cap; }
            constexpr auto& operator[](this auto&& s, uint i) noexcept { return s.data[i]; }

            constexpr uint size() const noexcept { return cap - data; }

            constexpr span() noexcept = default;
            constexpr span(T* single) noexcept: data(single), cap(single+1) {}
            constexpr span(T* first, T* end) noexcept: data(first), cap(end) {}
            constexpr span(T* first, uint n) noexcept: data(first), cap(first + n) {}
            
            template <uint L> constexpr span(T (&items)[L]): data(items), cap(items+L) {}

            constexpr explicit operator bool() const noexcept { return data != nullptr; }
            constexpr bool operator!()         const noexcept { return data == nullptr; }

            template <typename C>
            constexpr span(C& container) noexcept requires(
                std::is_convertible_v<decltype(container.begin()), T*> &&
                std::is_convertible_v<decltype(container.end()), T*>
            ):
                data((T*)container.begin()),
                cap((T*)container.end())
            {}

            template <typename f, typename... A> 
            void map(A... args, T* result) const noexcept {
                for (uint i{0}; i < size(); i++) result[i] = f(data[i], std::forward(args...));
            }

            span subspan(uint from, uint to) noexcept {
                return {data+from, to-from};
            }

            const span subspan(uint from, uint to) const noexcept {
                return {data + from, to-from};
            }
    };

    template <> class span<void> {
        const void* data{nullptr}, *cap{nullptr};
        public:
            constexpr const void* begin() const noexcept { return data; }
            constexpr const void* end()   const noexcept { return cap; }
            constexpr uint size() const noexcept { return (char*)cap - (char*)data; }

            constexpr explicit operator bool() const noexcept { return data != nullptr; }
            constexpr bool operator!()         const noexcept { return data == nullptr; }

            template <typename T> span(const span<T> from) noexcept:
                data((const void*) from.begin()),
                cap((const void*) from.end())
            {}
    
            template <typename T> const T& operator[](uint i) noexcept {
                return *((const T*)data + i);
            }
            
            template <typename C>
            constexpr span(const C& container) noexcept requires(
                std::is_convertible_v<decltype(container.begin()), const void*> &&
                std::is_convertible_v<decltype(container.end()), const void*>
            ):
                data((const void*)container.begin()),
                cap((const void*)container.end())
            {}
    };
}