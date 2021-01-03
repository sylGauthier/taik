#ifndef TAIK_H
#define TAIK_H

#include <3dmr/scene/scene.h>
#include <3dmr/material/solid.h>

#define MAP_LENGTH  256
#define MAP_WIDTH   8

#define VEC(v, x, y, z) v[0] = x; v[1] = y; v[2] = z;


/* map stuff */

enum TileType {
    TA_EMPTY,
    TA_WALL,
    TA_NUM_TILE_TYPES
};

struct Tile {
    struct Node* node;
    enum TileType type;
};

struct Map {
    struct Geometry assets[TA_NUM_TILE_TYPES];
    struct Tile tiles[MAP_LENGTH][MAP_WIDTH];
    struct Node* root;
    unsigned int last;
};

int map_init(struct Map* map);
void map_free(struct Map* map);


/* ship stuff */

struct Ship {
    float xPos;
};

int ship_init(struct Ship* ship);


/* UI stuff */

struct UI {
    struct Scene scene;
    struct Camera cam;
    struct Viewer* viewer;

    struct Node* camNode;

    struct Taik* taik;
};

int ui_init(struct UI* ui);
int ui_update_map(struct UI* ui, struct Map* map);
int ui_render(struct UI* ui);
void ui_free(struct UI* ui);

/* main stuff */

struct Taik {
    struct UI ui;
    struct Map map;
    struct Ship ship;
};

int taik_init(struct Taik* taik);
int taik_parse_options(struct Taik* taik, int argc, char** argv);
int taik_run(struct Taik* taik);
void taik_free(struct Taik* taik);

#endif
