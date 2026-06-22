#pragma once
#include <PKCore/string.hpp>
#include <PKCore/stack.hpp>
#include <cctype>
#include <cstdlib>
using namespace pk;

namespace pkgame::json {
    enum tokentype: char { _EMPTY, _BOOL, _ARRAY, _ARRAYEND, _OBJECT, _OBJECTEND, _STRING, _INT, _FLOAT };

    struct token {
        tokentype type{_EMPTY};
        strview key;
        union {
            strview value_str;
            int value_int;
            float value_flt;
        };

        explicit operator bool() const { return type != _EMPTY; }
        bool operator!() const { return type == _EMPTY; }

        bool match(const strview& i) const {
            return key == i;
        }

        bool match(const strview& i, tokentype t) const {
            return type == t && key == i;
        }
    };

    // assumes JSON is properly formatted, doesnt do checks
    class parser {
        const char *cur, *end;
        stack<char, 64> scope;

        void space_skip() {
            while (cur != end && isspace(*cur)) ++cur;
        }

        const char* find(char val, const char* until) const {
            return (const char*)std::memchr(cur, val, until - cur);
        }

        const char* next_delimiter() {
            // if comma, point past comma
            if (!scope.size()) return end;

            constexpr char sdif = '}' - '{';
            const char *scope_end = find(scope.back() + sdif, end);
            const char *comma_end = find(',', scope_end);

            return (comma_end) ? comma_end + 1 : scope_end;
        }

        public:
            parser(strview src): cur(src.begin()), end(src.end()) {
                space_skip();
                scope.push(*cur++);
            }

            token next_token() {
                space_skip();
                strview key;

                if (scope.back() == '{' && *cur != '}') {
                    // objects expect key-value pairs, this detects the key
                    ++cur; // first "
                    key = {cur, find('"', end)};
                    cur = find(':', end) + 1;
                    space_skip();
                }

                switch (*cur) {
                    case '"': { // string value
                        const char *str_start = cur+1;
                        bool escape = false;

                        while (cur != end) {
                            bool exit = false;
                            if (escape) 
                                {++cur; escape = false; }
                            else switch(*cur) {
                                case '"':  exit = true; break;
                                case '\\': escape = true;
                                default:   ++cur;
                            }

                            if (exit) break;
                        }
                        const char* str_end = cur;
                        cur = next_delimiter();
                        return {.type = _STRING, .key = key, .value_str = {str_start, str_end}};
                    }
                    case 't': { // true literal
                        cur += 4;
                        cur = next_delimiter();
                        return {.type = _BOOL, .key = key, .value_int = 1};
                    }
                    case 'f': { // false literal
                        cur += 5;
                        cur = next_delimiter();
                        return {.type = _BOOL, .key = key, .value_int = 0};
                    }
                    case 'n': { // null literal
                        cur += 4;
                        cur = next_delimiter();
                        return {.type = _BOOL, .key = key, .value_int = 0};    
                    }
                    case '{': { // object begin
                        ++cur;
                        scope.push('{');
                        return {.type = _OBJECT, .key = key};    
                    }
                    case '[': { // array begin
                        ++cur;
                        scope.push('[');
                        return {.type = _ARRAY, .key = key};
                    }
                    case '}': { // object end
                        ++cur;
                        scope.pop();
                        return {.type = _OBJECTEND};    
                    }
                    case ']': { // array end
                        ++cur;
                        scope.pop();
                        return {.type = _ARRAYEND};
                    }
                    default: { // int or float
                        const char *delim = next_delimiter();
                        bool has_dot = find('.', delim) != nullptr;
                        if (has_dot) {
                            // float
                            float value = strtof(cur, (char**)&cur);
                            cur = delim;
                            return {.type = _FLOAT, .key = key, .value_flt = value};
                        } else {
                            // int
                            int value = strtol(cur, (char**)&cur, 10);
                            cur = delim;
                            return {.type = _INT, .key = key, .value_int = value};
                        }
                    }
                }
            }

            bool completed() const { return !scope.size(); }
    };
}