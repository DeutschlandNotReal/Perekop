#pragma once

#include <PK/Core/vector.hpp>
#include <PK/debug.hpp>

#include <utility>

namespace pk {
    template <typename T> class SID;
    template <typename T> class handle;
    static inline constexpr uint nullid{~0u};
  
    template <typename T> class set {
        friend SID<T>;

        vector<handle<T>> data;
        vector<SID<T>> index;
        SID<T> free;

        constexpr SID<T> find(SID<T> idt) const noexcept {
            if (!idt || idt.gen & 128 || idt.inx > index.size()) return nullid;
        
            SID<T> dpos = index[idt.inx];

            if (dpos.gen != idt.gen || dpos > data.size()) return nullid;
          
            return dpos;
        };

        public:
            constexpr auto* begin(this auto&& s) noexcept { return s.data.begin(); }
            constexpr auto* end(this auto&& s)   noexcept { return s.data.end(); }

            constexpr uint size() const noexcept { return data.size(); }

            constexpr auto* operator[](this auto&& v, SID<T> i) noexcept { 
                SID<T> dpos = v.find(i);
            
                return dpos ? v.data.begin() + dpos.inx : nullptr;
            }

            template <typename... args>
            handle<T>& create(args&&... arg) requires(std::is_constructible_v<T, args...>) {
                SID<T> id;

                if (free.inx == 0xFFFFFF) {
                    id = {index.size(), 0u};
                    index.emplace(data.size(), 0u);
                } else {
                    SID<T>& slot = index[free.inx];
                    id = {free.inx, (slot.gen + 1u) & 127u};
                    free = {slot.inx, 0u};
                    slot = {data.size(), id.gen};
                }

                handle<T>& item = data.emplace(std::forward<args>(arg)...);
                item.ID = id;
                return item;
            }

            void destroy(SID<T> i) {
                SID<T> dpos = find(i);
                if (!dpos) return;

                if (dpos.inx != data.size() - 1) {
                    handle<T>& back = data.back();
                    index[back.getID().inx] = {dpos.inx, back.getID().gen};
                    data[dpos.inx] = std::move(back);
                }
                data.pop();

                index[i.inx] = {free.inx, i.gen | 128u};
                free = {i.inx, 0u};
            }
    };

    template <typename T>
    class handle: public T {
        friend set<T>;
        SID<T> ID;

        public:
            inline static constinit set<T> items;

            template <typename... args>
            handle(args&&... arg) requires(std::is_constructible_v<T, args...>): T(std::forward<args>(arg)...) {}

            const SID<T> getID() const noexcept { return ID; }
            SID<T> getID() noexcept { return ID; }

            template <typename... args>
            inline static handle& create(args... arg) requires (std::is_constructible_v<T, args...>) { 
                return items.create(std::forward<args>(arg)...);
            }

            void destroy() { 
                return items.destroy(ID); 
            }
    };

    
    template <typename T> class SID {
        friend set<T>;

        union {
            struct { uint inx : 24, gen : 8; };

            uint x{nullid};
        };

        public: 
            SID() noexcept = default;
            SID(uint x) noexcept: x(x) {}
            SID(uint inx, uint gen) noexcept: inx(inx), gen(gen) {}

            operator uint() const noexcept { return x; }

            auto* get() const noexcept {
                return handle<T>::items[*this];
            }

            explicit operator bool() const noexcept { return x != nullid; }
            bool operator!()         const noexcept { return x == nullid; }
    };
}