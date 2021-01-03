#include <stdio.h>
#include <stdlib.h>

#include <3dmr/render/camera_buffer_object.h>

#include "taik.h"

int ui_init(struct UI* ui) {
    int sceneInit = 0;
    ui->viewer = NULL;
    ui->camNode = NULL;

    camera_projection(1., 30 / 360. * 2 * M_PI, 0.001, 1000.,
                      ui->cam.projection);

    if (!(ui->viewer = viewer_new(640, 480, "taik"))) {
        fprintf(stderr, "Error: can't create viewer\n");
    } else if ((sceneInit = scene_init(&ui->scene, &ui->cam))) {
        fprintf(stderr, "Error: can't init scene\n");
    } else if (!(ui->camNode = malloc(sizeof(struct Node)))) {
        fprintf(stderr, "Error: can't allocate node\n");
    } else {
        node_init(ui->camNode);
        return 1;
    }
    ui_free(ui);
    if (sceneInit) scene_free(&ui->scene, NULL);
    return 0;
}

int ui_update_map(struct UI* ui, struct Map* map) {
    return 1;
}

int ui_render(struct UI* ui) {
    viewer_process_events(ui->viewer);
    uniform_buffer_send(&ui->scene.camera);
    viewer_next_frame(ui->viewer);
    return 1;
}

void ui_free(struct UI* ui) {
    viewer_free(ui->viewer);
    free(ui->camNode);
}
