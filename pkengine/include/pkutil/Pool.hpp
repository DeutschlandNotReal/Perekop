#pragma once
#include <pkutil/Array.hpp>

// stuff about stablepool
// * allocates in blocks not whole chunks (existing data doesnt have ot be moved)
// * basically internal linked list for free slots (each free slot contains index for next free slot...)
// * stable (very good)

namespace pkutil {
    template <typename T> class StablePool {
        Array<T*> _blocks;
        uint _cur{0}, _cap{256}, _free{0xFFFFFFFF};
        T* _at(int i) const noexcept { return _blocks[i>>8]+(i&255); }
        int _slot() {
            if (~_free) {
                // free slot available
                int i = _free; 
                _free = *reinterpret_cast<int*>(_at(i));
                return i;
            } else {
                if (_cur == _cap) 
                    _blocks.push(pkutil::alloc<T>(256));
                return _cur++;
            }
        }
        public:
            StablePool() { _blocks.push(alloc<T>(256)); }
            T& operator[](uint i) const noexcept { return *_at(i); }
            uint insert(const T& item) {
                int i = _slot();
                new (_at(i)) T(item);
                return i;
            }
            template <typename... A> uint emplace(A&... args) {
                int i = _slot();
                new (_at(i)) T(args...);
                return i;
            }
            void remove(uint i) {
                if (i == _cur-1) { --_cur; return; }
                uint* prev = &_free;
                while (*prev != 0xFFFFFFFF && *prev < i)
                    prev = reinterpret_cast<uint*>(_at(*prev));
                *reinterpret_cast<uint*>(_at(i)) = *prev;
                *prev = i;
            }
            struct iterator {
                uint i; int r; 
                const StablePool* pool;
                iterator(const StablePool* pool, uint i): pool(pool), i(i), r(pool->_free) {};
                iterator& operator++() noexcept { 
                    while (r-- == 0)
                        r = *reinterpret_cast<int*>(pool->_at(i)) - i;
                    i++; 
                    return *this; 
                }
                bool operator!=(const iterator& limit) const noexcept { return i != limit.i; }
                T& operator*() const noexcept { return *pool->_at(i); }
            };
            iterator begin() const noexcept { return {this, 0}; }
            iterator end() const noexcept { return {this, _cur}; }
            uint capacity() const noexcept  { return _blocks.capacity() * 256; }

            ~StablePool() {
                for (T*& block : _blocks) ::operator delete(block);
            }
    };
}