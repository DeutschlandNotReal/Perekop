#include <PKLib/json.hpp>
#include <PKCore/array.hpp>
#include <cmath>
using namespace pk;

const char* find(const char *cur, char val, const char *until) noexcept {
    return (const char*)std::memchr(cur, val, until - cur);
}

bool is_space(u8 c) {
    // all space characters are below 64
    static constexpr u64 lookup{1ull << ' ' | 1ull << '\n' | 1ull << '\r' | 1ull << '\t'};

    return (lookup >> c) & 1;
} 

f32 pow10(i32 n) noexcept {
    static constexpr f32 ln10 = 2.30258509299f;

    return expf(ln10 * n);
}

void parseu32(const char *&cur, const char *end, u32& accum) noexcept {
    while (cur != end) {
        u8 digit = *cur - '0';
        if (digit > 9) break;
        accum = accum * 10 + digit;
        ++cur;
    }
}

u32 parseu32(const char *&cur, const char *end) noexcept {
    u32 accum{0};
    parseu32(cur, end, accum);
    return accum;
}

struct n32 {
    union { f32 f32; i32 i32; };
};

n32 parsen32(const char *&cur, const char *end, bool& flt) noexcept {
    i8 sign = *cur == '-';
    cur += sign;
    sign = (1 - (sign << 1));

    u32 frac = parseu32(cur, end);
    i16 exp{0};

    if (*cur == '.') {
        ++cur;
        const char *dcur = cur;
        parseu32(cur, end, frac);
        exp -= (cur - dcur); // digits of decimals
    } else {
        flt = false;
        return {.i32 = (i32)frac * sign};
    } 
    if (*cur == 'e' || *cur == 'E') {
        char eneg = *(++cur) == '-';
        if (eneg || *cur == '+') ++cur; // skip - or +

        exp += parseu32(cur, end) * (1 - (eneg << 1));
    }
    
    flt = true;
    return {.f32 = (f32)(sign * frac) * pow10(exp)};
}

strview parsestr(const char *&cur, const char *end) noexcept {
    const char *strcur = cur;
    while (cur != end) {
        if (*cur == '\\' && cur + 1 != end)
            cur += 2; 
        else if (*cur == '"') 
            break;
        ++cur;
    }

    return {strcur, cur++};
}

namespace pk::json {
    // json parser assumes json is perfectly formatted, doesnt throw errors
    void parse(strview src, void(*callback)(const token& t, void* userdata), void* userdata) {
        using enum pk::json::type;
        if (!src) return;
        const char *cur = src.begin(), *end = src.end();

        auto space_skip = [&cur, &end](){
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
}

    