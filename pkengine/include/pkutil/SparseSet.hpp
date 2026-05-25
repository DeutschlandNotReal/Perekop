#pragma once
#include <pkutil/Array.hpp>

namespace pk {
    template <typename T> class SparseSet {
        using I = decltype(T::id);
        // type T got to have 'id' member (mesh & model)
        Array<T> data;
        Array<I> index;

       public:
            SparseSet(uint L = 8): data(L), index(L) {};

            T& operator[](I i) const noexcept {
                return data[index[i]];
            }

            T* begin() const noexcept { 
                return data.begin();
            }

            T* end() const noexcept { 
                return data.end();
            }
            
            template <typename... A> T& insert(A&&... args) {
                I id = index.size();
                index.push(id);
                data.push(args...);
                data.back().id = id;
                return data.back();
            }

            void remove(I i) {
                T& back = data.popout();
                index[back.id] = index[i];
                data[index[i]] = back;
            }
    };
}