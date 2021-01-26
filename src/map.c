#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <3dmr/mesh/box.h>
#include <3dmr/mesh/quad.h>
#include <3dmr/render/texture.h>

#include "taik.h"

#define TA_DATA_PATH "data/"

static int steal_geometry(struct Asset* a,
                          struct Node* n,
                          struct ImportSharedData* sh) {
    struct Geometry* g = &a->geom;
    unsigned int i;

    g->vertexArray = n->data.geometry->vertexArray;
    g->material = n->data.geometry->material;
    a->matpar = g->material->params;
    n->type = NODE_EMPTY;
    free(n->data.geometry);
    for (i = 0; i < sh->numVA; i++) {
        if (sh->va[i] == g->vertexArray) {
            sh->va[i] = NULL;
            break;
        }
    }
    for (i = 0; i < sh->numMatParams; i++) {
        if (sh->matParams[i] == g->material->params) {
            sh->matParams[i] = NULL;
            break;
        }
    }
    return 1;
}

static int init_wall(struct Asset* asset) {
    FILE* f = NULL;
    struct Node* n = NULL;
    struct ImportMetadata meta = {0};
    struct ImportSharedData shared = {0};
    int ok = 1;

    if (!(f = fopen(TA_DATA_PATH "wall.glb", "r"))) {
        fprintf(stderr, "Error: can't open wall glb file\n");
        ok = 0;
    } else if (!(n = malloc(sizeof(*n)))) {
        fprintf(stderr, "Error: can't allocate node\n");
        ok = 0;
    } else {
        node_init(n);
        if (!gltf_load(n, f, TA_DATA_PATH, &shared, &meta, 1)) {
            fprintf(stderr, "Error: can't parse wall glb file\n");
            ok = 0;
        } else if (!n->nbChildren || n->children[0]->type != NODE_GEOMETRY) {
            ok = 0;
        } else {
            ok = steal_geometry(asset, n->children[0], &shared);
        }
    }
    if (f) fclose(f);
    nodes_free(n, imported_node_free);
    import_free_shared_data(&shared);
    import_free_metadata(&meta);
    return ok;
}

static int init_empty(struct Asset* asset) {
    int meshInit = 0;
    struct Mesh quad;

    if (!(asset->matpar = pbr_material_params_new())) {
        fprintf(stderr, "Error: can't create material param\n");
    } else if (!(meshInit = make_quad(&quad, 1, 1))) {
        fprintf(stderr, "Error: can't init mesh\n");
    } else if (!(asset->geom.vertexArray = vertex_array_new(&quad))) {
        fprintf(stderr, "Error: can't init vertex array\n");
    } else {
        GLuint tex;
        mesh_free(&quad);
        tex = texture_load_from_png("data/textures/sand.png");
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        material_param_set_vec3_texture(&asset->matpar->albedo, tex);
        if (!(asset->geom.material =
                    pbr_material_new(quad.flags, asset->matpar))) {
            fprintf(stderr, "Error: can't init material\n");
        } else {
            return 1;
        }
    }
    free(asset->matpar);
    if (meshInit) mesh_free(&quad);
    if (asset->geom.vertexArray) vertex_array_free(asset->geom.vertexArray);
    if (asset->geom.material) free(asset->geom.material);
    return 0;
}

static int init_assets(struct Map* map) {
    unsigned int i;
    for (i = 0; i < TA_NUM_TILE_TYPES; i++) {
        map->assets[i].matpar = NULL;
        map->assets[i].geom.vertexArray = NULL;
        map->assets[i].geom.material = NULL;
    }

    if (!init_empty(map->assets + TA_EMPTY)) return 0;
    if (!init_wall(map->assets + TA_WALL)) return 0;
    return 1;
}

static int init_tile(struct Map* map,
                     unsigned int row,
                     unsigned int col) {
    Vec3 pos;
    VEC(pos, (float)col - (float)MAP_WIDTH / 2. + 0.5, 0, 0);

    if (!(map->tiles[row][col].node = malloc(sizeof(struct Node)))) return 0;
    node_init(map->tiles[row][col].node);
    node_set_pos(map->tiles[row][col].node, pos);
    return node_add_child(map->rows[row], map->tiles[row][col].node);
}

static void set_tile(struct Map* map,
                     unsigned int row,
                     unsigned int col,
                     int t) {
    struct Node* n = map->tiles[row][col].node;
    Vec3 pos;

    map->tiles[row][col].type = t;
    if (map->assets[t].geom.vertexArray) {
        node_set_geometry(n, &map->assets[t].geom);
    }
    VEC(pos, n->position[0], n->position[1], t ? 0.5 : 0);
    node_set_pos(n, pos);
}

static void gen_row(struct Map* map, unsigned int row) {
    unsigned int i;
    int t;

    for (i = 0; i < MAP_WIDTH; i++) {
        if (i == 0 || i == MAP_WIDTH - 1) {
            t = TA_WALL;
        } else {
            t = rand() % TA_NUM_TILE_TYPES;
            t *= !(row % 3);
        }
        set_tile(map, row, i, t);
    }
}

static int create_row(struct Map* map, unsigned int row) {
    if (!(map->rows[row] = malloc(sizeof(struct Node)))) {
        fprintf(stderr, "Error: can't create row\n");
    } else {
        node_init(map->rows[row]);
        if (!(node_add_child(map->root, map->rows[row]))) {
            fprintf(stderr, "Error: can't add row to root\n");
        } else {
            Vec3 t;
            VEC(t, 0, row, 0);
            node_set_pos(map->rows[row], t);
            return 1;
        }
    }
    if (map->rows[row]) node_rm_child(map->root, map->rows[row]);
    free(map->rows[row]);
    return 0;
}

int map_init(struct Map* map, struct Camera* cam) {
    unsigned int row, col, ok = 1;

    map->root = NULL;
    map->first = 0;
    map->cam = cam;
    memset(map->assets, 0, sizeof(map->assets));
    memset(map->tiles, 0, sizeof(map->tiles));

    if (!init_assets(map)) {
        ok =  0;
    } else if (!(map->root = malloc(sizeof(*map->root)))) {
        ok = 0;
    } else {
        node_init(map->root);
        for (row = 0; row < MAP_LENGTH; row++) {
            if (!create_row(map, row)) {
                ok = 0;
                break;
            }
            for (col = 0; col < MAP_WIDTH; col++) {
                if (!init_tile(map, row, col)) {
                    ok = 0;
                    break;
                }
            }
            if (row < 5) {
                unsigned int i;
                for (i = 0; i < MAP_WIDTH; i++) {
                    set_tile(map, row, i, i % (MAP_WIDTH - 1)
                                          ? TA_EMPTY : TA_WALL);
                }
            } else {
                gen_row(map, row);
            }
        }
    }
    if (!ok) map_free(map);
    return ok;
}

void map_free(struct Map* map) {
    unsigned int i, j;

    for (i = 0; i < MAP_LENGTH; i++) {
        free(map->rows[i]->children);
        free(map->rows[i]);
        for (j = 0; j < MAP_WIDTH; j++) {
            free(map->tiles[i][j].node);
        }
    }
    free(map->root->children);
    free(map->root);

    for (i = 0; i < TA_NUM_TILE_TYPES; i++) {
        vertex_array_free(map->assets[i].geom.vertexArray);
        free(map->assets[i].geom.material);
        free(map->assets[i].matpar);
    }
}

static void reset_row(struct Map* map, unsigned int row) {
    unsigned int i;

    for (i = 0; i < MAP_WIDTH; i++) {
        map->tiles[row][i].node->type = NODE_EMPTY;
        map->tiles[row][i].type = TA_EMPTY;
    }
}

int map_forward(struct Map* map, float dy) {
    Vec3 t;
    unsigned int row;

    VEC(t, 0, -dy, 0);

    for (row = 0; row < MAP_LENGTH; row++) {
        node_translate(map->rows[row], t);
    }
    if (!node_visible(map->rows[map->first],
                      MAT_CONST_CAST(map->cam->view),
                      MAT_CONST_CAST(map->cam->projection))) {
        reset_row(map, map->first);
        gen_row(map, map->first);
        t[1] = MAP_LENGTH;
        node_translate(map->rows[map->first], t);
        map->first = (map->first + 1) % MAP_LENGTH;
        return 1;
    }
    return 0;
}
