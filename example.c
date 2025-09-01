#include <assert.h>

#define SPONGE_IMPLEMENTATION
#include "sponge.h"

void draw_frame(sponge_Canvas c) {
    assert(c.width > 140 + 20);
    assert(c.height > 70 + 40);
    sponge_clear(c, 0xFF000000);
    sponge_draw_rect(c, 80, 70, 80 + 20, 70 + 40, 0xFFFF00FF);
    sponge_draw_rect(c, 140, 70, 140 + 20, 70 + 40, 0xFFFF00FF);
}
