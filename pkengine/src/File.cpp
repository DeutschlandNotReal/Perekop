#include <cstdio>

#include <pkutil/File.hpp>

const char* pk::File::read(const char* path, const char** end) {
    FILE* f = fopen(path, "rb");
    if (!f) {
        printf("Can't find file: '%s'\n", path);
        return "";
    }
    fseek(f, 0, SEEK_END);
    int len = ftell(f); rewind(f);
    char* data = new char[len+1];
    fread(data, 1, len, f);
    data[len] = '\0';
    if (end) *end = data + len;
    return data;
}

const char* pk::File::rawread(const char* path, const char** end) {
    FILE* f = fopen(path, "rb");
    if (!f) {
        printf("Can't find file: '%s'\n", path);
        return "";
    }
    fseek(f, 0, SEEK_END);
    int len = ftell(f); rewind(f);
    char* data = new char[len];
    fread(data, 1, len, f);
    if (end) *end = data + len;
    return data;
}