#pragma once
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <utility>

namespace pk {
    template <typename T> class Array {
        T* _data = nullptr;
        unsigned int _cur = 0, _len = 1;

        T* _next() {
            if (_cur == _len) { 
                _len = _len * 2 + 1;          
                T* new_data = (T*)realloc(_data, _len * sizeof(T));
                if (new_data) _data = new_data;
            }
            return _data + _cur++; 
        }

        public:
            ~Array() { free(_data); }
            Array(): _data((T*)malloc(sizeof(T))) {}
            Array(unsigned int cap): _data((T*)malloc(sizeof(T) * cap)) {}

            Array(Array&& ar): _data(ar._data), _cur(ar._cur), _len(ar._len) { 
                ar._data = nullptr; ar._cur = 0; ar._len = 0; 
            }

            Array(const Array& ar): _data((T*)malloc(sizeof(T)*(ar._len))), _cur(ar._cur), _len(ar._len) {
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
                _data = ar._data; _len = ar._len; _cur = ar._cur;
                ar._data = nullptr; ar._len = 0; ar._cur = 0;
                return *this;
            }
            
            T& operator[](unsigned int i) const {
                return _data[i];
            }

            void push(T item) { new (_next()) T(item); }
            
            template <typename... args> void emplace(args&&... values) {
                new (_next()) T(std::forward<args>(values)...);
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

            [[nodiscard]] unsigned int length() const noexcept { return _cur; }
            [[nodiscard]] unsigned int capacity() const noexcept { return _len; }
            [[nodiscard]] unsigned int size() const noexcept { return _cur * sizeof(T); }
            [[nodiscard]] T* data() const noexcept { return _data; }
            [[nodiscard]] bool empty() const noexcept { return _cur == 0; }

            void clear() noexcept { _cur = 0; }
            void resize(unsigned int new_size) {
                T* prev = _data;
                _len = new_size;
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
                ar._len = real_length;
                ar._cur = real_length;
                std::memcpy(ar._data, _data + from, sizeof(T) * real_length);
                return ar;
            }
    };
}