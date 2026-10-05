#pragma once
#include <PK/Core/memory.hpp>

namespace pk {
    template <typename T, unsigned n = 0> 
    class array {
        T data[n];
        public:
            constexpr array() noexcept = default;
            constexpr array(const T (&src)[n]) noexcept {
                pk::copy(data, src, n);
            }

            template <typename... arg>
            constexpr array(arg... args) noexcept requires(sizeof...(args) == n && (std::is_convertible_v<arg, T> && ...))  : 
                data{static_cast<T>(args)...} 
            {}

            template <unsigned... ns>
            constexpr array(const array<T, ns>&... subarrays) requires((... + ns) <= n) {
                unsigned i = 0;
                ([this, &i](const auto& sub){ 
                    pk::copy(data + i, sub.begin(), sub.size()); i+= sub.size(); 
                }(subarrays), ...);
            }

            constexpr const T* begin() const noexcept { return data; }
            constexpr const T* end()   const noexcept { return data + n; }
            constexpr T* begin() noexcept { return data; }
            constexpr T* end()   noexcept { return data + n; }
            constexpr unsigned size() const noexcept { return n; }

            constexpr const T& operator[](unsigned i) const noexcept { return data[i]; }
            constexpr T& operator[](unsigned i) noexcept { return data[i]; }
    };

    template <typename T>
    class array<T, 0> {
        T* data{nullptr};
        unsigned len{0};

        public:
            constexpr array() noexcept = default;

            constexpr array(unsigned len) noexcept: 
                len(len),
                data(pk::alloc<T>(len))
            {
                if constexpr (std::is_trivial_v<T>) {
                    std::memset(data, 0, len * sizeof(T));
                } else for (unsigned i = 0; i < len; i++)
                    data[i] = T();
            }

            constexpr array(std::initializer_list<T> list) noexcept {
                len = list.size();
                len = pk::alloc<T>(list.size());
                pk::move(data, list.begin(), list.size());
            }

            constexpr const T* begin() const noexcept { return data; }
            constexpr const T* end()   const noexcept { return data + len; }
            constexpr T* begin() noexcept { return data; }
            constexpr T* end()   noexcept { return data + len; }
            constexpr unsigned size() const noexcept { return len; }

            constexpr const T& operator[](unsigned i) const noexcept { return data[i]; }
            constexpr T& operator[](unsigned i) noexcept { return data[i]; }

            constexpr void clear() noexcept {
                if (std::is_trivially_destructible_v<T>) {
                    T* last = data + len;
                    while (--last >= data) last->~T();
                } 
            }

            constexpr array(array&& val) noexcept:
                data(std::exchange(val.data, nullptr)),
                len(std::exchange(val.len, 0))
            {}

            constexpr array& operator=(array&& val) noexcept {
                if (data) {
                    clear();
                    pk::free(data);
                }

                data = std::exchange(val.data, nullptr);
                len  = std::exchange(val.len, 0);
                return *this;
            }

            constexpr array(const array& val) noexcept:
                data(pk::alloc<T>(val.size())),
                len(val.size()) 
            {
                pk::copy(data, val.begin(), val.size());
            }

            constexpr array& operator=(const array& val) noexcept {
                if (data && val.size() == size()) {
                    clear();
                } else {
                    if (data) pk::free(data);
                    data = pk::alloc<T>(val.size());
                }

                pk::copy(data, val.begin(), val.size());
            }

            constexpr void deallocate() noexcept {
                if (data) {
                    clear();
                    pk::free(data);
                    len = 0; data = nullptr;
                }
            }
            
            ~array() { deallocate(); }
    };
}