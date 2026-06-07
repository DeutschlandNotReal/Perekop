#pragma once
#include <PKCore/Memory.hpp>
#include <initializer_list>
#include <type_traits>

namespace pk {
    // static array
    template <typename T> class array {
        T *data{nullptr}, *cap{nullptr};
        void destruct() { 
            if constexpr (!std::is_trivially_destructible_v<T>)
                for (T* i = data; i < cap; i++) i->~T();
        }

        public:
            [[nodiscard]] array() = default;
            [[nodiscard]] array(uint32_t len): data(PKAlloc<T>(len)), cap(data + len) {};
            [[nodiscard]] array(std::initializer_list<T> items): data(PKAlloc<T>(items.size())) {
                cap = data + items.size();
                PKCopy(data, items.begin(), items.size());
            }

            template <uint32_t L> 
            [[nodiscard]] array(T (&items)[L]): data(PKAlloc<T>(L)) { 
                cap = data + L; 
                PKCopy(data, items, L); 
            }

            [[nodiscard]] explicit operator bool() const { return data != nullptr; }
            [[nodiscard]] bool operator!() const { return data == nullptr; }

            [[nodiscard]] array(array&& b): data(b.data), cap(b.cap) { b.data = b.cap = nullptr; }
            array& operator=(array&& b) {
                if (&b == this) return *this;
                if (data) destruct();
                if (size() != b.size()) { PKFree(data); data = PKAlloc<T>(b.size()); }

                PKMove(data, b.data, b.size());
                return *this;
            }

            [[nodiscard]] array(const array& b): data(
                PKAlloc<T>(b.size())) {
                cap = data + b.size();
                PKCopy(data, b.data, b.size());
            }

            array& operator=(const array& b) {
                if (&b == this) return *this;
                if (data) destruct();
                if (size() != b.size()) { PKFree(data); data = PKAlloc<T>(b.size()); }

                PKCopy(data, b.data, b.size());
                return *this;
            }
            
            T& operator[](uint32_t i) const { return data[i]; }
            T& back()  const { return *(cap-1); }
            T* begin() const { return data; }
            T* end()   const { return cap; }

            uint32_t size() const { return cap - data; }
            uint32_t capacity() const { return cap - data; }

            ~array() {
                if (data) destruct();
                PKFree(data);
            }
    };

    // dynamic array (std::vector substitute!!)
    template <typename T> class dynarray {
        T *data{nullptr}, *cur{nullptr}, *cap{nullptr};
        void destruct() {
            if constexpr (!std::is_trivially_destructible_v<T>)
                for (T* i = data; i < cur; i++) i->~T();
        }

        void resize(uint32_t ncap) {
            if (!data) { data = cur = PKAlloc<T>(ncap); cap = data + ncap; return; }
            uint32_t len = size();
            T* ldata = data;
            data = PKAlloc<T>(ncap);
            PKMove(data, ldata, size());
            cur = data + len;
            cap = data + ncap;
            PKFree(ldata);
        }

        T* next() {
            if (cur == cap) resize(capacity() * 1.5 + 8);
            return cur++;
        }

        public:
            [[nodiscard]] dynarray() = default;
            [[nodiscard]] dynarray(uint32_t len): data(PKAlloc<T>(len)), cap(data + len) {};
            [[nodiscard]] dynarray(std::initializer_list<T> items): data(PKAlloc<T>(items.size())) {
                cap = data + items.size();
                PKCopy(data, items.begin(), items.size());
            }
            
            template <uint32_t L> 
            [[nodiscard]]dynarray(T (&items)[L]): data(PKAlloc<T>(L)) { 
                cap = cur = data + L; 
                PKCopy(data, items, L); 
            }
            
            [[nodiscard]] explicit operator bool() const { return data != nullptr; }
            [[nodiscard]] bool operator!() const { return data == nullptr; }

            T& operator[](uint32_t i) const { return data[i]; }
            T& back()  const { return *(cur-1); }
            T* begin() const { return data; }
            T* end()   const { return cur; }
            [[nodiscard]] bool empty() const { return cap == data; }
            [[nodiscard]] bool full()  const { return cap == cur; }
            
            uint32_t size() const { return cur - data; }
            uint32_t capacity() const { return cap - data; }

            [[nodiscard]] dynarray(dynarray&& b): data(b.data), cap(b.cap) { b.data = b.cap = nullptr; }
            dynarray& operator=(dynarray&& b) {
                if (&b == this) return *this;
                if (data) destruct();
                if (size() < b.size()) { PKFree(data); data = PKAlloc<T>(b.size()); }

                PKMove(data, b.data, b.size());
                return *this;
            }

            [[nodiscard]] dynarray(const dynarray& b): data(PKAlloc<T>(b.size())) {
                cap = data + b.size();
                PKMove(data, b.data, b.size());
            }
            
            dynarray& operator=(const dynarray& b) {
                if (&b == this) return *this;
                if (data) destruct();
                if (size() < b.size()) { PKFree(data); data = PKAlloc<T>(b.size()); }

                PKMove(data, b.data, b.size());
                return *this;
            }

            void clear() { 
                if constexpr (!std::is_trivially_destructible_v<T>)
                    for (T* i = cur-1; i >= data; i--) i->~T();
                cur = data; 
            }

            void pop() { 
                if constexpr(!std::is_trivially_destructible_v<T>)
                    (--cur)->~T();
                else --cur;
            }

            [[nodiscard]] T&& popout() { 
                return *--cur;
            }

            void reserve(uint32_t new_size) {
                if (new_size > capacity()) resize(new_size);
            }

            template <typename... A> T& emplace(A&&... args) {
                new (next()) T(args...);
                return back(); 
            }

            // unsafe emplace
            template <typename... A> T& u_emplace(A&&... args) {
                new (cur++) T(args...);
                return back();
            }

            T& push(const T& item) { 
                PKCopy(next(), &item, 1);
                return back();
            }

            // unsafe push
            T& u_push(const T& item) {
                PKCopy(cur++, &item);
                return back();
            }

            void push(std::initializer_list<T> items) {
                reserve(size() + items.size());
                PKCopy(cur, items.begin(), items.size());
                cur += items.size();
            }
    };

    // For viewers who just read array data (dont care if static or dynamic, only reads)
    template <typename T> class arrayview {
        T *data{nullptr}, *cur{nullptr};
        public:
            T* begin() const { return data; }
            T* end()   const { return cur; }
            T& operator[](uint32_t i) const { return data[i]; }
            uint32_t size() const { return cur - data; }

            [[nodiscard]] arrayview(const dynarray<T>& a): data(a.begin()), cur(data + a.size()) {}
            [[nodiscard]] arrayview(const array<T>& a): data(a.begin()), cur(data + a.size()) {}
            template <uint32_t L> arrayview(T (&items)[L]): data(items), cur(items+L) {}
    };
}