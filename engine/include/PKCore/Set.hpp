#pragma once
#include <PKCore/vector.hpp>
#include <type_traits>

namespace pk {
    template <typename T> class set {
        using I = decltype(T::id);

        struct slot {
            union {
                // so that v's destructor isn't called when item is destroyed
                T v;
            };
 
            template <typename... A> slot(I id, A... args) {
                new (&v) T(forward_cast<A>(args)...);
                v.id = id;
            }

            slot(slot&& b): v(rvalue_cast(b.v)) {}

            void nullify() {
                if constexpr (!std::is_trivially_destructible_v<T>)
                    if (v.id) v.~T(); 
                v.id = 0;
            }

            ~slot() { nullify(); }

            operator T&() { return v; }
        };

        vector<slot> data; 
        vector<I> free;
        // I assumed as unsigned type (u16, u32)
        I items{0};

        struct set_iterator {
            T *cur{nullptr}, *end{nullptr};

            void operator++() {
                while (++cur < end && !cur->id);
            }        

            bool operator!=(const set_iterator&) const { return cur != end; }

            T& operator*() const { return *cur; }
        };

        public:
            T& operator[](I i) const { 
                return data[i - 1]; // 1-index, 0 is for null
            }

            set_iterator begin() const { 
                T *cur = (T*)data.begin();
                while (cur != (T*)data.end() && !cur->id) ++cur;

                return {cur, (T*)data.end()}; 
            }

            set_iterator end() const { return {}; }
            
            template <typename... A> T& insert(A&&... args) {
                ++items;

                if (!free.is_empty()) {
                    I id = free.popout_back();
                    new (&data[id-1]) slot(id, forward_cast<A>(args)...);

                    return data[id-1].v;
                } else {
                    return data.emplace_back(data.size() + 1, forward_cast<A>(args)...);
                }
            }

            void remove(I id) {
                --items;
                if (id == data.size()) data.pop_back();
                else free.push_back(id);

                data[id-1].nullify();
            }

            bool valid(I id) {
                return !(id-1 > data.size() || !data[id-1].v.id);
            }

            uint32_t size()     const { return items; }
            uint32_t capacity() const { return data.capacity(); }
    };
}