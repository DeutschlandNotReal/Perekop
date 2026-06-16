#pragma once
#include <PKCore/memory.hpp>
#include <initializer_list>
#include <type_traits>

namespace pk {
    // dynamic array std::vector substitute
    template <typename T> class vector {
          T *data{nullptr}, *cur{nullptr}, *cap{nullptr};

        void resize(uint32_t newcap) {
            if (!data) { 
                data = cur = pk::alloc<T>(newcap); 
                cap = data + newcap; 
                return; 
            }

            uint32_t len = size();
            T* ldata = data;
            data = pk::alloc<T>(newcap);
            pk::move(data, ldata, len);
            pk::free(ldata);
            cap = data + newcap;
            cur = data + len;
        }

        public:
            vector() = default;
            vector(uint32_t len): data(pk::alloc<T>(len)) { cap = data + len; cur = data; }
            vector(const vector &b): 
                data(pk::alloc<T>(b.size())) {
                cap = cur = data + b.size();
                pk::copy(data, b.data, b.size());
            }
            vector(vector &&b): 
                data(b.data), cap(b.cap), cur(b.cur) { 
                b.data = b.cap = b.cur = nullptr; 
            }
            vector(std::initializer_list<T> items): data(pk::alloc<T>(items.size())) {
                cap = cur = data + items.size();
                pk::copy(data, items.begin(), items.size());
            }
            template <uint32_t L> vector(const T (&items)[L]): data(pk::alloc<T>(L)) { 
                cap = cur = data + L; 
                pk::copy(data, items, L); 
            }

            explicit operator bool() const { return data != nullptr; }
            bool operator!()         const { return data == nullptr; }

            T& operator[](uint32_t i) const { return data[i]; }

            T& back()  const { return *(cur-1); }
            T* begin() const { return data; }
            T* end()   const { return cur; }

            bool is_empty() const { return cur == data; }
            bool is_full()  const { return cap == cur; }
            bool in_range(T* ptr) const { return ptr >= data && ptr < cap; }
            
            uint32_t size()     const { return cur - data; }
            uint32_t capacity() const { return cap - data; }
            uint32_t bytesize() const { return size() * sizeof(T); }

            vector& operator=(const vector &b) {
                if (&b == this) return *this;

                if (!data || size() < b.size()) {
                    if (data) { clear(); pk::free(data); }
                    data = pk::alloc<T>(b.size()); 
                    cap = data + b.size(); 
                };

                pk::copy(data, b.data, b.size());
                cur = data + b.size();

                return *this;
            }

            void grow() { 
                resize(capacity() * 1.5 + 8); 
            }

            void clear() { 
                if constexpr (!std::is_trivially_destructible_v<T>)
                    while (cur > data) (--cur)->~T();
                else cur = data;
            }

            void pop_back() {
                if constexpr (!std::is_trivially_destructible_v<T>) (--cur)->~T(); else --cur;
            }

            void popout_back(T* to) {
                new (to) T(rvalue_cast(*--cur));
            }

            void reserve(uint32_t new_size) {
                if (new_size > capacity()) resize(new_size);
            }

            T& emplace_back() {
                if (is_full()) grow();
                new (cur) T();
                return *(cur++);
            }

            template <typename... A> T& emplace_back(A&&... args) {
                if (is_full()) grow();
                new (cur) T(forward_cast<A>(args)...);
                return *(cur++);
            }

            // at must be less than or equal to size
            template <typename... A> T& emplace(uint32_t index, A&&... args) {
                if (is_full()) grow();
                if (index == size()) return emplace_back(forward_cast<A>(args)...);
                
                pk::rshift(data + index, size() - index);
                new (data + index) T(forward_cast<A>(args)...);
                ++cur;
                return data[index];
            }

            T& push_back(const T& item) {
                if (is_full()) grow();
                new (cur) T(item);
                return *(cur++);
            }

            // at must be less than or equal to size
            T& push(uint32_t at, const T& item) {
                if (is_full()) grow();
                if (at == size()) return push_back(item);
                
                pk::rshift(data + at, size() - at);
                new (data + at) T(item);
                ++cur;
                return data[at];
            }

            void push_back(std::initializer_list<T> items) {
                if (size() + items.size() > capacity()) resize(size() + items.size());

                pk::copy(cur, items.begin(), items.size());
                cur += items.size();
            }

            ~vector() {
                if (data) { clear(); pk::free(data); }
                data = cur = cap = nullptr;
            }
    };

    // refers to MUTABLE chunk of memory, std::span substitute
    template <typename T> class span {
        T* data{nullptr}, *cap{nullptr};
        public:
            T* begin() const { return data; }
            T* end()   const { return cap; }
            uint32_t size() const { return cap - data; }

            T& operator[](uint32_t i) const { return data[i]; }

            span(T* single): data(single), cap(single+1) {}
            span(T* first, T* end): data(first), cap(end) {}
            span(T* first, uint32_t n): data(first), cap(first + n) {}
            span(vector<T> &vec): data(vec.begin()), cap(vec.end()) {}

            template <uint32_t L> span(T (&items)[L]): data(items), cap(items+L) {}

            explicit operator bool() const { return data != nullptr; }
            bool operator!()         const { return data == nullptr; }
    };
}