#include <Internal.hpp>

#include <cstdio>
#include <pkutil/File.hpp>
using namespace pk;
using namespace glm;

const char* File::read(const char* path, bool null, const char** end) {
    FILE* f = fopen(path, "rb");
    if (!f) {
        printf("Can't find file: '%s'\n", path);
        return new char('\0');
    }
    fseek(f, 0, SEEK_END);
    int len = ftell(f); rewind(f);
    char* data = new char[len+(null?1:0)];
    fread(data, 1, len, f);
    if (null) data[len++] = '\0';
    if (end) *end = data + len;
    return data;
}
/*
Mesh Mesh::from_file(const char *path) {
    const char *end, *src = File::read(path, true, &end), *cur = src;
    if (!src[0]) return Mesh();
    // just OBJ for now...
    Array<vec3> pos, norm;
    Array<vec2> uv;
    while (cur < end) {
        while (isspace(*cur)) ++cur;
        switch (*cur)
            case 'v': ++cur;
                switch(*cur) {
                    case 't': ++cur;
                    uv.push({
                        strtof(cur, (char**)&cur), 
                        strtof(cur, (char**)&cur)
                    }); break;
                }
                switch( )
                pos.push({
                    strtof(cur, (char**)&cur), 
                    strtof(cur, (char**)&cur), 
                    strtof(cur, (char**)&cur)
                }); break;
            case 'v': ++cur;
                pos.push({
                    strtof(cur, (char**)&cur), 
                    strtof(cur, (char**)&cur), 
                    strtof(cur, (char**)&cur)
                }); break;
            case 'v': ++cur;
                pos.push({
                    strtof(cur, (char**)&cur), 
                    strtof(cur, (char**)&cur), 
                    strtof(cur, (char**)&cur)
                }); break;
    }
};
*/