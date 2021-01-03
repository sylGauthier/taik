#include <stdio.h>

#include "taik.h"

int ship_init(struct Ship* ship) {
    ship->xPos = 0;
    return 1;
}

void ship_free(struct Ship* ship) {
    return;
}

int taik_init(struct Taik* taik) {
    int uiInit = 0, mapInit = 0, shipInit = 0;

    if (!(uiInit = ui_init(&taik->ui))) {
        fprintf(stderr, "Error: can't init UI\n");
    } else if (!(mapInit = map_init(&taik->map))) {
        fprintf(stderr, "Error: can't init map\n");
    } else if (!(shipInit = ship_init(&taik->ship))) {
        fprintf(stderr, "Error: can't init ship\n");
    } else {
        taik->ui.taik = taik;
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
    while (1) {
        ui_render(&taik->ui);
    }
    return 1;
}

void taik_free(struct Taik* taik) {
    ui_free(&taik->ui);
}
