#define PK_ENGINE_SRC
#define PK_PRINT_OBJ

#define PK_DEBUG "file.cpp"
#define PK_DEBUG_VEC ~0

#include <Internal.hpp>

#include <cstdio>
#include <PKLib/File.hpp>
#include <PKLib/Geometry.hpp>
using namespace pk;

string File::read(refstring path) {
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

Mesh::Mesh(refstring path) {
    string src = File::read(path);
    if (!src) return;
    #ifdef PK_PRINT_OBJ
        printf("Reading OBJ %s\n", path.begin());
    #endif
    
    refstring reader{src};
    vector<vec3> vpos, vnor;
    vector<vec2> vtex;
    struct OBJTVert { short p, t, n; };

    bool has_newline{true};
    while (has_newline) {
        reader.skip_space();
        switch (reader.consume()) {
            case 'v': {
                switch (reader[0]) {
                    case 't': { // uv
                        vec2 v{
                            reader.next_float(),
                            reader.next_float()
                        }; 
                        vtex.push(v);
                        reader.adv();
                        #ifdef PK_PRINT_OBJ
                            printf("OBJ: vt%i (%.2f, %.2f)\n", vtex.size() - 1, v.x, v.y);
                        #endif
                        break;
                    }
                    case 'n': { // normal
                        vec3 v{
                            reader.next_float(),
                            reader.next_float(),
                            reader.next_float()
                        }; 
                        vnor.push(v);
                        reader.adv(); 
                        #ifdef PK_PRINT_OBJ
                            printf("OBJ: vn%i (%.2f, %.2f, %.2f)\n", vnor.size() - 1, v.x, v.y, v.z);
                        #endif
                        break;
                    }
                    default: { // position
                        vec3 v{
                            reader.next_float(),
                            reader.next_float(),
                            reader.next_float()
                        };
                        vpos.push(v);
                        #ifdef PK_PRINT_OBJ
                            printf("OBJ: v%i (%.2f, %.2f, %.2f)\n", vpos.size() - 1, v.x, v.y, v.z);
                        #endif
                        break;
                    }
                    break;
                }
            }

            case 'f': {
                OBJTVert V[3];

                for (int i = 0; i < 3; i++) {
                    short p = reader.next_long(), uv, n;
                    if (reader[0] == '/' && reader[1] == '/') {
                        uv = -1; reader.adv(); n = reader.next_long();
                    } else {
                        uv = reader.next_long(); reader.adv();
                        n = reader.next_long();
                    }
                    V[i] = {p, uv, n};
                }

                for (int i = 0; i < 3; i++) {
                    vertices.emplace(
                        vpos[V[i].p],
                        vnor[V[i].n],
                        (V[i].t == -1 ? vec2{0, 0} : vtex[V[i].t])
                    );

                    uint16_t last = vertices.size() - 1;
                    indices.push({last, ++last, ++last});

                    #ifdef PK_PRINT_OBJ
                        printf("OBJ: t (v%i, v%i, v%i)\n", vtex.size() - 1, last - 2, last - 1, last);
                    #endif
                }

                break;
            }
        }

        reader.next_line(&has_newline);
    }
}