#include <winscard.h>
#define PK_ENGINE_SRC
#define PK_PRINT_OBJ

#include <Internal.hpp>

#include <cstdio>
#include <PKLib/File.hpp>
#include <PKLib/Geometry.hpp>
using namespace pk;

string File::read(rstring path) {
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

Mesh::Mesh(rstring path) {
    string src = File::read(path);
    if (!src) return;

    printf("Reading OBJ %s\n", path.begin());
    
    rstring reader{src};

    vector<vec3> pos{16};
    vector<vec3> nor{16};
    vector<vec2> tex{16};

    // sort of a map, BLOAT!
    struct entry { 
        uint16_t id; 
        vector<entry> leaf;
    };
    vector<entry> vertex_map;

    auto id_search = [](const entry& e, uint16_t id){ return e.id == id; };

    while (true) {
        reader.skip_whitespace();

        switch (reader.consume()) {
            case 'v': {
                switch (reader[0]) {
                    case 't': { // uv
                        tex.emplace(
                            reader.nextf32(),
                            reader.nextf32()
                        );

                        reader.advance(); break;
                    }
                    case 'n': { // normal
                        nor.emplace(
                            reader.nextf32(),
                            reader.nextf32(),
                            reader.nextf32()
                        );

                        reader.advance(); break;
                    }
                    default: { // position
                        pos.emplace(
                            reader.nextf32(),
                            reader.nextf32(),
                            reader.nextf32()
                        );
                    }
                }
                break;
            }

            case 'f': {
                uint16_t vids[4];
                int vcount{0};

                auto* nline = reader.find('\n');
                nline = (nline ? nline : reader.end());

                while (reader.begin() < nline) {
                    uint16_t p = reader.nexti64();
                    reader.advance();
                    uint16_t t = reader.nexti64();
                    reader.advance();
                    uint16_t n = reader.nexti64();

                    uint16_t v = vertices.size();

                    // i should probably make a map class soon
                    entry* p_find = vertex_map.lfind(id_search, p);
                    if (p_find) {
                        entry* t_find = p_find->leaf.lfind(id_search, t);
                        if (t_find) {
                            entry* n_find = t_find->leaf.lfind(id_search, n);
                            if (n_find) { v = *(uint16_t*)((char*)&n_find->leaf + 8); } else {
                                entry& n_vec = t_find->leaf.emplace(n, vector<entry>());
                                *(uint16_t*)((char*)&n_vec.leaf + 8) = v;
                            }
                        } else {
                            entry& t_vec = p_find->leaf.emplace(t, vector<entry>());
                            entry& n_vec = t_vec.leaf.emplace(n, vector<entry>());
                            *(uint16_t*)((char*)&n_vec.leaf + 8) = v;
                        }
                    } else {
                        entry& p_vec = vertex_map.emplace(p, vector<entry>());
                        entry& t_vec = p_vec.leaf.emplace(t, vector<entry>());
                        entry& n_vec = t_vec.leaf.emplace(n, vector<entry>());
                        *(uint16_t*)((char*)&n_vec.leaf + 8) = v;
                    }
                    
                    vids[vcount++] = v;
                    if (vcount >=4) break;
                    if (v == vertices.size()) {
                        vertices.emplace(pos[p], nor[n], tex[t]);
                    }
                }

                if (vcount == 3) {
                    indices.push({vids[0], vids[1], vids[2]});
                } else if (vcount == 4) {
                    indices.push({
                        vids[0], vids[1], vids[2],
                        vids[0], vids[2], vids[3]
                    });
                } else {
                    printf("%i, what are we doing here?\n", vcount);
                }
                break;
            }
        }

        auto* nline = reader.find('\n');
        if (nline) { reader.point(nline); } else { break; }
    }

    printf("Finished reading OBJ %s, produced %i vertices, %i triangles\n", path.begin(), vertices.size(), indices.size() / 3);
}