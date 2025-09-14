#include <assert.h>

#include "../sponge.h"

void init() {}

void draw_frame(sponge_Texture c) {
    assert(c.width > 140 + 20);
    assert(c.height > 70 + 40);
    sponge_clear(c, sponge_color32_make(0xFF000000));
    sponge_draw_rect(c, sponge_vec2i_make(80, 70),  sponge_vec2i_make(80 + 20, 70 + 40),  sponge_color32_make(0xFFFF00FF));
    sponge_draw_rect(c, sponge_vec2i_make(140, 70), sponge_vec2i_make(140 + 20, 70 + 40), sponge_color32_make(0xFFFF00FF));
}
