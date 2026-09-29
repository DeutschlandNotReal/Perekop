#pragma once
#include <PK/Container/memory.hpp>

namespace pk {
    template <typename T> class span {
        T* data{nullptr}, *cap{nullptr};
        public:
            constexpr const T* begin() const noexcept { return data; }
            constexpr const T* end()   const noexcept { return cap; }
            constexpr T* begin() noexcept { return data; }
            constexpr T* end()   noexcept { return cap; }

            constexpr unsigned size() const noexcept { return cap - data; }

            constexpr T& operator[](unsigned i) noexcept { return data[i]; }
            constexpr const T& operator[](unsigned i) const noexcept { return data[i]; }

            constexpr span() noexcept = default;
            constexpr span(T* single) noexcept: data(single), cap(single+1) {}
            constexpr span(T* first, T* end) noexcept: data(first), cap(end) {}
            constexpr span(T* first, unsigned n) noexcept: data(first), cap(first + n) {}
            
            template <unsigned L> constexpr span(T (&items)[L]): data(items), cap(items+L) {}

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
                for (unsigned i = 0; i < size(); i++) result[i] = f(data[i], std::forward(args...));
            }
    };

    template <> class span<void> {
        const void* data{nullptr}, *cap{nullptr};
        public:
            constexpr const void* begin() const noexcept { return data; }
            constexpr const void* end()   const noexcept { return cap; }
            constexpr unsigned size() const noexcept { return (char*)cap - (char*)data; }

            constexpr explicit operator bool() const noexcept { return data != nullptr; }
            constexpr bool operator!()         const noexcept { return data == nullptr; }

            template <typename T> span(const span<T> from) noexcept:
                data((const void*) from.begin()),
                cap((const void*) from.end())
            {}

            template <typename T> const T& read(unsigned index) noexcept {
                return *((const T*)data + index);
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