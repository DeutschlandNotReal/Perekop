#pragma once

#include <PK/Core/vector.hpp>

#include <utility>

namespace pk {
    template <typename T> class set;
    template <typename T> class handle;
    static inline constexpr uint nullid{~0u};
  
    template <typename T> class id {
        friend set<T>;
        uint val{nullid};
            
        uint getGen()   const noexcept { return (val >> 24) & 0x7F; }
        uint getIndex() const noexcept { return val & 0xFFFFF; }
        void upGen() noexcept { val += (0xFFFFF + 1); }
        bool invalid()  const noexcept { return val & 0x80000000; }
        void validate() noexcept { val &= 0x7FFFFFFF; }
        void invalidate() noexcept { val |= 0x80000000; }

        public: 
            id() noexcept = default;
            id(uint x) noexcept: val(x) {}
            explicit operator bool() const noexcept { return val != nullid; }
            bool operator!() const noexcept { return val == nullid; }

            operator T*() noexcept { return handle<T>::set[val]; }
            operator const T*() const noexcept { return handle<T>::set[val]; }
    };

    template <typename T> class set {
        friend id<T>;
        vector<T> data;
        vector<uint> index;
        id<T> free;

        constexpr id<T> testID(id<T> i) const noexcept {
            id<T> x = i.getIndex();
            if (i.invalid() || x.getIndex() > index.size()) return ~0u;

            id<T> dpos = index[x];

            if (dpos.invalid() || dpos.getGen() != i.getGen()) return ~0u;
  
            return dpos;
        }

        public:
            constexpr auto* begin(this auto&& s) noexcept { return s.data.begin(); }
            constexpr auto* end(this auto&& s)   noexcept { return s.data.end(); }

            constexpr uint size() const noexcept { return data.size(); }
            constexpr bool valid(uint id) const noexcept { return testID(id); }

            constexpr auto* operator[](this auto&& v, id<T> i) noexcept { 
                id<T> index = v.testID(i);
                if (index.invalid()) return nullptr;

                return &v.data[index];
            }

            template <typename... args>
            T& create(args&&... arg) {
                id<T> id;
                if (free.invalid()) {
                    id = index.size();
                } else {
                    id = free;
                    free = index[free.getIndex()];

                    id.validate();
                    id.upGen();
                }

                data.emplace(id, std::forward<args>(arg)...);
                index.push(data.size() - 1);
                return data.back();
            }

            void destroy(id<T> i) {
                id<T> dpos = testID(i);
                if (!dpos) return;

                index[i.getIndex()] = free;
                free = i.invalidate();

                T back = std::move(data.back());
                index[back.id.getIndex()] = i.getIndex();
                data[dpos] = std::move(back);
                data.pop();                
            }
    };

    template <typename T> 
    static inline auto& getset = []()->set<handle<T>>&{ static set<handle<T>> set; return set; }();

    template <typename T>
    class handle: public T {
        friend set<T>;
        id<T> uid;

        public:

            template <typename... args>
            handle(args&&... arg) : T(std::forward<args>(arg)...) {}

            const id<handle<T>> getID() const noexcept { return uid; }
            id<handle<T>> getID() noexcept { return uid; }

            inline static set<handle<T>> &set = getset<T>;
    };
}