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
    pos.emplace(0);
    nor.emplace(0);
    tex.emplace(0);

    char *cur = src.begin(), *end = src.end();
    struct OBJVertex { 
        uint64_t data; 
        bool operator==(const OBJVertex& b) const { return (data >> 16) == (b.data >> 16); }
        bool operator<(const OBJVertex& b) const { return (data >> 16) < (b.data >> 16); }
        bool operator>(const OBJVertex& b) const { return (data >> 16) > (b.data >> 16); }

        uint16_t* id()  const { return ((uint16_t*)&data); }
        uint16_t* pos() const { return ((uint16_t*)&data) + 1; }
        uint16_t* tex() const { return ((uint16_t*)&data) + 2; }
        uint16_t* nor() const { return ((uint16_t*)&data) + 3; }
        OBJVertex() = default;
        OBJVertex(uint16_t p, uint16_t t, uint16_t n):
            data((uint64_t)p<<16 | (uint64_t)t<<32 | (uint64_t)n<<48)
        {}
    };

    vector<OBJVertex> vertex_ptn;
    auto &indices = mesh.indices;
    auto &vertices = mesh.vertices;

    indices.clear();
    vertices.clear();

    while (cur < end) {
        while (cur < end && isspace(*cur)) ++cur;

        switch (*(cur++)) {
            case 'v': {
                switch (*cur) {
                    case 't': {
                        ++cur; // skip 't'
                        tex.emplace_back( nextf32(&cur), nextf32(&cur) );
                        break;
                    }
                    case 'n': {
                        ++cur; // skip 'n'
                        nor.emplace_back( nextf32(&cur), nextf32(&cur), nextf32(&cur) );
                        break;
                    }
                    default: {
                        pos.emplace_back( nextf32(&cur), nextf32(&cur), nextf32(&cur) );
                    }
                }
                break;
            }

            case 'f': {
                uint16_t v[4];
                uint8_t  n = 0;

                char *line_end = find(cur, end, '\n') ?: end;

                while (cur < line_end) {
                    uint64_t v_pos = nextu32(&cur), v_tex = 0, v_nor = 0;
                    if (*cur == '/' && *(cur+1) == '/') {
                        // case: p//n
                        cur += 2;
                        v_nor = nextu32(&cur);
                    } else if (*cur == '/') {
                        // case: p/t
                        ++cur;
                        v_tex = nextu32(&cur);
                        if (*cur == '/') {
                            // case: p/t/n
                            ++cur;
                            v_nor = nextu32(&cur);
                        }
                    }
                    // case: p (do nothing more)

                    OBJVertex v_ptn(v_pos, v_tex, v_nor);
                    
                    // duplicate vertex removal
                    OBJVertex* match = lower_bound({vertex_ptn}, v_ptn);
                    uint16_t i = vertex_ptn.size();

                    if (match == vertex_ptn.end() ||  !(*match == v_ptn)) {
                        *v_ptn.id() = i;
                        vertex_ptn.push(match - vertex_ptn.begin(), v_ptn);
                    } else {
                        i = *match->id();
                    }

                    v[n++] = i;

                    if (n > 3) break;
                }

                if (n == 3) 
                    indices.push_back({v[0], v[1], v[2]});
                else if (n == 4) {
                    indices.push_back({
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

    vertices.reserve(vertex_ptn.size());
    for (OBJVertex& v_pnt : vertex_ptn) {
        vertices.emplace_back(
            pos[*v_pnt.pos()],
            nor[*v_pnt.nor()],
            tex[*v_pnt.tex()]
        ); 
    }

    printf("\nFinished reading OBJ %s, produced %i vertices, %i triangles\n", path.begin(), vertices.size(), indices.size() / 3);
}