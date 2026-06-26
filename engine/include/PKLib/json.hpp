#pragma once
#include <PKCore/string.hpp>

namespace pk::json {
    enum type: char {_empty = 0, _null, _bool, _str, _int, _flt, _arr = '[', _arr_end = ']', _obj = '{', _obj_end = '}', };
    struct token {
        strview key;
        type type{_empty};

        union {
            strview value_str;
            int value_int;
            float value_flt;
        };

        explicit operator bool() const { return type; }
        bool operator!() const { return !type; }
    };


    extern void parse(strview src, void(*callback)(const token&, void*), void* userdata = nullptr);
}