#pragma once
#include <PKCore/string.hpp>
#include <PKCore/stack.hpp>
#include <PKLib/json.hpp>
#include <cctype>
#include <cstdlib>
using namespace pk;

void space_skip(const char* &cur, const char *end) {
    while (cur != end && isspace(*cur)) ++cur;
}

const char* find(const char *cur, char val, const char *until) {
    return (const char*)std::memchr(cur, val, until - cur);
}

namespace pk::json {
    // assumes JSON is properly formatted, doesnt do checks

    void parse(strview src, bool (*consumer)(token token)) {
        using enum pk::json::type;
        if (!src) return;

        const char *cur = src.begin(), *end = src.end();
        stack<char, 64> scope;

        auto next_delimiter = [&cur, &end, &scope](){
            if (!scope.size()) return end;

            static constexpr char sdif = '}' - '{';
            const char *scope_end = find(cur, scope.back() + sdif, end);
            const char *comma_end = find(cur, ',', scope_end);

            return comma_end ? comma_end + 1 : scope_end;
        };

        space_skip(cur, end);

        consumer({.type = json::type(*cur)});
        scope.push(*cur++);

        strview key;
        token tok{};
        while (!scope.is_empty()) {
            if (scope.back() == '{' && *cur != '}') {
                // key-value pair
                const char *colon = find(++cur, ':', end);
                key = {cur, find(cur, '"', colon)};
                cur = colon;
                space_skip(cur, end);
            }

            switch (*cur) {
                case '"': { // string value
                    const char *strcur = cur+1;
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
                    const char *strend = cur;
                    cur = next_delimiter();
                    tok = {.type = _str, .key = key, .value_str = {strcur, strend}};
                    break;
                }
                case 't': { // true literal
                    cur += 4;
                    cur = next_delimiter();
                    tok = {.type = _bool, .key = key, .value_int = 1};
                    break;
                }
                case 'f': { // false literal
                    cur += 5;
                    cur = next_delimiter();
                    tok = {.type = _bool, .key = key, .value_int = 0};
                    break;
                }
                case 'n': { // null literal
                    cur += 4;
                    cur = next_delimiter();
                    tok = {.type = _bool, .key = key, .value_int = 0};
                    break; 
                }

                case '{':   // object begin
                case '[': { // array begin
                    scope.push(*cur);
                    tok = {.type = json::type(*cur++), .key = key};
                    break;
                }

                case '}':  // object end
                case ']': { // array end
                    scope.pop();
                    tok = {.type = json::type(*cur++)};
                    break;
                }

                default: { // int or float
                    const char *delim = next_delimiter();
                    bool has_dot = find(cur, '.', delim) != nullptr;
                    if (has_dot) 
                        tok = {.type = _flt, .key = key, .value_flt = strtof(cur, nullptr)};
                    else 
                        tok = {.type = _int, .key = key, .value_int = strtol(cur, nullptr, 10)};
                    
                    cur = delim;
                }
            }
        }
    }
}

    