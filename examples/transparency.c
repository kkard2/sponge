#include <math.h>
#include <stdlib.h>

#include "../sponge.h"
#define SPONGE_EXAMPLE_IMPLEMENTATION
#define SPONGE_EXAMPLE_INIT_DEFINED
#define SPONGE_EXAMPLE_MOUSE_MOVE_DEFINED
#define SPONGE_EXAMPLE_DRAW_FRAME_3D_DEFINED
#include "example.h"

#define FOV (60.0f / PI * 2.0f)
#define DISTANCE (10.0f)
#define GOLDEN_RATIO (1.618033988749f)

static int32_t mouse_x;
static int32_t mouse_y;

static sponge_Vec3    positions[12];
static sponge_Color32 colors[12];
static sponge_Color32 colors_red[12];
static sponge_Color32 colors_green[12];
static sponge_Color32 colors_blue[12];
static int32_t        triangles[20 * 3];

void make_icosahedron(sponge_Vec3 out_positions[12], int32_t out_triangles[20 * 3]) {
    float a = sqrtf(1.0f / (1.0f + (GOLDEN_RATIO * GOLDEN_RATIO)));
    float b = a * GOLDEN_RATIO;
    out_positions[0]  = sponge_vec3_make(0.0f, -a,  b);
    out_positions[1]  = sponge_vec3_make(0.0f,  a,  b);
    out_positions[2]  = sponge_vec3_make(0.0f, -a, -b);
    out_positions[3]  = sponge_vec3_make(0.0f,  a, -b);

    out_positions[4]  = sponge_vec3_make(-b, 0.0f,  a);
    out_positions[5]  = sponge_vec3_make( b, 0.0f,  a);
    out_positions[6]  = sponge_vec3_make(-b, 0.0f, -a);
    out_positions[7]  = sponge_vec3_make( b, 0.0f, -a);

    out_positions[8]  = sponge_vec3_make(-a,  b, 0.0f);
    out_positions[9]  = sponge_vec3_make( a,  b, 0.0f);
    out_positions[10] = sponge_vec3_make(-a, -b, 0.0f);
    out_positions[11] = sponge_vec3_make( a, -b, 0.0f);

    size_t i = 0;
    // 2x y
    out_triangles[i++] =  4; out_triangles[i++] =  1; out_triangles[i++] =  0;
    out_triangles[i++] =  0; out_triangles[i++] =  1; out_triangles[i++] =  5;
    out_triangles[i++] =  3; out_triangles[i++] =  2; out_triangles[i++] =  7;
    out_triangles[i++] =  2; out_triangles[i++] =  3; out_triangles[i++] =  6;
    // 2y z
    out_triangles[i++] =  4; out_triangles[i++] =  6; out_triangles[i++] =  8;
    out_triangles[i++] = 10; out_triangles[i++] =  6; out_triangles[i++] =  4;
    out_triangles[i++] =  7; out_triangles[i++] =  5; out_triangles[i++] =  9;
    out_triangles[i++] =  5; out_triangles[i++] =  7; out_triangles[i++] = 11;
    // 2z x
    out_triangles[i++] =  8; out_triangles[i++] =  9; out_triangles[i++] =  1;
    out_triangles[i++] =  3; out_triangles[i++] =  9; out_triangles[i++] =  8;
    out_triangles[i++] =  0; out_triangles[i++] = 11; out_triangles[i++] = 10;
    out_triangles[i++] = 10; out_triangles[i++] = 11; out_triangles[i++] =  2;
    // xyz
    out_triangles[i++] = 10; out_triangles[i++] =  4; out_triangles[i++] =  0;
    out_triangles[i++] =  0; out_triangles[i++] =  5; out_triangles[i++] = 11;
    out_triangles[i++] =  1; out_triangles[i++] =  4; out_triangles[i++] =  8;
    out_triangles[i++] =  9; out_triangles[i++] =  5; out_triangles[i++] =  1;
    out_triangles[i++] =  2; out_triangles[i++] =  6; out_triangles[i++] = 10;
    out_triangles[i++] = 11; out_triangles[i++] =  7; out_triangles[i++] =  2;
    out_triangles[i++] =  8; out_triangles[i++] =  6; out_triangles[i++] =  3;
    out_triangles[i++] =  3; out_triangles[i++] =  7; out_triangles[i++] =  9;
}

// NOTE(kard):
// there are many ways to handle opacity and base library doesn't provide one.
// this example implementation requires caller to order objects back to front.
// you could allocate multiple canvases and merge them at the end (after sorting pixel based on depth).
void draw_mesh_col_transparent(
    sponge_Texture c, float *depths,
    sponge_Mat4 model, sponge_Mat4 view, sponge_Mat4 proj,
    sponge_Vec3 *positions, sponge_Color32 *colors, int32_t *triangles, size_t triangles_count,
    float opacity
) {
    sponge_Mat4 mvp = sponge_mat4_mul_mat4(sponge_mat4_mul_mat4(model, view), proj);
    for (size_t i = 0; i < triangles_count; i += 3) {
        sponge_DrawMeshTriangleContext ctx = sponge_draw_mesh_triangle_init(c, mvp, positions, triangles, i);
        sponge_Color32 *row = c.pixels + (ctx.min_pixel.y * c.stride_pixels);
        float *depth_row = depths + (ctx.min_pixel.y * c.stride_pixels);

        for (int32_t y = ctx.min_pixel.y; y <= ctx.max_pixel.y; y++, row += c.stride_pixels, depth_row += c.stride_pixels) {
            for (int32_t x = ctx.min_pixel.x; x <= ctx.max_pixel.x; x++) {
                float w0, w1, w2;
                if (sponge_draw_mesh_triangle_iter(ctx, sponge_vec2i_make(x, y), &depth_row[x], &w0, &w1, &w2)) {
                    sponge_ColorF c0 = sponge_color32_to_colorf(colors[ctx.t0]);
                    sponge_ColorF c1 = sponge_color32_to_colorf(colors[ctx.t1]);
                    sponge_ColorF c2 = sponge_color32_to_colorf(colors[ctx.t2]);
                    sponge_ColorF result = sponge_vec4_make(0.0f, 0.0f, 0.0f, 0.0f);
                    result = sponge_vec4_add(result, sponge_vec4_mul(c0, w0));
                    result = sponge_vec4_add(result, sponge_vec4_mul(c1, w1));
                    result = sponge_vec4_add(result, sponge_vec4_mul(c2, w2));

                    sponge_ColorF cur = sponge_color32_to_colorf(row[x]);
                    result = sponge_vec4_add(cur, sponge_vec4_mul(sponge_vec4_sub(result, cur), opacity));
                    row[x] = sponge_colorf_to_color32(result);
                }
            }
        }
    }
}

void init() {
    make_icosahedron(positions, triangles);
    sponge_Vec3 light = sponge_vec3_make(1.0f, 1.0f, 1.0f);
    float len = (light.x * light.x) + (light.y * light.y) + (light.z * light.z);
    len = sqrtf(len);
    light = sponge_vec3_mul(light, 1.0f / len);
    for (uint32_t i = 0; i < 12; i++) {
        sponge_Vec3 v = positions[i];
        float value = v.x * light.x + v.y * light.y + v.z * light.z;
        value = SPONGE_CLAMP(value, 0.1f, 1.0f);
        sponge_ColorF color = sponge_vec4_make(1.0f, value, value, value);
        color = sponge_vec4_mul(color, 255.0f);
        colors[i] = sponge_colorf_to_color32(color);
        colors_red[i] = sponge_color32_make(0xFFFF0000);
        colors_green[i] = sponge_color32_make(0xFF00FF00);
        colors_blue[i] = sponge_color32_make(0xFF0000FF);

        //sponge_Vec3 v = sponge_vec3_mul(positions[i], 255.0f);
        //colors[i] = sponge_colorf_to_color32(sponge_vec4_make(1.0f, SPONGE_ABS(v.x), SPONGE_ABS(v.y), SPONGE_ABS(v.z)));
    }
}

void mouse_move(int32_t x, int32_t y) {
    mouse_x = x;
    mouse_y = y;
}

void draw_frame_3d(sponge_Texture c, float *depths) {
    sponge_clear_3d(c, depths, sponge_color32_make(0xFF000000));

    sponge_Mat4 model = sponge_mat4_translate(2.0f, 0.0f, 0.0f);
    sponge_Mat4 rotate = sponge_mat4_rotate(0.0f, 90.0f / 360.0f * PI * 2.0f, 0.0f);
    sponge_Mat4 model_red   = sponge_mat4_mul_mat4(model, rotate);
    sponge_Mat4 model_green = sponge_mat4_mul_mat4(model_red, rotate);
    sponge_Mat4 model_blue  = sponge_mat4_mul_mat4(model_green, rotate);

    float rot_y = (float)mouse_x / (float)c.width * PI * 2.0f;
    float rot_x = ((float)mouse_y / (float)c.height * PI) - (PI * 0.5f);
    sponge_Mat4 view = sponge_mat4_identity();
    view = sponge_mat4_mul_mat4(view, sponge_mat4_rotate(rot_x, rot_y, 0.0f));
    view = sponge_mat4_mul_mat4(view, sponge_mat4_translate(0.0f, 0.0f, -10.0f));

    sponge_Mat4 proj = sponge_mat4_projection(FOV, (float)c.width / (float)c.height, 1.0f, 100.0f);
    sponge_draw_mesh_col(c, depths, model, view, proj, positions, colors, triangles, 60);

    float z0 = sponge_vec3_mul_mat4(sponge_vec3_make(0.0f, 0.0f, 0.0f), sponge_mat4_mul_mat4(model_red,   view)).z;
    float z1 = sponge_vec3_mul_mat4(sponge_vec3_make(0.0f, 0.0f, 0.0f), sponge_mat4_mul_mat4(model_green, view)).z;
    float z2 = sponge_vec3_mul_mat4(sponge_vec3_make(0.0f, 0.0f, 0.0f), sponge_mat4_mul_mat4(model_blue,  view)).z;
    float z[3] = { z0, z1, z2 };
    int order[3] = { 0, 1, 2 };

    // do not look at this garbage
    if (z[order[0]] > z[order[1]]) {
        int tmp = order[0];
        order[0] = order[1];
        order[1] = tmp;
    }
    if (z[order[0]] > z[order[2]]) {
        int tmp = order[0];
        order[0] = order[2];
        order[2] = tmp;
    }
    if (z[order[1]] > z[order[2]]) {
        int tmp = order[1];
        order[1] = order[2];
        order[2] = tmp;
    }

    for (int i = 0; i < 3; i++) {
        switch(order[i]) {
            case 0:
                draw_mesh_col_transparent(c, depths, model_red,   view, proj, positions, colors_red,   triangles, 60, 0.1f);
                break;
            case 1:
                draw_mesh_col_transparent(c, depths, model_green, view, proj, positions, colors_green, triangles, 60, 0.2f);
                break;
            case 2:
                draw_mesh_col_transparent(c, depths, model_blue,  view, proj, positions, colors_blue,  triangles, 60, 0.3f);
                break;
        }
    }
}
