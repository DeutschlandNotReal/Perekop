#pragma once
#include <PK/Core/vector.hpp>
#include <cstdio>

namespace pk {
    template <typename T> class set {
        public: class entry : public T {
            public:
                const uint id{0};
  
                entry() noexcept = default;

                template <typename... args> entry(uint id, args&&... arg) noexcept:
                    T(std::forward<args>(arg)...), id(id) 
                {}

                explicit operator bool() const noexcept { return id != 0; }
                bool operator!() const noexcept { return id == 0; }
        };

        private:
            vector<entry> data;
            vector<uint> index;

        public:
            entry* begin() noexcept { return data.begin(); }
            entry* end()   noexcept { return data.end(); }

            const entry* begin() const noexcept { return data.begin(); }
            const entry* end() const noexcept { return data.end(); }

            uint size() const noexcept { return data.size(); }
            bool valid(uint id) const noexcept { return id - 1 < size(); }

            entry& operator[](uint id) noexcept { 
                if (id-1 > size()) printf("%i id's out of bounds mate\n", id); 
                return data[index[id-1]]; 
            }

            const entry& operator[](uint id) const noexcept { 
                if (id-1 > size()) printf("%i id's out of bounds mate\n", id); 
                return data[index[id-1]]; 
            }

            const entry& dataindex(uint id) const noexcept { return data[id]; }
            entry& dataindex(uint id) noexcept { return data[id]; }

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

    template <typename T> using entry = set<T>::entry&;
}