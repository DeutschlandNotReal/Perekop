#pragma once
#include <PK/Core/string.hpp>

namespace pk::json {
    class parser {
        const char *cur, *end;
        u64 scope_context; // bitmap, [ = 1, { = 0
        u8  scope_level;

        char get_scope() const noexcept; // returns ] or }
        void set_scope(char) noexcept; // requires [ or {
        void pop_scope() noexcept;

        const char* find(const char* from, const char* to, char val) const noexcept;

        public:
            parser(strview src) noexcept;

            struct parse_object { strview index, content; };

            bool finished() const noexcept;
            parse_object next() noexcept;
    };
}