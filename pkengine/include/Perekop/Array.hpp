#pragma once
#include <cstring>
#include <initializer_list>
#include <type_traits>
#include <utility>

namespace pk {
    template <typename T> class Array {
        T* _data{nullptr};
        unsigned int _cur{0}, _end{0};

        void _move(T* dst, T* src, int n) {
            if constexpr(std::is_trivial_v<T>)
                std::memcpy(dst, src, n);
            else for (int i = 0; i < n; i++) 
                new (dst+i) T(std::move(src[i]));
        }
        
        void _copy(T* dst, T* src, int n) {
            if constexpr(std::is_trivial_v<T>)
                std::memcpy(dst, src, n);
            else for (int i = 0; i < n; i++) 
                new (dst+i) T(src[i]);
        }

        void _resize(unsigned int size) {
            T* _prev = _data;
            _data = (T*)::operator new(size * sizeof(T));
            if (_prev) {
                 _move(_data, _prev, _cur);
                ::operator delete(_prev);
            }
            _end = size;
        }

        public:
            T* begin() const noexcept { return _data; }
            T* end() const noexcept { return _data + _cur; }
            T* data() const noexcept { return _data; }

            unsigned int size() const noexcept { return _cur;  }
            unsigned int bytesize() const noexcept { return size() * sizeof(T); }
            unsigned int capacity() const noexcept { return _end; }
            unsigned int remaining() const noexcept { return _end - _cur; }

            bool empty() const noexcept { return 0 == _cur; }
            void clear() noexcept { _cur = 0; }

            T& back() const noexcept { 
                return *(_data + _cur - 1);
            }

            void pop() noexcept {
                if (!_data || !_cur) return;
                if constexpr(std::is_trivially_destructible_v<T>) 
                    --_cur;
                else
                    _data[--_cur].~T();
            }

            T popout() noexcept { return T(std::move(_data[--_cur])); }

            T& swap_pop(unsigned int i) noexcept {
                _data[i] = T(std::move(_data[--_cur]));
                return _data[i];
            }

            void reserve(unsigned int new_capacity) {
                if (new_capacity < capacity()) return;
                _resize(new_capacity);
            }

            template <typename... A> T& emplace(A&&... args) {
                if (!_data || !remaining()) _resize(size() * 2 + 1);
                T* ptr = _data + _cur++;
                new (ptr) T(std::forward<A>(args)...);
                return *ptr;
            }

            template <typename... A> T& emplace_at(unsigned int at, A&&... args) {
                if (!_data || at > capacity()) _resize(at);
                T* ptr = _data + at;
                new (ptr) T(std::forward<A>(args)...);
                return *ptr;
            }

            T& push(const T& item) {
                if (!_data || !remaining()) _resize(size() * 2 + 1);
                T* ptr = _data + _cur++;
                new (ptr) T(item);
                return *ptr;
            }

            void rawpush(const T& item) {
                new (_data+_cur++) T(item);
            }

            void push(std::initializer_list<T> items) {
                if (items.size() > remaining()) _resize(size() + items.size() - remaining());
                _move(_data + _cur, items.begin(), items.size());
                _cur += items.size();
            }

            void push_sorted(const T& item) {
                unsigned int i = _cur;
                while (i > 0 && _data[i-1] < item)
                    _data[i] = T(std::move(_data[--i]));
                new (_data + i) T(item);
            }

            Array() = default;
            Array(unsigned int capacity):
                _data((T*)::operator new(sizeof(T)*capacity)),
                _cur(0),
                _end(capacity)
            {}

            Array(const Array& copy):
                _data((T*)::operator new(copy.bytesize())),
                _cur(copy.size()),
                _end(copy.size())
            { _copy(_data, copy._data, copy.size()); }

            Array& operator=(const Array& copy) {
                ::operator delete(_data);
                new (this) Array(copy);
                return *this;
            }

            ~Array() {
                if constexpr(!std::is_trivially_destructible_v<T>)
                    for (int i = 0; i < _cur; i++) _data[i].~T();
                ::operator delete(_data);
            }

            T& operator[](int index) const noexcept { return *(_data + index); }
    };
}