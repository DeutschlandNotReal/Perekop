#pragma once 

namespace pk::File {
    const char* read(const char* path, const char** end = nullptr);
    const char* rawread(const char* path, const char** end = nullptr);
}