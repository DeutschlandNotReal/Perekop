#include <pkutil/Array.hpp>

namespace pk {
    template <typename T> class Set {
        using I = decltype(T::id);
        // type T got to have 'id' member
        Array<T> dense; Array<I> sparse; I top{0};

       public:
            Set(uint L = 8): dense(L), sparse(L) {};

            T& operator[](I i) const { 
                // index is 1 based so 0 can represent empty
                return dense[sparse[i-1]]; 
            }

            T* begin() const { return dense.begin(); }
            T* end() const { return dense.end(); }
            
            template <typename... A> T& insert(A&&... args) {
                dense.emplace(args...).id = top + 1;
                sparse.reserve(++top);
                sparse.emplace(dense.size() - 1);

                return dense.back();
            }

            void remove(I i, T* recall = nullptr) {
                if (i > top || i <= 0) return;
                I dense_id = sparse[i-1];
        
                if (dense_id != dense.size() - 1) {
                    sparse[dense.back().id-1] = dense_id;
                    dense.swap(dense_id, dense.size() - 1);
                }

                if (recall)
                    *recall = T((T&&) dense.popout());
                else
                    dense.pop();
            }

            uint size() const { return dense.size(); }
            uint ids() const { return top; }
    };
}