#define PK_INTERNAL
#include <PK/Util/json.hpp>
#include <PK/Core/array.hpp>
using namespace pk;

const char* find(const char *cur, char val, const char *until) noexcept {
    return (const char*)std::memchr(cur, val, until - cur);
}

bool is_space(u8 c) {
    // all space characters are below 64
    static constexpr u64 lookup{1ull << ' ' | 1ull << '\n' | 1ull << '\r' | 1ull << '\t'};

    return (lookup >> c) & 1;
}

namespace pk::json {
    // json parser assumes json is perfectly formatted, doesnt throw errors
    // new system next commit
    void parse(strview src, void(*callback)(const token& t, void* userdata), void* userdata) {
        /*
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
        */
    }
}

    