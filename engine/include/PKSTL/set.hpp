#pragma once
#include <PKSTL/vector.hpp>

namespace pk {
    template <typename T>
    class set {
        using index_t = decltype(T::id);
        vector<T, index_t> dense;
        vector<index_t> sparse;

        public:
            set() = default;

            const T& operator[](unsigned i) const noexcept { return dense[sparse[i-1]]; }
            T& operator[](unsigned i) noexcept { return dense[sparse[i-1]]; }

            const T* begin() const noexcept { return dense.begin(); }
            const T* end() const noexcept { return dense.end(); }
            
            T* begin() noexcept { return dense.begin(); }
            T* end() noexcept { return dense.end(); }

            index_t size() const noexcept { return dense.size(); }

            template <typename... arg>
            T& insert(arg&&... args) {
                T& item = dense.emplace(std::forward<arg>(args)...);
                sparse.push(dense.size() - 1);

                item.id = sparse.size();
                return item;
            }

            void remove(index_t index) {
                index_t dense_index = sparse[index];

                dense[dense_index] = std::move(dense.back()); // swap pop
                sparse[dense[dense_index].id] = dense_index;
            }
    };
}