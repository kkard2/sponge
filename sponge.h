#include <stdint.h>

typedef struct {
    uint32_t *pixels; // AARRGGBB
    uint32_t width;
    uint32_t height;
    uint32_t stride;
} sponge_Texture;

int  sponge_canvas_valid(sponge_Texture c);
void sponge_clear       (sponge_Texture c, uint32_t color);
void sponge_draw_rect   (sponge_Texture c, uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, uint32_t color);
void sponge_draw_texture(sponge_Texture c, uint32_t x0, uint32_t y0, sponge_Texture tex);

// TODO(kard): prefix stripping

#ifdef SPONGE_IMPLEMENTATION

// can be defined as nop in release builds
#ifndef SPONGE_ASSERT
#include <assert.h>
#define SPONGE_ASSERT(x) assert(x)
#endif // SPONGE_ASSERT

int sponge_canvas_valid(sponge_Texture c) {
    return c.width != 0 && c.height != 0;
}

void sponge_clear(sponge_Texture c, uint32_t color) {
    SPONGE_ASSERT(sponge_canvas_valid(c));

    sponge_draw_rect(c, 0, 0, c.width - 1, c.height - 1, color);
}

// TODO(kard): probably bounds checking
// TODO(kard): alpha blending maybe
void sponge_draw_rect(sponge_Texture c, uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, uint32_t color) {
    SPONGE_ASSERT(sponge_canvas_valid(c));

    uint32_t *row = c.pixels + (y0 * c.stride);

    for (uint32_t y = y0; y <= y1; y++, row += c.stride) {
        for (uint32_t x = x0; x <= x1; x++) {
            row[x] = color;
        }
    }
}

void sponge_draw_texture(sponge_Texture c, uint32_t x0, uint32_t y0, sponge_Texture tex) {
    SPONGE_ASSERT(sponge_canvas_valid(c));

    uint32_t *row_dst = c.pixels + (y0 * c.stride);
    uint32_t *row_src = tex.pixels;
    uint32_t x1 = x0 + tex.width;
    uint32_t y1 = y0 + tex.height;

    if (x1 >= c.width)
        x1 = c.width - 1;
    if (y1 >= c.height)
        y1 = c.height - 1;

    for (uint32_t y = y0; y <= y1; y++, row_src += tex.stride, row_dst += c.stride) {
        // TODO(kard): memcpy?
        for (uint32_t x = x0, x_src = 0; x <= x1; x++, x_src++) {
            row_dst[x] = row_src[x_src];
        }
    }
}

#endif // SPONGE_IMPLEMENTATION

