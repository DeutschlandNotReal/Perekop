#define PK_INTERNAL
#include <PK/Util/json.hpp>
#include <PK/Core/array.hpp>
using namespace pk;

template <char... C> static inline constexpr u64 mask = ((1ull << C) | ...);
inline constexpr u64 ws_mask = mask<'\n','\t','\r',' '>;

const char* find(const char *cur, char val, const char *until) noexcept {
    return (const char*)std::memchr(cur, val, until - cur);
}

void skip_matches(u64 mask, const char* &cur, const char* end) noexcept {
    while ((mask >> *cur) & ((*cur - 64) >> 31) & ((cur - end) >> 63)) ++cur;
}

void skip_nmatches(u64 mask, const char* &cur, const char* end) noexcept {
    while (~((mask >> *cur) & ((*cur - 64) >> 31)) & ((cur - end) >> 63)) ++cur;
}

json::parser::parser(strview src) noexcept: scope_level{0}, cur(src.begin()), end(src.end()) {
    skip_matches(ws_mask, cur, end);

    scope_context = *cur == '[';
}

const char* json::parser::find(const char *from, const char *to, char val) const noexcept {
    return (const char*)std::memchr(from, val, to - from);
}

char json::parser::get_scope() const noexcept {
    return (']' - '}') & 1 - ((scope_context << scope_level) & 1);
}

void json::parser::set_scope(char scope) noexcept {
    scope = (((scope - '{') >> 31) & 1) << ++scope_level;
    scope_context |= scope;
}

void json::parser::pop_scope() noexcept {
    scope_context &= ~('\1' << scope_level--);
}

bool json::parser::finished() const noexcept { return cur == end; }

json::parser::parse_object json::parser::next() noexcept {
    char scope = get_scope();

    strview index;
    if (scope == '{') {
        const char *i_src = cur;
        const char *i_end = find(i_src + 1, end, '"');
        index = {i_src, i_end + 1};
        cur = i_end;
        skip_matches(ws_mask | mask<':'>, cur, end);
    }
    
    const char *c_src = cur;
    skip_nmatches(ws_mask | mask<','>);
    const char *scope_end = find(cur, end, scope + 2);
    const char *comma_end = find(cur, scope_end, ',');
    const char *c_end = comma_end ? comma_end + 1 : scope_end;
}


namespace pk::json {
    // json parser assumes json is perfectly formatted, doesnt throw errors
    // new system next commit
    void parse(strview src, void(*callback)(const token& t, void* userdata), void* userdata) {
        using enum pk::json::type;
        if (!src) return;
        const char *cur = src.begin(), *end = src.end();

        auto space_skip = [&cur, &end](){  b     
            while (cur != end && is_space(*cur)) ++cur;
        };

        u16 scope{1};

        space_skip();
        callback({.type = (type)*cur++}, userdata);

        while (scope && cur < end) {
            space_skip();
            if (*cur == ',') { ++cur; space_skip(); }

            strview key;
            token tok{};
            parse_begin:
            switch (*cur) {
                case '"': {
                    // can be key or string
                    strview value{parsestr(++cur, end)};
                    if (*cur == ':') { // is key
                        key = value;
                        ++cur;
                        space_skip();
                        goto parse_begin;
                    } else  // is value
                        tok = {.key = key, .type = _str, .value_str = value};
                    break;
                }
                case 't': { // true literal
                    cur += 4;
                    tok = {.key = key, .type = _bool, .value_int = 1};
                    break;
                }
                case 'f': { // false literal
                    cur += 5;
                    tok = {.key = key, .type = _bool, .value_int = 0}; 
                    break;   
                }
                case 'n': { // null literal
                    cur += 4;
                    tok = {.key = key, .type = _null};
                    break;
                }
                case '[':
                case '{': {
                    // array/object start
                    ++scope; 
                    tok = {.key = key, .type = (type)*cur++};
                    break;
                }
                case ']':
                case '}': {
                    // array/object end
                    --scope;
                    tok = {.key = key, .type = (type)*cur++};
                    break;
                }

                default: {
                    // int or float literal

                    bool is_float;
                    n32 value = parsen32(cur, end, is_float);

                    tok = is_float 
                        ? token{.key = key, .type = _flt, .value_flt = value.f32} 
                        : token{.key = key, .type = _int, .value_int = value.i32};
                }
            }
            callback(tok, userdata);
        }
    }
        */
    }
}

    