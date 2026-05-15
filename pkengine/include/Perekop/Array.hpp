#pragma once
#include <cstring>
#include <initializer_list>
#include <malloc.h>
#include <type_traits>
#include <utility>

namespace pk {
    template <typename T> class Array {
        static constexpr bool trivial = std::is_trivial_v<T>;
        static constexpr bool trivial_D = std::is_trivially_destructible_v<T>;

        T* _data{nullptr}; unsigned int _cur{0}, _end{0};

        void _move(T* dst, T* src, int n) {
            if constexpr (trivial)
                std::memcpy(dst, src, n*sizeof(T));
            else for (int i = 0; i < n; i++)
                new (dst+i) T(std::move(src[i]));
        }

        void _copy(T* dst, T* src, int n) {
            if constexpr (trivial)
                std::memcpy(dst, src, n*sizeof(T));
            else for (int i = 0; i < n; i++)
                new (dst+i) T(src[i]);
        }

        void _resize(unsigned int size) {
            T* _prevdata = _data;
            _data = (T*)::operator new(size * sizeof(T));
            _move(_data, _prevdata, size);
            ::operator delete(_prevdata);
            _end = size;
        }

        T* _next() {
            if (_cur++ == _end) _resize(_end * 2 + 1);
            return _data + _cur;
        }

        public:
            T* begin() const noexcept { return _data; }
            T* end() const noexcept { return _data + _cur; }
            T* data() const noexcept { return _data; }

            unsigned int size() const noexcept { return _cur;  }
            unsigned int bytesize() const noexcept { return _cur * sizeof(T); }
            unsigned int capacity() const noexcept { return _end; }
            unsigned int remaining() const noexcept { return _end - _cur; }

            bool empty() const noexcept { return _cur == 0; }
            bool full() const noexcept { return _cur == _end; }
            void point_to_start() noexcept { _cur = 0; }
            void clear() {
                if constexpr(trivial_D)
                    _cur = 0;
                else for (; _cur > -1; --_cur)
                    _data[_cur].~T();
            }

            T& back() const noexcept { return _data[_cur-1]; }

            void pop() {
                if constexpr(!trivial_D) _data[--_cur].~T(); else --_cur;
            }

            T popout() { 
                return T(std::move(_data[--_cur])); 
            }

            T& swap_pop(unsigned int i) noexcept {
                _data[i] = T(std::move(_data[--_cur]));
                return _data[i];
            }

            void reserve(unsigned int extra) {
                _resize(capacity() + extra);
            }

            template <typename... A> T& Emplace(A&&... args) {
                new (_next()) T(std::forward<A>(args)...);
                return back();
            }

            T& push(const T& item) {
                new (_next()) T(item);
                return back();
            }

            // no-check push, assumes array is large enough
            T& push_nc(const T& item) {
                new (_data+_cur++) T(item);
                return back();
            }

            void push(std::initializer_list<T> items) {
                if (items.size() > remaining()) reserve(items.size() - remaining());
                _move(_data + _cur, items.begin(), items.size());
                _cur += items.size();
            }

            Array():
                _data((T*)::operator new(sizeof(T))),
                _cur(0), _end(1)
            {}

            Array(unsigned int capacity):
                _data((T*)::operator new(sizeof(T)*capacity)),
                _cur(0), _end(capacity)
            {}

            Array(const Array& copy):
                _data((T*)::operator new(copy.bytesize())),
                _cur(copy.size()), _end(copy.size())
            { _copy(_data, copy._data, copy.size()); }

            Array& operator=(const Array& copy) {
                ~Array();
                new (this) Array(copy);
                return *this;
            }

            ~Array() {
                if constexpr(!trivial_D) for (T& item : *this) item.~T();
                ::operator delete(_data);
            }

            T& operator[](int index) const noexcept { return *(_data + index); }

            bool operator==(const Array<T>& b) const noexcept {
                return size() == b.size() && std::memcmp(_data, b._data, bytesize()) == 0;
            }
    };
}