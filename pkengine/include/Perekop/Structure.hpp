#pragma once
#include <algorithm>
#include <cstring>
#include <initializer_list>
#include <type_traits>
#include <utility>

namespace pk {
    template <typename T, typename I = unsigned int> class Array {
        T* _data{nullptr};
        I _cur{0}, _cap{1};

        void _copy(const T* from, T* to, I l) const {
            if constexpr (std::is_trivial_v<T>) 
                std::memcpy(to, from, l * sizeof(T));
            else for (I i = I(0); i < l; i++)
                new (to + i) T(from[i]);
        }

        void _move(const T* from, T* to, I l) const {
            if constexpr (std::is_trivial_v<T>) 
                std::memcpy(to, from, l * sizeof(T));
            else for (I i = I(0); i < l; i++)
                new (to + i) T(std::move(from[i]));
        }

        public:
            void resize(I _newcap) {
                _cap = _newcap;
                T* _prev = _data;     
                _data = (T*)::operator new(_cap * sizeof(T));
                _copy(_prev, _data, _cur);
                ::operator delete(_prev);
            }

            T* begin() const noexcept { return _data; }
            T* end() const noexcept { return _data + _cur; }

            ~Array() { 
                if constexpr (!std::is_trivial_v<T>) 
                    for (I i = I(0); i < _cur; i++) _data[i].~T();

                ::operator delete(_data);
            }

            Array(): _data((T*)::operator new(sizeof(T))) {}

            Array(I cap): _data((T*)::operator new(sizeof(T) * cap)) {}

            Array(Array&& ar): _data(ar._data), _cur(ar._cur), _cap(ar._cap) { 
                ar._data = nullptr; ar._cur = 0; ar._cap = 0; 
            }

            Array(const Array& ar): _data((T*)::operator new(sizeof(T)*ar._cap)), _cur(ar._cur), _cap(ar._cap) {
                _copy(ar._data, _data, _cur);
            }

            Array& operator=(const Array& ar) {
                if (&ar != this) {
                    ::operator delete(_data);
                    new (this) Array(ar);
                }

                return *this;
            }

            Array& operator=(Array&& ar) noexcept {
                ::operator delete(_data);
                _data = ar._data; _cap = ar._cap; _cur = ar._cur;
                ar._data = nullptr; ar._cap = 0; ar._cur = 0;
                return *this;
            }
            
            T& operator[](I i) const {
                return _data[i];
            }

            T& push(T item) {
                if (full()) resize(_cap * 2 + 1);
                T* ptr = _data + _cur++;
                new (ptr) T(item); 
                return *ptr;
            }

            void push(std::initializer_list<T> items) {
                if (_cur + items.size() > _cap) 
                    resize(std::max(_cap * 2 + 1, _cur + (I)items.size()));

                _copy(items.begin(), _data + _cur, items.size());
                _cur += items.size();
            }
            
            template <typename... args> T& emplace(args&&... values) {
                if (full()) resize(_cap * 2 + 1);
                T* ptr = _data + _cur++;
                new (ptr) T(std::forward<args>(values)...);
                return *ptr;
            }

            T pop() noexcept {
                return T(std::move(_data[--_cur])); 
            }

            [[nodiscard]] T& back() const noexcept {
                return _data[_cur-1];
            }

            T& swappop(I i) noexcept {
                _data[i] = _data[--_cur];
                return _data[i];
            }

            [[nodiscard]] I size() const noexcept { return _cur; }
            [[nodiscard]] I capacity() const noexcept { return _cap; }
            [[nodiscard]] I bytes() const noexcept { return _cur * sizeof(T); }
            [[nodiscard]] T* data() const noexcept { return _data; }            
            [[nodiscard]] bool empty() const noexcept { return _cur == 0; }
            [[nodiscard]] bool full() const noexcept { return _cur == _cap; }

            void clear() noexcept { _cur = 0; }

            Array clone(I start, I length) const {
                if (start >= _cur) return Array();
                I l = std::min(length, _cur - start);
                Array cloned(l);
                cloned._cap = l; cloned._cur = l;
                _copy(_data + start, cloned._data, l);

                return cloned;
            }
    };

    template <typename T> class Pool {
        struct PoolBlock {
            T* _data = nullptr; unsigned char _cur = 0;
            T& operator[](unsigned int I) const { return _data[I]; }
            PoolBlock(): _data((T*)::operator new(sizeof(T) * 255)) {};
            ~PoolBlock() {
                for (unsigned char i = 0; i < _cur; i++) _data[i].~T();
                ::operator delete(_data);
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

            unsigned int bytes() const noexcept { return size() * sizeof(T); }

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