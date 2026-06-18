#pragma once
#include <PKCore/vector.hpp>
#include <PKCore/string.hpp>
#include <PKLib/file.hpp>
#include <cctype>
using namespace pk;

struct JSONInstance {
    enum JSONType: char { empty, array, array_end, object, object_end, bnull, bfalse, btrue, integer, real, string };

    JSONType type{empty};
    vstring index{}; // still referance to original source, only present in object
    void* data{nullptr};

    explicit operator bool() const { return type != JSONType::empty; }
    bool operator!() const { return type == JSONType::empty; }

    JSONInstance(JSONType type, vstring index): type(type), index(index) {}

    JSONInstance(vstring index, int val): type(integer), index(index) {
        data = reinterpret_cast<void*>(val);
    }

    JSONInstance(vstring index, float val): type(real), index(index) {
        data = reinterpret_cast<void*>(*(int*)&val);
    }
};

class JSONParser {
    const char *cur, *end;
    void space_skip() {
        while (cur != end && isspace(*cur)) ++cur; 
    }

    const char* find(char c) const {
        return (const char*)std::memchr(cur, c, end - cur);
    }

    vector<bool> scope;

    public:
        JSONParser(vstring src): cur(src.begin()), end(src.end()) {
            space_skip();
            switch (*cur) {
                case '{': // object
                    scope.push_back(1); break;
                case '[': // array
                    scope.push_back(0); break;
            }
        }

        JSONInstance emit() {
            bool is_object = scope.back();



            vstring index;
            if (is_object) {
                ++cur; // skips first "
                index = {cur, find('"')};
                cur = index.end() + 1; // skips index and last "
                skip_whitespace();
                ++cur; // skips :
                skip_whitespace();
            }

            vstring value;

            switch(*cur) {
                case '[': // array 
                    return JSONInstance(JSONInstance::array, index);    
                case '{': // object
                    return JSONInstance(JSONInstance::object, index);
                case '"': { // string
                    auto *str_cur = cur + 1;
                    auto *str_end = find('"') - 1;
                    cur = str_end + 2;
                    return JSONInstance(JSONInstance::string, {str_cur, str_end});
                }
                case ']': // array end
                    return JSONInstance(JSONInstance::array_end, index);
                case '}': // object end
                    return JSONInstance(JSONInstance::object_end, index);
                    
            }


        };
};