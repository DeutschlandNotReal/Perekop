#pragma once
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <type_traits>
#include <utility>

namespace pk {
    template <typename T> class Array {
        T* _data = nullptr;
        unsigned int _cur = 0, _cap = 1;

        T* _next() {
            if (_cur == _cap) { 
                _cap = _cap * 2 + 1;          
                _data = (T*)realloc(_data, _cap * sizeof(T));
            }

            return _data + _cur++; 
        }

        public:
            ~Array() { 
                if constexpr (std::is_destructible<T>()) 
                    for (int i = 0; i < _cur; i++) _data[i].~T();

                free(_data); 
            }

            Array(): _data((T*)malloc(sizeof(T))) {}
            Array(unsigned int cap): _data((T*)malloc(sizeof(T) * cap)) {}

            Array(Array&& ar): _data(ar._data), _cur(ar._cur), _cap(ar._cap) { 
                ar._data = nullptr; ar._cur = 0; ar._cap = 0; 
            }

            Array(const Array& ar): _data((T*)malloc(sizeof(T)*(ar._cap))), _cur(ar._cur), _cap(ar._cap) {
                std::memcpy(_data, ar._data, _cur * sizeof(T));
            }

            Array& operator=(const Array& ar) {
                if (&ar != this) {
                    free(_data);
                    new (this) Array(ar);
                }
                return *this;
            }

            Array& operator=(Array&& ar) noexcept {
                free(_data);
                _data = ar._data; _cap = ar._cap; _cur = ar._cur;
                ar._data = nullptr; ar._cap = 0; ar._cur = 0;
                return *this;
            }
            
            T& operator[](unsigned int i) const {
                return _data[i];
            }

            T& push(T item) {
                T* ptr = _next();
                new (ptr) T(item); 
                return *ptr;
            }
            
            template <typename... args> T& emplace(args&&... values) {
                T* ptr = _next();
                new (ptr) T(std::forward<args>(values)...);
                return *ptr;
            }

            T pop() { 
                return _data[--_cur];
            }

            T& swappop(unsigned int i) {
                _data[i] = _data[--_cur];
                return _data[i];
            }

            T& back() const {
                return _data[_cur-1];
            }

            [[nodiscard]] unsigned int size() const noexcept { return _cur; }
            [[nodiscard]] unsigned int capacity() const noexcept { return _cap; }
            [[nodiscard]] unsigned int memsize() const noexcept { return _cur * sizeof(T); }
            [[nodiscard]] T* data() const noexcept { return _data; }
            [[nodiscard]] bool empty() const noexcept { return _cur == 0; }

            void clear() noexcept { _cur = 0; }
            void resize(unsigned int new_size) {
                T* prev = _data;
                _cap = new_size;
                _cur = std::min(_cur, new_size);
                _data = (T*)malloc(sizeof(T) * new_size);
                for (int i = 0; i < _cur; i++) _data[i] = std::move(prev[i]);
                free(prev);
            }

            Array clone(unsigned int from, unsigned int length) const {
                if (from >= _cur) return Array();
                Array ar;
                unsigned int real_length = std::min(length, _cur - from);
                ar._data = (T*)malloc(sizeof(T) * real_length);
                ar._cap = real_length;
                ar._cur = real_length;
                std::memcpy(ar._data, _data + from, sizeof(T) * real_length);
                return ar;
            }
    };

    template <typename T> class Pool {
        struct PoolBlock {
            T* _data = nullptr; unsigned char _cur = 0;
            T& operator[](unsigned int I) const { return _data[I]; }
            PoolBlock(): _data((T*)malloc(sizeof(T) * 255)) {};
            ~PoolBlock() {
                for (unsigned char i = 0; i < _cur; i++) _data[i].~T();
                free(_data);
            }
        };
        Array<PoolBlock> blocks;

        T* _next() {
            if (blocks.back()._cur == 255) blocks.emplace();
            auto &back = blocks.back();
            return back._data + back._cur++;
        }

        public:
            Pool() { blocks.emplace(); }

            unsigned int size() const noexcept {
                return 256*(blocks.size()-1) + (unsigned int)blocks.back()._cur;
            }
            unsigned int memsize() const noexcept { return size() * sizeof(T); }
            unsigned int capacity() const noexcept { return blocks.size() * 255; }

            T& operator[](unsigned int I) const {
                return blocks[I >> 8][I & 255];
            }

            template <typename... args> T& emplace(args&&... params) {
                T* ptr = _next();
                new (ptr) T(std::forward<args>(params)...);
                return *ptr;
            }

            T& push(T item) {
                T* ptr = _next();
                new (ptr) T(item);
                return *ptr;
            }
    };
}