#define PK_ENGINE_SRC
#include <Internal.hpp>
#include <cstdio>
#include <cctype>
#include <PKLib/file.hpp>
#include <PKCore/algorithm.hpp>
using namespace pk;

string file::read_src(vstring path) {
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

// first occurance of char between cur and end
char* find(char* cur, char* end, char c) {
    return (char*)std::memchr(cur, c, end - cur);
}

uint32_t nextu32(char **cur) {
    return std::strtoul(*cur, cur, 10);
}

float nextf32(char **cur) {
    return std::strtof(*cur, cur);
}

void file::read_obj(vstring path, Mesh& mesh) {
    string src = file::read_src(path);
    if (!src) return;
    printf("Reading OBJ %s\n", path.begin());
    
    vector<vec3> pos{1000};
    vector<vec3> nor{1000};
    vector<vec2> tex{1000};
    char *cur = src.begin(), *end = src.end();
    vector<uint64_t> pnt;
    auto &indices = mesh.indices;
    auto &vertices = mesh.vertices;

    indices.clear();
    vertices.clear();

    int duplicates = 0;
    while (cur < end) {
        while (cur < end && isspace(*cur)) ++cur;

        printf("\r%i vertices (duplicates %i), %i indices", pnt.size(), duplicates, indices.size());
        fflush(stdout);

        switch (*(cur++)) {
            case 'v': {
                switch (*cur) {
                    case 't': {
                        ++cur; // skip 't'
                        tex.emplace( nextf32(&cur), nextf32(&cur) );
                        break;
                    }
                    case 'n': {
                        ++cur; // skip 'n'
                        nor.emplace( nextf32(&cur), nextf32(&cur), nextf32(&cur) );
                        break;
                    }
                    default: {
                        pos.emplace( nextf32(&cur), nextf32(&cur), nextf32(&cur) );
                    }
                }
                break;
            }

            case 'f': {
                uint16_t v[4];
                uint8_t  n = 0;

                char *line_end = find(cur, end, '\n') ?: end;

                while (cur < line_end) {
                    uint64_t vertex =  nextu32(&cur); 
                    cur++;
                    vertex = (vertex<<16) | nextu32(&cur); 
                    cur++;
                    vertex = (vertex<<16) | nextu32(&cur);
                    
                    // duplicate vertex removal
                    uint16_t i = alg::low_bound({pnt}, vertex) - pnt.begin();
                    
                    if (i == pnt.size() || pnt[i] != vertex)
                        pnt.push_at(i, vertex);
                    else ++duplicates;

                    v[n++] = i;

                    if (n > 3) break;
                }

                if (n == 3) 
                    indices.push({v[0], v[1], v[2]});
                else if (n == 4) {
                    indices.push({
                        v[0], v[1], v[2],
                        v[0], v[2], v[3]
                    });
                }
                break;
            }
        }

        cur = find(cur, end, '\n'); // next line

        if (!cur) break; 
        else ++cur; // skips actual \n
    }

    vertices.reserve(pnt.size());
    for (uint64_t pnt_vertex : pnt) {
        vertices.emplace(
            pos[((pnt_vertex >> 32)) - 1],
            nor[(pnt_vertex & 0xFFFF) - 1],
            tex[((pnt_vertex >> 16) & 0xFFFF) - 1]
        );
    }

    printf("\nFinished reading OBJ %s, produced %i vertices, %i triangles\n", path.begin(), vertices.size(), indices.size() / 3);
}