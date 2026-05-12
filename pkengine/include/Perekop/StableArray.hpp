#pragma once

// Purpose of this rather than pk::Array is this one tries to keep indices in order
// (VERY important for meshes since user gets a mesh ID)

#include <cstring>
#include <type_traits>
#include <utility>
namespace pk {
    template <typename T, typename Index> class StableArray {
        T* _data{nullptr};
        Index _cur{0}, _end{0}, _free{~0};

        void _move(T* dst, T* src, Index n) {
            if constexpr(std::is_trivial_v<T>)
                std::memcpy(dst, src, n*sizeof(T));
            else for (Index i = 0; i < n; i++)
                dst[i] = std::move(src[i]);
        }

        Index _next() {
            if (!_data) { _end = 1; _data = (T*)::operator new(sizeof(T)*_end); }
            if (_free != ~0) {
                // available slot
                Index i = _free;
                _free = *reinterpret_cast<Index*>(&_data[_free]);
                ++_cur;
                return i;
            } else {
                if (_cur++ == _end) {
                    // array grow
                    _end = _end * 2 + 1;
                    T* _prev = _data;
                    _data = (T*)::operator new(sizeof(T)*_end);
                    _move(_data, _prev, _cur-1);
                    ::operator delete(_prev);
                }
                return _cur-1;
            }
        }

        public:
            unsigned int size() const noexcept { return _cur;  }
            unsigned int bytesize() const noexcept { return size() * sizeof(T); }
            unsigned int capacity() const noexcept { return _end; }
            unsigned int remaining() const noexcept { return _end - _cur; }

            T& operator[](Index i) const noexcept { return _data[i]; }
            struct SAIterator {
                const StableArray* array; Index index, until;
                SAIterator() = default;
                SAIterator(const StableArray* array): array(array), index(0), until(array->_free) {}
                T& operator*() { return array->_data[index]; }
                SAIterator& operator++() {
                    Index end = array->_end;
                    T* data = array->_data;
                    while (++index == until && index < end)
                        until = *reinterpret_cast<Index*>(&data[index]);

                    return *this;
                }
                bool operator!=(char) const {
                    return index != array->_end;
                };
            };
            T* data() { return _data; }
            SAIterator begin() { return {this}; }
            char end() { return '\0'; }

            Index insert(const T& item) {
                Index i = _next();
                new (_data + i) T(item);
                return i;
            }

            template <typename... A> Index emplace(A&&... args) {
                Index i = _next();
                new (_data + i) T(std::forward<A>(args)...);
                return i;
            }

            void pop() {
                if constexpr(!std::is_trivially_destructible_v<T>)
                    _data[--_cur].~T();
                else
                    --_cur;
            }

            T popout() {
                return T(std::move(_data[--_cur]));
            }

            void remove(Index i) {
                if constexpr(!std::is_trivially_destructible_v<T>) _data[i].~T();
                *reinterpret_cast<Index*>(_data+i) = _free;
                _free = i;
            }

            void dismiss(Index i) {
                *reinterpret_cast<Index*>(_data+i) = _free;
                _free = i;
            }

            T removeout(Index i) {
                T item = T(std::move(_data[i]));
                *reinterpret_cast<Index*>(_data+i) = _free;
                _free = i;
                return item;
            }

            ~StableArray() {
                if constexpr(!std::is_trivially_destructible_v<T>)
                    for (T& item : *this) item.~T();
                ::operator delete(_data); 
            }
    };
}