#define PK_INTERNAL
#include <cstdio>
#include <PK/Util/file.hpp>
#include <PK/Core/algorithm.hpp>
using namespace pk;

String pk::read_file(SView path) {
    FILE* f = fopen(path, "rb");
    if (!f) {
        printf("Can't find file: '%*s'\n", path.size(), path.begin());
        return {};
    }

    fseek(f, 0, SEEK_END);
    u32 len = ftell(f); rewind(f);
    String text(len);

    fread(text.begin(), 1, len, f);   
    fclose(f);
    return text;
}