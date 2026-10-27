#pragma once
#include <PK/Core/vector.hpp>
#include <cstdio>

namespace pk {
    template <typename T> class set {
        public: class entry {
            uint id;
            T val;

            public:
                uint GetId() const noexcept { return id; }

                operator const T&() const noexcept { return val; }
                operator T&() noexcept { return val; }

                const T& operator*() const noexcept { return val; }
                T& operator*() noexcept { return val; }

                const T* operator->() const noexcept { return &val; }
                T* operator->() noexcept { return &val; }

                template <typename... args>
                entry(uint id, args&&... arg) noexcept: id(id), val(std::forward<args>(arg)...) {}
        };

        private:
            pk::vector<entry> data;
            pk::vector<uint> index;

        public:
            entry* begin() noexcept { return data.begin(); }
            entry* end()   noexcept { return data.end(); }

            const entry* begin() const noexcept { return data.begin(); }
            const entry* end() const noexcept { return data.end(); }

            uint size() const noexcept { return data.size(); }
            bool valid(uint id) const noexcept { return id - 1 > size(); }

            entry& operator[](uint id) noexcept { 
                if (id-1 > size()) printf("%i id's out of bounds mate\n", id); 
                return data[index[id-1]]; 
            }

            const entry& operator[](uint id) const noexcept { 
                if (id-1 > size()) printf("%i id's out of bounds mate\n", id); 
                return data[index[id-1]]; 
            }

            template <typename... args>
            entry& push(args&&... arg) {
                uint id = index.size() + 1; // base-1 index, 0 = invalid
                data.emplace(id, std::forward<args>(arg)...);
                index.push(data.size() - 1);
                return data.back();
            }

            void remove(uint id) {
                if (--id > index.size()) return;

                uint pos = index[id];
                data[pos].~T();
                if (data.size() > 1)
                    new (data + pos) T(std::move(data.back()));
            }
    };
}