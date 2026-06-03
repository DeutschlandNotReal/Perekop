#pragma once
#include <pkutil/Array.hpp>

namespace pk {
    template <typename T> class Set {
        using I = decltype(T::id);
        // type T got to have 'id' member
        Array<T> data;
        Array<I> index;

       public:
            Set(uint L = 8): data(L), index(L) {};

            T& operator[](I i) const { return data[index[i]]; }
            T* begin() const { return data.begin(); }
            T* end() const { return data.end(); }
            
            template <typename... A> T& insert(A&&... args) {
                index.push(index.size()+1);
                data.emplace(args...);
                data.back().id = index.back();
                return data.back();
            }

            void remove(I i) {
                T& back = data.popout();
                index[back.id] = index[i];
                data[index[i]] = back;
            }

            uint size() { return data.size(); }
    };
}