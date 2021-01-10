#include <stdio.h>
#include <math.h>
#include <string.h>

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

static int collide_row(struct Ship* ship, struct Map* map, unsigned int row) {
    unsigned int i;
    for (i = 0; i < MAP_WIDTH; i++) {
        if (map->tiles[row][i].type != TA_EMPTY) {
            if (       fabs(map->tiles[row][i].node->position[0]
                            - ship->node->position[0]) <= 0.75) {
                return 1;
            }
        }
    }
    return 0;
}

static int collide(struct Ship* ship, struct Map* map) {
    unsigned int i;

    for (i = 0; i < MAP_LENGTH; i++) {
        if (fabs(map->rows[i]->position[1] - ship->node->position[1]) <= 0.75) {
            if (collide_row(ship, map, i)) return 1;
        }
    }
    return 0;
}

int taik_run(struct Taik* taik) {
    float dt;
    unsigned int score = 0;
    while (taik->ui.running) {
        dt = ui_render(&taik->ui);
        if (map_forward(&taik->map, taik->ship.speed * dt)) {
            score++;
            printf("score: %d\n", score);
            if (score % 100 == 0) taik->ship.speed += 0.5;
        }
        if (collide(&taik->ship, &taik->map)) {
            printf("game over\n");
            taik->ui.running = 0;
        }
    }
    return 1;
}

void taik_free(struct Taik* taik) {
    ui_free(&taik->ui);
    map_free(&taik->map);
    ship_free(&taik->ship);
}
