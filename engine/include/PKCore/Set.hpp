#include <PKCore/vector.hpp>

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
        // type T got to have 'id' member
        vector<T> dense; 
        vector<I> sparse; 
        I top{0};

       public:
            set(uint32_t L = 8): dense(L), sparse(L) {};

            T& operator[](I i) const { 
                // index is 1 based so 0 can represent empty

                #if defined(PK_DEBUG_SET) && (PK_DEBUG_SET & 0b0001)
                    if (i > top) printf("\033[31m(%s) ERROR: set<%s> OOB [%i/%u]\n\033[0m", PK_DEBUG, classname<T>, i, top);
                #endif
                return dense[sparse[i-1]]; 
            }

            T* begin() const { return dense.begin(); }
            T* end() const { return dense.end(); }
            
            template <typename... A> T& insert(A&&... args) {
                dense.emplace(args...).id = top + 1;

                #if defined(PK_DEBUG_SET) && (PK_DEBUG_SET & 0b0010)
                    printf("(%s) set<%s> insert (id %i)\n", PK_DEBUG, classname<T>, top+1);
                #endif

                sparse.emplace(dense.size() - 1);
                sparse.reserve(++top);

                return dense.back();
            }

            void remove(I i, T* recall = nullptr) {
                if (i > top || i <= 0) return;
                I dense_id = sparse[i-1];
        
                #if defined(PK_SET_DEBUG) && (PK_DEBUG_SET & 0b0100)
                    printf("(%s) set<%s> remove (id %i)\n", PK_DEBUG, classname<T>, i);
                #endif

                if (dense_id != dense.size() - 1) {
                    sparse[dense.back().id-1] = dense_id;
                    dense.swap(dense_id, dense.size() - 1);
                }

                if (recall)
                    new (recall) T(dense.popout());
                else
                    dense.pop();
            }

            uint32_t size() const { return dense.size(); }
            uint32_t ids() const { return top; }
    };
}