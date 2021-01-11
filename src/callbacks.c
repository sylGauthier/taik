#include <3dmr/render/camera_buffer_object.h>

#include "taik.h"

void resize_callback(struct Viewer* viewer, void* data) {
    struct Taik* taik = data;
    struct UI* ui = &taik->ui;

    glViewport(0, 0, viewer->width, viewer->height);

    camera_set_ratio(((float)viewer->width) / ((float)viewer->height),
                     ui->cam.projection);

    camera_buffer_object_update_projection(&ui->scene.camera,
            MAT_CONST_CAST(ui->cam.projection));

    uniform_buffer_send(&ui->scene.camera);
}

void close_callback(struct Viewer* viewer, void* data) {
    struct Taik* taik = data;
    struct UI* ui = &taik->ui;

    ui->running = 0;
    return;
}

void cursor_callback(struct Viewer* viewer,
                     double xpos, double ypos,
                     double dx, double dy,
                     int bl, int bm, int br, void* data) {
    struct Taik* taik = data;
    Vec3 t;

    VEC(t, dx / 300., 0, 0);
    node_translate(taik->ship.node, t);
}
