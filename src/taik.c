#include <stdio.h>

#include "taik.h"

int taik_init(struct Taik* taik) {
    int uiInit = 0, mapInit = 0, shipInit = 0;

    if (!(uiInit = ui_init(&taik->ui, taik))) {
        fprintf(stderr, "Error: can't init UI\n");
    } else if (!(mapInit = map_init(&taik->map, &taik->ui.cam))) {
        fprintf(stderr, "Error: can't init map\n");
    } else if (!scene_add(&taik->ui.scene, taik->map.root)) {
        fprintf(stderr, "Error: can't add root to scene\n");
    } else if (!(shipInit = ship_init(&taik->ship))) {
        fprintf(stderr, "Error: can't init ship\n");
    } else if (!scene_add(&taik->ui.scene, taik->ship.node)) {
        fprintf(stderr, "Error: can't add ship to scene\n");
    } else {
        return 1;
    }
    if (uiInit) ui_free(&taik->ui);
    if (mapInit) map_free(&taik->map);
    if (shipInit) ship_free(&taik->ship);
    return 0;
}

int taik_parse_options(struct Taik* taik, int argc, char** argv) {
    return 1;
}

int taik_run(struct Taik* taik) {
    float dt;
    while (taik->ui.running) {
        dt = ui_render(&taik->ui);
        map_forward(&taik->map, taik->ship.speed * dt);
    }
    return 1;
}

void taik_free(struct Taik* taik) {
    ui_free(&taik->ui);
    map_free(&taik->map);
    ship_free(&taik->ship);
}
