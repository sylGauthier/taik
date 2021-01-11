#ifndef TAIK_H
#define TAIK_H

#include <3dmr/scene/scene.h>
#include <3dmr/material/pbr.h>

#define MAP_LENGTH  32
#define MAP_WIDTH   12

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

struct Asset {
    struct Geometry geom;
    struct PBRMaterialParams* matpar;
};

struct Map {
    struct Asset assets[TA_NUM_TILE_TYPES];
    struct Tile tiles[MAP_LENGTH][MAP_WIDTH];
    struct Node *root, *rows[MAP_LENGTH];
    struct Camera* cam;
    unsigned int first;
};

int map_init(struct Map* map, struct Camera* cam);
void map_free(struct Map* map);
int map_forward(struct Map* map, float dy);


/* ship stuff */

struct Ship {
    struct Asset asset;
    struct Node* node;
    float prevx;
    float speed;
};

int ship_init(struct Ship* ship);
void ship_free(struct Ship* ship);
void ship_update(struct Ship* ship);


/* UI stuff */

struct UI {
    struct Scene scene;
    struct Camera cam;
    struct Viewer* viewer;

    struct Node* camNode;
    char running;
};

int ui_init(struct UI* ui, void* taik);
int ui_update_map(struct UI* ui, struct Map* map);
float ui_render(struct UI* ui);
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
