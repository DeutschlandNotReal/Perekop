#define PK_ENGINE_SRC
// #define PK_DEBUG "file.cpp"

#include <Internal.hpp>

#include <cstdio>
#include <PKLib/File.hpp>
using namespace pk;

string File::read(stringview path) {
    FILE* f = fopen(path, "rb");
    if (!f) {
        printf("Can't find file: '%s'\n", path.begin());
        return {};
    }

    fseek(f, 0, SEEK_END);
    uint32_t len = ftell(f); rewind(f);
    string str{len};

    fread(str.begin(), 1, len, f);   
    fclose(f);
    return {(string&&) str};
}