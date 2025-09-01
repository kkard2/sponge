#include <stdint.h>

typedef struct {
    // TODO(kard): define byte order
    uint32_t *pixels;
    uint32_t width;
    uint32_t height;
    uint32_t stride;
} sponge_Canvas;

void sponge_clear(sponge_Canvas c, uint32_t color);
void sponge_draw_rect(sponge_Canvas c, uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, uint32_t color);

// TODO(kard): prefix stripping

#ifdef SPONGE_IMPLEMENTATION

void sponge_clear(sponge_Canvas c, uint32_t color) {
    sponge_draw_rect(c, 0, 0, c.width - 1, c.height - 1, color);
}

// TODO(kard): probably bounds checking
// TODO(kard): alpha blending maybe
void sponge_draw_rect(sponge_Canvas c, uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, uint32_t color) {
    uint32_t *row = c.pixels + (y0 * c.stride);

    for (uint32_t y = y0; y <= y1; y++, row += c.stride) {
        for (uint32_t x = x0; x <= x1; x++) {
            row[x] = color;
        }
    }
}

#endif // SPONGE_IMPLEMENTATION

