#include <stdio.h>
#include <stdlib.h>

#include <3dmr/render/camera_buffer_object.h>

#include "taik.h"

void resize_callback(struct Viewer* viewer, void* data);
void close_callback(struct Viewer* viewer, void* data);
void cursor_callback(struct Viewer* viewer,
                     double xpos, double ypos,
                     double dx, double dy,
                     int bl, int bm, int br, void* data);

void update_node(struct Scene* scene, struct Node* n, void* data) {
    switch (n->type) {
        case NODE_DLIGHT:
            break;
        case NODE_PLIGHT:
            break;
        case NODE_SLIGHT:
            break;
        case NODE_CAMERA:
            camera_buffer_object_update_view_and_position(&scene->camera,
                    MAT_CONST_CAST(n->data.camera->view));
            break;
        default:;
    }
}

int ui_init(struct UI* ui, void* cbData) {
    int sceneInit = 0;
    ui->viewer = NULL;
    ui->camNode = NULL;
    ui->running = 1;

    camera_projection(1., 30 / 360. * 2 * M_PI, 0.001, 1000.,
                      ui->cam.projection);

    if (!(ui->viewer = viewer_new(640, 480, "taik"))) {
        fprintf(stderr, "Error: can't create viewer\n");
    } else if (!(sceneInit = scene_init(&ui->scene, &ui->cam))) {
        fprintf(stderr, "Error: can't init scene\n");
    } else if (!(ui->camNode = calloc(1, sizeof(struct Node)))) {
        fprintf(stderr, "Error: can't allocate node\n");
    } else {
        Vec3 t = {0, -10, 15};

        ui->viewer->callbackData = cbData;
        ui->viewer->resize_callback = resize_callback;
        ui->viewer->key_callback = NULL;
        ui->viewer->cursor_callback = cursor_callback;
        ui->viewer->mouse_callback = NULL;
        ui->viewer->close_callback = close_callback;

        viewer_set_cursor_mode(ui->viewer, VIEWER_CURSOR_DISABLED);

        camera_set_ratio(
                ((float)ui->viewer->width) / ((float)ui->viewer->height),
                ui->cam.projection);
        camera_buffer_object_update_projection(
                &ui->scene.camera,
                MAT_CONST_CAST(ui->cam.projection));
        uniform_buffer_send(&ui->scene.camera);

        node_init(ui->camNode);
        if (!scene_add(&ui->scene, ui->camNode)) {
            fprintf(stderr, "Error: can't add root to scene\n");
        } else {
            Vec3 axis = {1, 0, 0};
            node_set_camera(ui->camNode, &ui->cam);
            node_set_pos(ui->camNode, t);
            node_rotate(ui->camNode, axis, M_PI / 5.);
            glfwSwapInterval(1);
            return 1;
        }
    }
    ui_free(ui);
    if (sceneInit) scene_free(&ui->scene, NULL);
    return 0;
}

int ui_update_map(struct UI* ui, struct Map* map) {
    return 1;
}

float ui_render(struct UI* ui) {
    float dt;
    dt = viewer_next_frame(ui->viewer);
    viewer_process_events(ui->viewer);

    scene_update_nodes(&ui->scene, update_node, NULL);
    scene_update_render_queue(&ui->scene, MAT_CONST_CAST(ui->cam.view),
                                          MAT_CONST_CAST(ui->cam.projection));

    uniform_buffer_send(&ui->scene.camera);

    scene_render(&ui->scene);
    return dt;
}

void ui_free(struct UI* ui) {
    scene_free(&ui->scene, NULL);
    viewer_free(ui->viewer);
    free(ui->camNode);
}
