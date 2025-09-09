#include "../sponge.h"

#define SPEED (4)
static int32_t offset = 0;

void init() {}

void draw_frame(sponge_Texture c) {
    sponge_clear(c, 0xFF000000);
    offset += SPEED;

    if (offset > 100)
        offset = -100;

    sponge_draw_triangle_col3(
        c,
        30, 30 + offset,
        c.width - 30, 50 + offset,
        30, c.height - 30 + offset,
        0xFFFF0000, 0xFF00FF00, 0xFF0000FF);
}
