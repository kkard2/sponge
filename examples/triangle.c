#include "../sponge.h"
#define SPONGE_EXAMPLE_IMPLEMENTATION
#include "example.h"

#define SPEED (4)
static int32_t offset = 0;

void draw_frame(sponge_Texture c) {
    sponge_clear(c, sponge_color32_make(0xFF000000));
    offset += SPEED;

    if (offset > 100)
        offset = -100;

    sponge_draw_triangle_col3(
        c,
        sponge_vec2i_make(30, 30 + offset),
        sponge_vec2i_make(c.width - 30, 50 + offset),
        sponge_vec2i_make(30, c.height - 30 + offset),
        sponge_colorf_make(0xFFFF0000), sponge_colorf_make(0xFF00FF00), sponge_colorf_make(0xFF0000FF));
}
