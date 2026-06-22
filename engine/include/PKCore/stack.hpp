#pragma once 
#include <PKCore/memory.hpp>
#include <cstdint>
#include <type_traits>

// no dynamic resizing unlike pk::vector, lives in stack memory not heap
namespace pk {
    template <typename T, uint16_t L> class stack {
        alignas(T) char items[L * sizeof(T)];
        uint16_t cur{0};

        public:
            T& push(const T& item) {
                pk::copy((T*)items + cur++, &item);
                return back(); 
            }

            T& push(T&& item) {
                pk::move((T*)items + cur++, &item);
                return back();
            }

            void pop() {
                if constexpr (!std::is_trivially_destructible_v<T>)
                    ((T*)items)[--cur].~T();
                else
                    --cur;
            }
 
            void clear() {
                if constexpr (!std::is_trivially_destructible_v<T>)
                    for (uint16_t i = (cur - 1); i >= 0; i--)
                        ((T*)items)[i].~T();
                cur = 0;
            }

            [[nodiscard]] T&& popout() {
                return rvalue_cast(((T*)items)[--cur]);
            }

            T& back() { return ((T*)items)[cur-1]; }
            T* begin() const { return (T*)items; }
            T* end()   const { return (T*)items + cur; }

            T& operator[](uint16_t i) { return ((T*)items)[i]; }

            uint16_t size() const { return cur; }
            bool is_empty() const { return !cur; }
            bool is_full()  const { return cur == L; }

            ~stack() { clear(); }

    };
}