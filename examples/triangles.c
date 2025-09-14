#include "../sponge.h"

#define SPEED_ABS (2)
#define GRID_SIZE (10)


static int32_t offset = 0;
static int32_t speed = SPEED_ABS;

void init() {}

void draw_frame(sponge_Texture c) {
    sponge_clear(c, sponge_color32_make(0xFF000000));
    offset += speed;

    if (offset > 30) {
        speed = -SPEED_ABS;
    } else if (offset < -30) {
        speed = SPEED_ABS;
    }

    int32_t offset_x = c.width / GRID_SIZE;
    int32_t offset_y = c.height / GRID_SIZE;

    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            int32_t x = i * offset_x;
            int32_t y = j * offset_y + offset;

        sponge_draw_triangle_col3(
            c,
            sponge_vec2i_make(x, y + offset_y),
            sponge_vec2i_make(x + offset_x, y),
            sponge_vec2i_make(x + (offset_x * 3 / 4), y + (offset_y * 3 / 4)),
            sponge_colorf_make(0xFFFF0000), sponge_colorf_make(0xFF00FF00), sponge_colorf_make(0xFF0000FF));
        }
    }
}
