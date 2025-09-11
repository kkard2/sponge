#include <stdint.h>

typedef struct {
    uint32_t *pixels; // AARRGGBB
    uint32_t width;
    uint32_t height;
    uint32_t stride;
} sponge_Texture;

typedef struct {
    float x;
    float y;
} sponge_Vec2;

sponge_Vec2 sponge_add2(sponge_Vec2 v0, sponge_Vec2 v1);
sponge_Vec2 sponge_sub2(sponge_Vec2 v0, sponge_Vec2 v1);
float       sponge_dot2(sponge_Vec2 v0, sponge_Vec2 v1);

int  sponge_canvas_valid      (sponge_Texture c);
void sponge_clear             (sponge_Texture c, uint32_t color);
void sponge_draw_rect         (sponge_Texture c, uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, uint32_t color);
void sponge_draw_line         (sponge_Texture c,  int32_t x0,  int32_t y0,  int32_t x1,  int32_t y1, uint32_t color);
void sponge_draw_texture      (sponge_Texture c, uint32_t x0, uint32_t y0, sponge_Texture tex);
void sponge_draw_triangle_col (sponge_Texture c,  int32_t x0,  int32_t y0,  int32_t x1,  int32_t y1,  int32_t x2,  int32_t y2, uint32_t color);
void sponge_draw_triangle_col3(
    sponge_Texture c, int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2,
    uint32_t color0, uint32_t color1, uint32_t color2);

// TODO(kard): prefix stripping

#ifdef SPONGE_IMPLEMENTATION

// can be defined as nop in release builds
#ifndef SPONGE_ASSERT
#include <assert.h>
#define SPONGE_ASSERT(x) assert(x)
#endif // SPONGE_ASSERT

#define SPONGE__CLAMP(val, min, max) ((val) < (min) ? (min) : ((val) > (max) ? (max) : (val)))
#define SPONGE__MIN(x, y) ((x) < (y) ? (x) : (y))
#define SPONGE__MAX(x, y) ((x) > (y) ? (x) : (y))
#define SPONGE__ABS(x) ((x) >= 0 ? (x) : -(x))

typedef struct {
    float a;
    float r;
    float g;
    float b;
} sponge__ColorF;

sponge__ColorF sponge__colf_unpack(uint32_t col) {
    uint32_t b = col & 0xFF;
    col = col >> 8;
    uint32_t g = col & 0xFF;
    col = col >> 8;
    uint32_t r = col & 0xFF;
    col = col >> 8;
    uint32_t a = col & 0xFF;
    sponge__ColorF result = { .a = (float)a, .r = (float)r, .g = (float)g, .b = (float)b };
    return result;
}

uint32_t sponge__colf_pack(sponge__ColorF color) {
    uint32_t result = 0;
    result |= SPONGE__CLAMP((uint32_t)color.a, 0x00, 0xFF);
    result = result << 8;
    result |= SPONGE__CLAMP((uint32_t)color.r, 0x00, 0xFF);
    result = result << 8;
    result |= SPONGE__CLAMP((uint32_t)color.g, 0x00, 0xFF);
    result = result << 8;
    result |= SPONGE__CLAMP((uint32_t)color.b, 0x00, 0xFF);
    return result;
}

sponge__ColorF sponge__colf_mul(sponge__ColorF color, float f) {
    color.a *= f;
    color.r *= f;
    color.g *= f;
    color.b *= f;
    return color;
}

sponge__ColorF sponge__colf_add(sponge__ColorF color0, sponge__ColorF color1) {
    sponge__ColorF result;
    result.a = color0.a + color1.a;
    result.r = color0.r + color1.r;
    result.g = color0.g + color1.g;
    result.b = color0.b + color1.b;
    return result;
}


typedef struct {
    sponge_Vec2 v0;
    sponge_Vec2 v1;
    float d00;
    float d01;
    float d11;
    float denom;
} sponge__BarycentricContext;

// TODO(kard): measure if caching this makes sense
sponge__BarycentricContext sponge__barycentric_init(sponge_Vec2 t0, sponge_Vec2 t1, sponge_Vec2 t2) {
    sponge__BarycentricContext result;
    result.v0 = sponge_sub2(t1, t0);
    result.v1 = sponge_sub2(t2, t0);
    result.d00 = sponge_dot2(result.v0, result.v0);
    result.d01 = sponge_dot2(result.v0, result.v1);
    result.d11 = sponge_dot2(result.v1, result.v1);
    result.denom = (result.d00 * result.d11) - (result.d01 * result.d01);
    return result;
}

void sponge__barycentric(
    sponge__BarycentricContext ctx, sponge_Vec2 p,
    sponge_Vec2 t0,
    sponge_Vec2 t1,
    sponge_Vec2 t2,
    float *u, float *v, float *w
) {
    sponge_Vec2 v2 = sponge_sub2(p, t0);
    float d20 = sponge_dot2(v2, ctx.v0);
    float d21 = sponge_dot2(v2, ctx.v1);
    float vr = (ctx.d11 * d20 - ctx.d01 * d21) / ctx.denom;
    float wr = (ctx.d00 * d21 - ctx.d01 * d20) / ctx.denom;
    float ur = 1.0f - vr - wr;
    *u = ur;
    *v = vr;
    *w = wr;
}


sponge_Vec2 sponge_add2(sponge_Vec2 v0, sponge_Vec2 v1) {
    sponge_Vec2 result;
    result.x = v0.x + v1.x;
    result.y = v0.y + v1.y;
    return result;
}

sponge_Vec2 sponge_sub2(sponge_Vec2 v0, sponge_Vec2 v1) {
    sponge_Vec2 result;
    result.x = v0.x - v1.x;
    result.y = v0.y - v1.y;
    return result;
}

float sponge_dot2(sponge_Vec2 v0, sponge_Vec2 v1) {
    return (v0.x * v1.x) + (v0.y * v1.y);
}

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

void sponge_draw_line(sponge_Texture c, int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint32_t color) {
    SPONGE_ASSERT(sponge_canvas_valid(c));

    int32_t dx = x1 - x0;
    dx = SPONGE__ABS(dx);
    int32_t dy = y1 - y0;
    dy = SPONGE__ABS(dy);
    int32_t d = (2 * dy) - dx;

    int32_t sx = (x0 < x1) ? 1 : -1;
    int32_t sy = (y0 < y1) ? 1 : -1;
    int32_t err = dx - dy;

    while (1) {
        if (x0 >= 0 && x0 < (int32_t)c.width && y0 >= 0 && y0 < (int32_t)c.height)
            c.pixels[(y0 * c.stride) + x0] = color;

        if (x0 == x1 && y0 == y1) break;

        int32_t e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x0 += sx; }
        if (e2 < dx)  { err += dx; y0 += sy; }
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

void sponge_draw_triangle_col(sponge_Texture c, int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t color) {
    sponge_draw_triangle_col3(c, x0, y0, x1, y1, x2, y2, color, color, color);
}

// NOTE(kard):
// this does not accept floats on purpose.
// there are more efficient algorithms for producing triangle pixels other than
// checking every one of them in rectangle with barycentric coords (e.g. using Brehensam algorithm)
// and they can work with integer coordinates.
// TODO(kard): this should be explored in the future
// TODO(kard): backface culling?
void sponge_draw_triangle_col3(
    sponge_Texture c, int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2,
    uint32_t color0, uint32_t color1, uint32_t color2) {
    SPONGE_ASSERT(sponge_canvas_valid(c));

    sponge_Vec2 t0 = { .x = (float)x0, .y = (float)y0 };
    sponge_Vec2 t1 = { .x = (float)x1, .y = (float)y1 };
    sponge_Vec2 t2 = { .x = (float)x2, .y = (float)y2 };

    int32_t smin_x = SPONGE__MIN(x0, SPONGE__MIN(x1, x2));
    int32_t smax_x = SPONGE__MAX(x0, SPONGE__MAX(x1, x2));
    int32_t smin_y = SPONGE__MIN(y0, SPONGE__MIN(y1, y2));
    int32_t smax_y = SPONGE__MAX(y0, SPONGE__MAX(y1, y2));

    uint32_t min_x = (uint32_t)SPONGE__CLAMP(smin_x, 0, (int32_t)(c.width - 1));
    uint32_t max_x = (uint32_t)SPONGE__CLAMP(smax_x, 0, (int32_t)(c.width - 1));
    uint32_t min_y = (uint32_t)SPONGE__CLAMP(smin_y, 0, (int32_t)(c.height - 1));
    uint32_t max_y = (uint32_t)SPONGE__CLAMP(smax_y, 0, (int32_t)(c.height - 1));

    sponge__BarycentricContext ctx = sponge__barycentric_init(t0, t1, t2);

    uint32_t *row = c.pixels + (min_y * c.stride);

    sponge__ColorF color0f = sponge__colf_unpack(color0);
    sponge__ColorF color1f = sponge__colf_unpack(color1);
    sponge__ColorF color2f = sponge__colf_unpack(color2);

    for (uint32_t y = min_y; y <= max_y; y++, row += c.stride) {
        for (uint32_t x = x0; x <= max_x; x++) {
            float u, v, w;
            sponge_Vec2 p = { .x = (float)x, .y = (float)y };
            sponge__barycentric(ctx, p, t0, t1, t2, &u, &v, &w);
            if (u > 0.0f && v > 0.0f && w > 0.0f)
            {
                // TODO(kard): make more robust, this probably has a lot of off by 1 errors
                sponge__ColorF c0 = sponge__colf_mul(color0f, u);
                sponge__ColorF c1 = sponge__colf_mul(color1f, v);
                sponge__ColorF c2 = sponge__colf_mul(color2f, w);
                sponge__ColorF result = sponge__colf_add(c0, sponge__colf_add(c1, c2));
                row[x] = sponge__colf_pack(result);
            }
        }
    }
}

#endif // SPONGE_IMPLEMENTATION

