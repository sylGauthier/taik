#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <3dmr/mesh/box.h>

#include "taik.h"

void ship_update(struct Ship* ship) {
    Vec3 axis = {0, 1, 0};
    Quaternion q;
    float dx = ship->node->position[0] - ship->prevx;

    quaternion_set_axis_angle(q, axis, dx * 5);
    node_set_orientation(ship->node, q);
    ship->prevx = ship->node->position[0];
}

int ship_load(struct Ship* ship) {
    FILE* f;
    int ok = 1;
    if (!(f = fopen("data/ship.glb", "r"))) {
        fprintf(stderr, "Error: cant open ship glb file\n");
    } else {
        memset(&ship->metadata, 0, sizeof(ship->metadata));
        memset(&ship->shared, 0, sizeof(ship->shared));
        ok = gltf_load(ship->node, f, "data/",
                       &ship->shared, &ship->metadata, 1);
        fclose(f);
        return ok;
    }
    return 0;
}

int ship_init(struct Ship* ship) {
    ship->node = NULL;
    if (!(ship->node = malloc(sizeof(struct Node)))) {
        fprintf(stderr, "Error: can't allocate ship node\n");
    } else {
        Vec3 t = {0, 0, 0.5};
        node_init(ship->node);
        if (ship_load(ship)) {
            node_translate(ship->node, t);
            ship->speed = 2.;
            ship->prevx = 0;
            return 1;
        }
    }
    free(ship->node);
    return 0;
}

void ship_free(struct Ship* ship) {
    nodes_free(ship->node, imported_node_free);
    import_free_shared_data(&ship->shared);
    import_free_metadata(&ship->metadata);
    return;
}
