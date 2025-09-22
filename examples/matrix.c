#include "../sponge.h"
#define SPONGE_EXAMPLE_IMPLEMENTATION
#include "example.h"

#define SPEED ((1.0f / 360.0f) * 2 * PI)
static float angle = 0.0f;

void draw_frame(sponge_Texture c) {
    sponge_clear(c, sponge_color32_make(0xFF000000));
    angle += SPEED;
    sponge_Vec3 v0 = sponge_vec3_make(0.0f, -0.8f, 0.0f);
    sponge_Vec3 v1 = sponge_vec3_make(0.8f, 0.8f, 0.0f);
    sponge_Vec3 v2 = sponge_vec3_make(-0.8f, 0.8f, 0.0f);

    sponge_Mat4 m = sponge_mat4_rotate(0.0f, 0.0f, angle);
    m = sponge_mat4_mul_mat4(m, sponge_mat4_translate(1.0f, 1.0f, 0.0f));
    m = sponge_mat4_mul_mat4(m, sponge_mat4_scale((float)(c.width / 2), (float)(c.height / 2), 1.0f));

    v0 = sponge_vec3_mul_mat4(v0, m);
    v1 = sponge_vec3_mul_mat4(v1, m);
    v2 = sponge_vec3_mul_mat4(v2, m);

    sponge_draw_triangle_col3(
        c,
        sponge_vec2i_make((int32_t)v0.x, (int32_t)v0.y),
        sponge_vec2i_make((int32_t)v1.x, (int32_t)v1.y),
        sponge_vec2i_make((int32_t)v2.x, (int32_t)v2.y),
        sponge_colorf_make(0xFFFF0000), sponge_colorf_make(0xFF00FF00), sponge_colorf_make(0xFF0000FF));
}
