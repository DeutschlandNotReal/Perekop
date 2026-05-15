#pragma once
#include <Perekop/Array.hpp>

// stuff about stablepool
// * allocates in blocks not whole chunks (existing data doesnt have ot be moved)
// * basically internal linked list for free slots (each free slot contains index for next free slot...)
// * stable (very good)

namespace pk {
    template <typename T> class StablePool {
        Array<T*> _blocks;
        int _cur{0}, _cap{0}, _free{-1}, _items{0};

        T* at(int i) const noexcept { return _blocks[i>>8]+(i&255); }

        int slot() {
            ++_items;
            if (_free == -1) {
                if (_cur == _cap) _blocks.push((T*)::operator new(sizeof(T) * 256));
                return _cur++;
            } else {
                int i = _free; 
                _free = *static_cast<int*>(at(i));
                return i;
            }
        }

        public:
            StablePool(): 
                _blocks((T*)::operator new(sizeof(T) * 256)),
                _cap(256)
            {}
            T& operator[](unsigned int i) const noexcept { return &at(i); }
            unsigned int insert(const T& item) {
                int i = slot();
                new (at(i)) T(item);
                return i;
            }
            template <typename... A> unsigned int emplace(A&... args) {
                int i = slot();
                new (at(i)) T(std::forward(args)...);
                return i;
            }
            void dismiss(unsigned int i) {
                --_items;
                if (i == _cur-1) 
                    --_cur;
                else {
                    *static_cast<int*>(at(i)) = _free;
                    _free = i;
                }
            }
            void remove(unsigned int i) {
                if constexpr (!std::is_trivially_destructible_v<T>)
                    operator[](i).~T();
                dismiss(i);
            }
            struct SPIterator {
                unsigned int i = 0; int remaining; StablePool* pool;
                SPIterator(StablePool* p, int j): pool(p), i(j), remaining(p->_free) {};
                SPIterator& operator++() noexcept { 
                    while (remaining-- == 0)
                        remaining = *static_cast<int*>(pool->at(i)) - i;
                    i++; return *this; 
                }
                bool operator!=(const SPIterator& limit) noexcept { return i != limit.i; }
                T& operator*() noexcept { return pool->at(i); }
            };
            SPIterator begin() const noexcept { return SPIterator(this, 0); }
            SPIterator end() const noexcept { return SPIterator(this, _cur); }
            
            unsigned int size() { return _items; }
            unsigned int capacity() { return _blocks.capacity() * 256; }
            unsigned int bytesize() { return _items * sizeof(T); }

            ~StablePool() {
                if constexpr (!std::is_trivially_destructible_v<T>)
                    for (T& item : *this) item.~T();
                for (T* block : _blocks)
                    ::operator delete(block);
            }
    };
}