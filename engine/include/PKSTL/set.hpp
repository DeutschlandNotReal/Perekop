#pragma once
#include <PKSTL/vector.hpp>

namespace pk {
    template <typename T>
    class set {
        using index_t = decltype(T::id);
        vector<T, index_t> dense{16 };
        vector<index_t> sparse;

        T& at(index_t i) noexcept { return dense[sparse[i]]; }

        const T& at(index_t i) const noexcept { return dense[sparse[i]]; }

        public:
            set() = default;
            class handle {
                friend set;
                handle(set* set, index_t index) noexcept: set(set), index(index - 1) {}

                set* set; index_t index;
                public:
                    [[nodiscard]] T* operator->() noexcept { return &set->at(index); }
                    [[nodiscard]] const T* operator->() const noexcept { return &set->at(index); }
                    [[nodiscard]] T& operator*() noexcept { return set->at(index); }
                    [[nodiscard]] const T& operator*() const noexcept { return set->at(index); }
                    [[nodiscard]] operator T&() noexcept { return set->at(index); }
                    [[nodiscard]] operator const T&() const noexcept { return set->at(index); }

                    handle& operator=(T&& val) { set->at(index) = std::move(val); return *this; }
                    handle& operator=(const T& val) { set->at(index) = val; return *this; }

                    handle(handle&& b) noexcept: set(b.set), index(b.index) { b.index = 0; b.set = nullptr; }
                    handle& operator=(handle&& b) noexcept { if (&b != this) { set = b.set; index = b.index; b.set = nullptr; b.index = 0; } return *this; }

                    [[nodiscard]] T delist() {
                        T item = std::move(set->at(index));
                        
                        auto& dense = set->dense;
                        auto& sparse = set->sparse;

                        index_t dense_index = sparse[index];

                        dense[dense_index] = std::move(dense.back()); // swap pop
                        dense.pop();
        
                        set = nullptr; index = 0;
                        return item;
                    }

                    index_t id() const noexcept { return index + 1; }

                    void remove() { delist(); }
            };
            friend handle;

            const handle operator[](index_t i) const noexcept { return {this, i}; }
            handle operator[](index_t i) noexcept { return {this, i}; }

            const T* begin() const noexcept { return dense.begin(); }
            T* begin() noexcept { return dense.begin(); }
            const T* end() const noexcept { return dense.end(); }
            T* end() noexcept { return dense.end(); }

            const handle front() const noexcept { return {this, dense.front().id}; }
            handle front() noexcept { return {this, dense.front().id}; }
            const handle back() const noexcept { return {this, dense.back().id}; }
            handle back() noexcept { return {this, dense.back().id}; }

            index_t size() const noexcept { return dense.size(); }

            template <typename... arg>
            handle insert(arg&&... args) requires(std::is_constructible_v<T, arg...>) {
                T& item = dense.emplace(std::forward<arg>(args)...);
                sparse.push(dense.size() - 1);

                item.id = sparse.size();
                return {this, item.id};
            }

            handle insert(T&& b) {
                T& item = dense.emplace(std::forward(b));
                sparse.push(dense.size() - 1);

                item.id = sparse.size();
                return {this, item.id};
            }
    };

    template <typename T>
    using set_handle = set<T>::handle;
}