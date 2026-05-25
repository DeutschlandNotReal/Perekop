#pragma once
#include <cstdio>
#include <iostream>

namespace pk::File { 
    static const char* read(const char* path) noexcept {
        FILE* f = fopen(path, "rb");
        if (!f) {
            std::cout << "Can't find file: '" << path << "'\n";
            return "\0";
        }
        fseek(f, 0, SEEK_END);
        int len = ftell(f);
        rewind(f);
        char* data = new char[len+1];
        fread(data, 1, len, f);
        data[len] = '\0';
        fclose(f);
        return data;
    }
}