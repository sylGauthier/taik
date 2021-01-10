#include <stdio.h>
#include <stdlib.h>

#include <3dmr/mesh/box.h>

#include "taik.h"

static int ship_asset_init(struct Asset* asset) {
    int meshInit = 0;
    struct Mesh mesh;

    if (!(asset->matpar = solid_material_params_new())) {
        fprintf(stderr, "Error: can't create material param\n");
    } else if (!(meshInit = make_box(&mesh, 0.5, 0.5, 0.5))) {
        fprintf(stderr, "Error: can't create ship mesh\n");
    } else if (!(asset->geom.vertexArray = vertex_array_new(&mesh))) {
        fprintf(stderr, "Error: can't create ship vertex array\n");
    } else if (!(asset->geom.material =
                 solid_material_new(mesh.flags, asset->matpar))) {
        fprintf(stderr, "Error: can't init ship material\n");
    } else {
        mesh_free(&mesh);
        material_param_set_vec3_elems(&asset->matpar->color, 0.5, 0.1, 0);
        return 1;
    }
    free(asset->matpar);
    if (meshInit) mesh_free(&mesh);
    if (asset->geom.vertexArray) vertex_array_free(asset->geom.vertexArray);
    if (asset->geom.material) free(asset->geom.material);
    return 0;
}

int ship_init(struct Ship* ship) {
    if (!ship_asset_init(&ship->asset)) {
        fprintf(stderr, "Error: can't init ship asset\n");
    } else if (!(ship->node = malloc(sizeof(struct Node)))) {
        fprintf(stderr, "Error: can't allocate ship node\n");
    } else {
        Vec3 t = {0, -3, 0};
        node_init(ship->node);
        node_translate(ship->node, t);
        node_set_geometry(ship->node, &ship->asset.geom);
        ship->xPos = 0;
        ship->speed = 2.;
        return 1;
    }
    return 0;
}

void ship_free(struct Ship* ship) {
    free(ship->node);
    vertex_array_free(ship->asset.geom.vertexArray);
    free(ship->asset.geom.material);
    free(ship->asset.matpar);
    return;
}
