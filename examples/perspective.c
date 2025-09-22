#include "../sponge.h"
#define SPONGE_EXAMPLE_IMPLEMENTATION
#define SPONGE_EXAMPLE_MOUSE_MOVE_DEFINED
#define SPONGE_EXAMPLE_DRAW_FRAME_3D_DEFINED
#include "example.h"

#define FOV (60.0f / PI * 2.0f)

static int32_t mouse_x;
static int32_t mouse_y;

void mouse_move(int32_t x, int32_t y) {
    mouse_x = x;
    mouse_y = y;
}

void draw_frame_3d(sponge_Texture c, float *depths) {
    sponge_clear_3d(c, depths, sponge_color32_make(0xFF000000));

    sponge_Vec3 positions[] = {
        sponge_vec3_make(-0.5f,  0.5f, 0.0f),
        sponge_vec3_make( 0.5f,  0.5f, 0.0f),
        sponge_vec3_make(-0.5f, -0.5f, 0.0f),
        sponge_vec3_make( 0.5f, -0.5f, 0.0f),
    };

    sponge_Color32 colors[] = {
        sponge_color32_make(0xFF00FFFF),
        sponge_color32_make(0xFFFFFFFF),
        sponge_color32_make(0xFF0000FF),
        sponge_color32_make(0xFFFF00FF),
    };

    int32_t triangles[] = {
        0, 1, 2,
        2, 1, 3,
    };

    sponge_Mat4 model = sponge_mat4_identity();
    model = sponge_mat4_mul_mat4(model, sponge_mat4_rotate(
        (float)(mouse_y - ((int32_t)c.height / 2)) / (float)c.height * PI,
        (float)(mouse_x - ((int32_t)c.width / 2)) / (float)c.width * PI,
        0.0f));
    model = sponge_mat4_mul_mat4(model, sponge_mat4_translate(0.0f, 0.0f, -5.0f));
    sponge_Mat4 view = sponge_mat4_identity();
    sponge_Mat4 proj = sponge_mat4_projection(FOV, (float)c.width / (float)c.height, 1.0f, 10.0f);
    sponge_draw_mesh_col(c, depths, model, view, proj, positions, colors, triangles, 6);
}
