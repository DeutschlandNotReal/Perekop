#pragma once
#include <PKCore/vector.hpp>
#include <type_traits>

namespace pk {
    template <typename T> class set {
        using I = decltype(T::id);

        vector<T> data; 
        vector<I> free; 
        I items{0};

        struct set_iterator {
            T *cur{nullptr}, *end{nullptr};

            void operator++() {
                while (++cur < end && cur->id == 0);
            }

            bool operator!=(const set_iterator&) const { return cur != end; }

            T& operator*() const { return *cur; }
        };

        public:
            T& operator[](uint16_t i) const { 
                return data[i - 1]; // 1-index, 0 is for null
            }

            set_iterator begin() const { return {.cur = data.begin(), .end = data.end()}; }
            set_iterator end()   const { return {}; }
            
            template <typename... A> T& insert(A&&... args) {
                ++items;

                if (!free.is_empty()) {
                    I id;
                    free.popout(&id);
                    new (&data[id-1]) T(forward_cast<A>(args)...);
                    data[id-1].id = id;

                    return data[id-1];
                } else {
                    I id = data.size() + 1;
                    data.emplace(forward_cast<A>(args)...).id = id;
                    return data.back();
                }
            }

            void remove(I id) {
                --items;
                if (id == data.back()) {
                    data.pop();
                    data[data.size()].id = 0;
                } else {
                    if constexpr(!std::is_trivially_destructible_v<T>)
                        data[id - 1].~T();

                    data[id - 1].id = 0;
                    free.emplace(id);
                }
            }

            void removeout(I id, T* to) {
                --items;
                if (id == data.size()) { 
                    data.popout(to); 
                } else { 
                    new (to) T(rvalue_cast(data[id - 1]));
                }
            }

            bool valid(I id) {
                return !(id <= 0 || id > data.size() || !data[id-1].id);
            }

            uint32_t size()     const { return items; }
            uint32_t capacity() const { return data.capacity(); }
            uint32_t memsize()  const { return data.capacity() * sizeof(T) + free.capacity() * sizeof(I); }
    };
}