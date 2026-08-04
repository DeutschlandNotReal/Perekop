#define PK_INTERNAL
#include <cstdio>
#include <PK/Util/file.hpp>
#include <PK/Core/algorithm.hpp>
using namespace pk;

string pk::read_file(strview path) {
    FILE* f = fopen(path, "rb");
    if (!f) {
        printf("\033[31mCouldn't find file: %*s \033[0m\n", path.size(), path.begin());
        return {};
    }

    fseek(f, 0, SEEK_END);
    u32 len = ftell(f); rewind(f);
    string text(len);

    fread(text.begin(), 1, len, f);   
    fclose(f);
    return text;
}