#include <stdio.h>
#include <stdlib.h>

#include <3dmr/mesh/box.h>

#include "taik.h"

static int init_wall(struct Geometry* asset) {
    int meshInit = 0;
    struct Mesh cube;
    struct SolidMaterialParams params;

    material_param_set_vec3_elems(&params.color, 0.5, 0.1, 0.);

    if (!(meshInit = make_box(&cube, 1, 1, 1))) {
        fprintf(stderr, "Error: can't init mesh\n");
    } else if (!(asset->vertexArray = vertex_array_new(&cube))) {
        fprintf(stderr, "Error: can't init vertex array\n");
    } else if (!(asset->material = solid_material_new(cube.flags, &params))) {
        fprintf(stderr, "Error: can't init material\n");
    } else {
        return 1;
    }
    if (meshInit) mesh_free(&cube);
    if (asset->vertexArray) vertex_array_free(asset->vertexArray);
    if (asset->material) free(asset->material);
    return 0;
}

static int init_assets(struct Map* map) {
    unsigned int i;
    for (i = 0; i < TA_NUM_TILE_TYPES; i++) {
        map->assets[i].vertexArray = NULL;
        map->assets[i].material = NULL;
    }

    if (!init_wall(map->assets + TA_WALL)) return 0;
    return 1;
}

static int set_tile(struct Map* map, unsigned int i, unsigned int j, int t) {
    Vec3 pos;
    VEC(pos, i, 0, j);

    if (!(map->tiles[i][j].node = malloc(sizeof(struct Node)))) return 0;
    node_init(map->tiles[i][j].node);
    node_set_pos(map->tiles[i][j].node, pos);
    if (map->assets[t].vertexArray) {
        node_set_geometry(map->tiles[i][j].node, &map->assets[t]);
    }
    return node_add_child(map->root, map->tiles[i][j].node);
}

int map_init(struct Map* map) {
    unsigned int i, j;
    if (!init_assets(map)) return 0;
    for (i = 0; i < MAP_LENGTH; i++) {
        for (j = 0; j < MAP_WIDTH; j++) {
            set_tile(map, i, j, (i + j % 2) ? TA_EMPTY : TA_WALL);
        }
    }
    map->last = MAP_LENGTH - 1;
    return 1;
}

void map_free(struct Map* map) {
    return;
}
