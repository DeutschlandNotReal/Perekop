#pragma once

#include <PK/Core/vector.hpp>
#include <PK/Core/setID.hpp>

#include <utility>

namespace pk {
    template <typename T> class set {
        public: class entry : public T {
            public:
                setID id;
  
                entry() noexcept = default;

                template <typename... args> entry(setID id, args&&... arg) noexcept:
                    T(std::forward<args>(arg)...), id(id) 
                {}

                explicit operator bool() const noexcept { return id != nullid; }
                bool operator!() const noexcept { return id == nullid; }
        };

        private:
            vector<entry> data;
            vector<uint> index;

        public:
            constexpr auto* begin(this auto&& s) noexcept { return s.data.begin(); }
            constexpr auto* end(this auto&& s)   noexcept { return s.data.end(); }

            constexpr uint size() const noexcept { return data.size(); }
            constexpr bool valid(uint id) const noexcept { return id  < size(); }

            constexpr auto& operator[](this auto&& v, uint id) noexcept { 
                return v.data[v.index[id]]; 
            }

            template <typename... args>
            entry& create(args&&... arg) {
                uint id = index.size();
                data.emplace(id, std::forward<args>(arg)...);
                index.push(data.size() - 1);
                return data.back();
            }

            void destroy(setID id) {
                if (id >= index.size()) return;
                uint dataid = index[id];
                if (dataid >= data.size()) return;

                setID back_id = data.back().id;
                std::swap(data.back(), data[dataid]);
                index[back_id] = dataid;
                data.pop();
                index[id] = nullid; 
            }
    };

    template <typename T> using entry = set<T>::entry&;
}