
#pragma once
#include <PKCore/vector.hpp>

namespace pk {
    template <typename T> class slot_map {
        using I = decltype(T::id);

        struct slot {
            // a union so that destructor isn't called
            union { T content; };
 
            template <typename... A> slot(I id, A... args) {
                new (&content) T(forward_cast<A>(args)...);
                content.id = id;
            }

            slot(slot&& b): content(rvalue_cast(b.content)) {}

            void nullify() {
                if (!content.id) return;
                content.~T();
                content.id = 0;
            }

            ~slot() { nullify(); }

            operator T&() { return content; }
        };

        vector<slot> slots; vector<I> free; I count{0};

        struct sm_iterator {
            slot *cur{nullptr}, *end{nullptr};
            
            void operator++() {
                while (++cur != end && !cur->content.id);
            }        

            bool operator!=(const sm_iterator&) const { return cur != end; }

            T& operator*() const { return cur->content; }
        };

        public:
            T& operator[](I i) const { 
                return slots[i - 1]; // 1-index, 0 is for null
            }

            sm_iterator begin() { 
                slot *cur = slots.begin();
                while (cur != slots.end() && !cur->content.id) ++cur;

                return {cur, slots.end()};
            }

            sm_iterator end() { return {}; }
            
            template <typename... A> T& insert(A&&... args) {
                ++count;

                if (!free.is_empty()) {
                    I id = free.popout_back();
                    new (&slots[id-1]) slot(id, forward_cast<A>(args)...);

                    return slots[id-1].v;
                } else {
                    return slots.emplace_back(slots.size() + 1, forward_cast<A>(args)...);
                }
            }

            void remove(I id) {
                --count;
                if (id == slots.size()) slots.pop_back();
                else free.push_back(id);

                slots[id - 1].nullify();
            }

            bool valid(I id) {
                return !(id-1 > slots.size() || !slots[id - 1].v.id);
            }

            u32 size()     const { return count; }
            u32 capacity() const { return slots.capacity(); }
    };
}