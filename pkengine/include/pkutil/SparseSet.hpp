#pragma once
#include <pkutil/Array.hpp>

namespace pkutil {
    template <typename T> class SparseSet {
        using I = decltype(T::id);
        // type T got to have 'id' member (mesh & model)
        T* data; I* indices;
        uint cur{0}, cap{0};

        void resize(int new_size) {
            realloc(data, cap, new_size);
            realloc(indices, cap, new_size);
            cap = new_size;
        }

       public:
            T& operator[](I index) const noexcept {
                return data[indices[index]];
            }

            T* begin() const noexcept { return data; }
            T* end() const noexcept { return data + cur; }
            
            template <typename... A> T& insert(A&&... args) {
                if (cur == cap) resize(cap * 2 + 1);
                indices[cur] = cur;
                new (data + cur) T(args...);
                data[cur].id = cur;
                return data[cur++];
            }

            void remove(I index) {
                T& back = data[--cur];
                indices[back.id] = indices[index];
                operator[](index) = back;
            }

            void remove(const T& item) {
                remove(item.id);
            }
        
            ~SparseSet() {
                free(data); free(indices);
            }


    };
}