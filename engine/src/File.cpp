#define PK_ENGINE_SRC
#include <Internal.hpp>
#include <cstdio>
#include <PKLib/file.hpp>
#include <PKCore/algorithm.hpp>
using namespace pk;

string pk::read_file(strview path) {
    FILE* f = fopen(path, "rb");
    if (!f) {
        printf("Can't find file: '%*s'\n", path.size(), path.begin());
        return {};
    }

    fseek(f, 0, SEEK_END);
    u32 len = ftell(f); rewind(f);
    string text(len);

    fread(text.begin(), 1, len, f);   
    fclose(f);
    return text;
}