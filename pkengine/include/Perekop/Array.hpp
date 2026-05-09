#pragma once

#include <cstring>
#include <initializer_list>
#include <utility>

namespace pk {
    // TRIVIAL TYPES ONLY FOR NOW
    template <typename T> class Array {
        T* _data;
        unsigned int _cur, _end;

        void _resize(unsigned int size) {
            T* _prev = _data;
            _data = (T*)::operator new(size * sizeof(T));
            std::memcpy(_data, _prev, _cur * sizeof(T));
            ::operator delete(_prev);
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

            T pop() noexcept {
                return T(std::move(*(_data + --_cur)));
            }

            void reserve(unsigned int new_capacity) {
                if (new_capacity < capacity()) return;
                _resize(new_capacity);
            }

            template <typename... A> T& emplace(A... args) {
                if (!remaining()) _resize(size() * 2 + 1);
                T* ptr = _data + _cur++;
                new (ptr) T(std::forward<A>(args)...);
                return *ptr;
            }

            T& push(const T& item) {
                if (!remaining()) _resize(size() * 2 + 1);
                T* ptr = _data + _cur++;
                new (ptr) T(item);
                return *ptr;
            }

            void push(std::initializer_list<T> items) {
                if (items.size() > remaining()) _resize(size() + items.size() - remaining());
                std::memcpy(_data + _cur, items.begin(), items.size() * sizeof(T));
                _cur += items.size();
            }

            Array():
                _data((T*)::operator new(10*sizeof(T))),
                _cur(0),
                _end(10)
            {}

            Array(unsigned int capacity):
                _data((T*)::operator new(capacity)),
                _cur(0),
                _end(capacity)
            {}

            Array(const Array& copy):
                _data((T*)::operator new(copy.bytesize())),
                _cur(copy.size()),
                _end(copy.size())
            { std::memcpy(_data, copy._data, copy.bytesize()); }

            Array& operator=(const Array& copy) {
                ::operator delete(_data);
                new (this) Array(copy);
                return *this;
            }

            T& operator[](int index) const noexcept { return *(_data + index); }
    };
}