#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "../sponge.h"
#define SPONGE_EXAMPLE_IMPLEMENTATION
#define SPONGE_EXAMPLE_INIT_DEFINED
#define SPONGE_EXAMPLE_MOUSE_MOVE_DEFINED
#define SPONGE_EXAMPLE_DRAW_FRAME_3D_DEFINED
#include "example.h"

#define FOV (60.0f / PI * 2.0f)
#define DISTANCE (10.0f)

static int32_t mouse_x;
static int32_t mouse_y;

static sponge_Vec3 *positions;
static sponge_Color32 *colors;
static int32_t *triangles;
static size_t triangles_count;

// TODO(kard): make proper error checking
void init() {
    FILE *handle = fopen("examples/assets/teapot.txt", "r");
    assert(handle);
    fscanf(handle, "%zu", &triangles_count);
    triangles_count *= 3;
    positions = malloc(triangles_count * sizeof(sponge_Vec3));
    colors    = malloc(triangles_count * sizeof(sponge_Color32));
    triangles = malloc(triangles_count * sizeof(int32_t));
    assert(positions && colors && triangles);

    for (size_t i = 0; i < triangles_count; i++) {
        fscanf(handle, "%f %f %f", &positions[i].x, &positions[i].y, &positions[i].z);
        sponge_Vec3 normal;
        fscanf(handle, "%f %f %f", &normal.x, &normal.y, &normal.z);
        sponge_ColorF color;
        color.a = 255.0f;
        color.r = (normal.x + 1.0f) * (255.0f * 0.5f);
        color.g = (normal.y + 1.0f) * (255.0f * 0.5f);
        color.b = (normal.z + 1.0f) * (255.0f * 0.5f);
        colors[i] = sponge_colorf_to_color32(color);

        // invert winding
        int32_t t = (int32_t)((i / 3) * 3);
        triangles[i] = t + (2 - (i % 3));
    }

    fclose(handle);
}

void mouse_move(int32_t x, int32_t y) {
    mouse_x = x;
    mouse_y = y;
}

void draw_frame_3d(sponge_Texture c, float *depths) {
    sponge_clear_3d(c, depths, sponge_color32_make(0xFF000000));

    sponge_Mat4 model = sponge_mat4_identity();
    model = sponge_mat4_mul_mat4(model, sponge_mat4_translate(0.0f, -2.0f, 0.0f));

    float rot_y = (float)mouse_x / (float)c.width * PI * 2.0f;
    float rot_x = ((float)mouse_y / (float)c.height * PI) - (PI * 0.5f);
    sponge_Mat4 view = sponge_mat4_identity();
    view = sponge_mat4_mul_mat4(view, sponge_mat4_rotate(rot_x, rot_y, 0.0f));
    view = sponge_mat4_mul_mat4(view, sponge_mat4_translate(0.0f, 0.0f, -10.0f));


    sponge_Mat4 proj = sponge_mat4_projection(FOV, (float)c.width / (float)c.height, 1.0f, 100.0f);
    sponge_draw_mesh_col(c, depths, model, view, proj, positions, colors, triangles, triangles_count);
}
