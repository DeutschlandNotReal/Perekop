#include <PKCore/vector.hpp>
#include <type_traits>

#if defined(PK_DEBUG_SET) && PK_DEBUG_SET != 0
#include <PKCore/debug.hpp>
// debug flags:
// 0b0001: index
// 0b0010: insert
// 0b0100: remove
#endif

namespace pk {
    template <typename T> class set {
        using I = decltype(T::id);
        vector<T> data; vector<I> free; uint16_t items{0};

        struct set_iterator {
            T *cur{nullptr}, *end{nullptr};

            void operator++() {
                while (++cur < end && cur->id == 0);
            }

            bool operator!=(const set_iterator&) const { return cur != end; }

            T& operator*() const { return *cur; }
        };

        public:
            T& operator[](I i) const { 
                // index is 1 based so 0 can represent empty

                #if defined(PK_DEBUG_SET) && (PK_DEBUG_SET & 0b0001)
                    if (i > data.size()) printf("\033[31m(%s) ERROR: set<%s> OOB [%i/%u]\n\033[0m", PK_DEBUG, classname<T>, i, data.size());
                #endif

                return data[i - 1]; 
            }

            set_iterator begin() const { return {.cur = data.begin(), .end = data.end()}; }
            set_iterator end()   const { return {}; }
            
            template <typename... A> T& insert(A&&... args) {
                ++items;

                if (!free.empty()) {
                    I id = free.popout();
                    new (&data[id - 1]) T(args...);
                    data[id - 1].id = id;

                    #if defined(PK_DEBUG_SET) && (PK_DEBUG_SET & 0b0010)
                        printf("(%s) set<%s> insert empty (id %i)\n", PK_DEBUG, classname<T>, id);
                    #endif

                    return data[id - 1];
                } else {
                    I id = data.size() + 1;
                    data.emplace(args...).id = id;

                    #if defined(PK_DEBUG_SET) && (PK_DEBUG_SET & 0b0010)
                        printf("(%s) set<%s> insert back (id %i)\n", PK_DEBUG, classname<T>, id);
                    #endif
                    
                    return data[id - 1];
                }
            }

            void remove(I id, T* recall = nullptr) {
                --items;

                if (id == data.back()) {
                    if (recall)
                        new (recall) T((T&&) data.popout());
                    else { data.pop(); };
                } else {
                    if (recall)
                        new (recall) T((T&&) data[id - 1]);
                    else if constexpr(!std::is_trivially_destructible_v<T>)
                        data[id - 1].~T();
                    data[id - 1] = 0;
                    free.emplace(id);
                }

                #if defined(PK_DEBUG_SET) && (PK_DEBUG_SET & 0b0100)
                    printf("(%s) set<%s> remove (id %i)\n", PK_DEBUG, classname<T>, id);
                #endif
            }

            uint32_t size() const { return items; }
            uint32_t capacity() const { return data.capacity(); }
            uint32_t memsize() const { return data.capacity() * sizeof(T) + free.capacity() * sizeof(I); }
    };
}