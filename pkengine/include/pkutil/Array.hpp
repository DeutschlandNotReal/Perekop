#pragma once
#include <cstring>
#include <initializer_list>
using uint = unsigned int;
namespace pkutil {
    template <typename T> inline T* alloc(int n) {
       return (T*)::operator new(sizeof(T)*n);
    }
    inline void dealloc(void* ptr) { ::operator delete(ptr); }
    

    template <typename T> class Array {
        T* _data{nullptr}; uint _cur{0}, _end{0};
        T* _slot() {
            if (_cur == _end) _resize(_end * 2 + 1);
            return _data + _cur++;
        }
        void _resize(uint size) {
            T* old = _data;
            _data = alloc<T>(size);
            std::memcpy(_data, old, _cur*sizeof(T));
            dealloc(old);
            _end = size;
        }
        public:
            Array(uint len = 1): _end(len), _data(alloc<T>(len)) {}
            Array(const T& first): _end(1), _cur(1), _data(new T(first)) {}
            Array(std::initializer_list<T> first): _end(first.size()), _cur(first.size()), _data(alloc<T>(first.size())) {
                std::memcpy(_data, first.begin(), first.size() * sizeof(T));
            }
            ~Array() { dealloc(_data); }
            T& operator[](uint i) const noexcept { return _data[i]; }
            T* begin() const noexcept { return _data; }
            T* end() const noexcept { return _data + _cur; }
            T& back() const noexcept { return _data[_cur-1]; }

            uint size() const noexcept { return _cur; }
            uint capacity() const noexcept { return _end; }
            void clear() noexcept { _cur = 0; }
            T& pop() noexcept { return _data[--_cur]; }
            T& swap_pop(uint i) noexcept { return _data[i] = _data[--_cur]; }

            void reserve(uint n) { _resize(capacity() + n); }

            template <typename... A> void emplace(A&&... args) { 
                new (_slot()) T(args...);
            }

            void push(const T& item) {
                new (_slot()) T(item);
            }

            void push(std::initializer_list<T> items) {
                int required = size() + items.size() - capacity();
                if (required > 0) reserve(required);
                std::memcpy(_data + _cur, items.begin(), items.size()*sizeof(T));
                _cur += items.size();
            }

            Array(const Array& b): _cur(b._cur), _end(b._end), _data(alloc<T>(b._end)) {
                std::memcpy(_data, b._data, b._cur * sizeof(T));
            }

            Array& operator=(const Array& b) {
                _cur = b._cur; _end = b._end; 
                dealloc(_data);
                _data = alloc<T>(b._end);
                std::memcpy(_data, b._data, _cur * sizeof(T));
                return *this;
            }

            Array(Array&& b): _cur(b._cur), _end(b._end), _data(b._data) {
                b._data = nullptr; b._end = 0; b._cur = 0;
            }

            Array& operator=(Array&& b) {
                if (_data) dealloc(_data);
                _data = b._data, _cur = b._cur, _end = b._end;
                b._end = 0; b._cur = 0; b._data = nullptr;
                return *this;
            }
    };
}