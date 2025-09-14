#include <stdint.h>

typedef union {
    // do not rely on byte order of this field (unless you only support one platform)
    uint32_t value;
    struct {
        uint8_t b;
        uint8_t g;
        uint8_t r;
        uint8_t a;
    };
} sponge_Color32;
sponge_Color32 sponge_color32_make(uint32_t value);

typedef struct {
    sponge_Color32 *pixels;
    uint32_t        width;
    uint32_t        height;
    size_t          stride_pixels;
} sponge_Texture;

typedef struct {
    float x;
    float y;
} sponge_Vec2;
sponge_Vec2 sponge_vec2_make(float x, float y);

typedef struct {
    float x;
    float y;
    float z;
} sponge_Vec3;
sponge_Vec3 sponge_vec3_make(float x, float y, float z);

typedef union {
    struct {
        float x;
        float y;
        float z;
        float w;
    };
    struct {
        float a;
        float r;
        float g;
        float b;
    };
} sponge_Vec4;
sponge_Vec4 sponge_vec4_make(float x, float y, float z, float w);

// in sponge apis components expected to be between 0.0 and 255.0
#define sponge_ColorF sponge_Vec4

typedef struct {
    int32_t x;
    int32_t y;
} sponge_Vec2I;
sponge_Vec2I sponge_vec2i_make(int32_t x, int32_t y);

sponge_Vec2 sponge_vec2_add(sponge_Vec2 v0, sponge_Vec2 v1);
sponge_Vec2 sponge_vec2_sub(sponge_Vec2 v0, sponge_Vec2 v1);
float       sponge_vec2_dot(sponge_Vec2 v0, sponge_Vec2 v1);

sponge_Vec4 sponge_vec4_add(sponge_Vec4 v0, sponge_Vec4 v1);
sponge_Vec4 sponge_vec4_mul(sponge_Vec4 v0, float f);

sponge_ColorF  sponge_color32_to_colorf(sponge_Color32 col);
sponge_Color32 sponge_colorf_to_color32(sponge_ColorF  col);

// checks if tex.width or tex.height is equal to 0
// calling sponge functions with invalid textures is not supported
int sponge_texture_valid(sponge_Texture tex);

// first argument is always rendering target (canvas)
void sponge_clear       (sponge_Texture c, sponge_Color32 col);

void sponge_draw_rect   (sponge_Texture c, sponge_Vec2I v0, sponge_Vec2I v1, sponge_Color32 col);
void sponge_draw_line   (sponge_Texture c, sponge_Vec2I v0, sponge_Vec2I v1, sponge_Color32 col);
void sponge_draw_texture(sponge_Texture c, sponge_Vec2I offset, sponge_Texture tex);

//void sponge_draw_triangle_col (sponge_Texture c, sponge_Vec2I v0, sponge_Vec2I v1, sponge_Vec2I v2, sponge_Color32 col);
//void sponge_draw_triangle_col3(sponge_Texture c,
//    sponge_Vec2I  v0,   sponge_Vec2I  v1,   sponge_Vec2I  v2,
//    sponge_ColorF col0, sponge_ColorF col1, sponge_ColorF col2);

#define SPONGE_CLAMP(val, min, max) ((val) < (min) ? (min) : ((val) > (max) ? (max) : (val)))
#define SPONGE_MIN(x, y) ((x) < (y) ? (x) : (y))
#define SPONGE_MAX(x, y) ((x) > (y) ? (x) : (y))
#define SPONGE_ABS(x) ((x) >= 0 ? (x) : -(x))

// TODO(kard): prefix stripping

#ifdef SPONGE_IMPLEMENTATION

// can be defined as nop in release builds
#ifndef SPONGE_ASSERT
#include <assert.h>
#define SPONGE_ASSERT(x) assert(x)
#endif // SPONGE_ASSERT

// TODO(kard): msvc does not inline this for some reason, explore if it's a concern
sponge_Color32 sponge_color32_make(uint32_t value) {
    sponge_Color32 result;
    result.a = (value & 0xFF000000) >> 24;
    result.r = (value & 0x00FF0000) >> 16;
    result.g = (value & 0x0000FF00) >> 8;
    result.b = (value & 0x000000FF);
    return result;
}
sponge_Vec2 sponge_vec2_make(float x, float y) {
    sponge_Vec2 result;
    result.x = x;
    result.y = y;
    return result;
}
sponge_Vec3 sponge_vec3_make(float x, float y, float z) {
    sponge_Vec3 result;
    result.x = x;
    result.y = y;
    result.z = z;
    return result;
}
sponge_Vec4 sponge_vec4_make(float x, float y, float z, float w) {
    sponge_Vec4 result;
    result.x = x;
    result.y = y;
    result.z = z;
    result.w = w;
    return result;
}
sponge_Vec2I sponge_vec2i_make(int32_t x, int32_t y) {
    sponge_Vec2I result;
    result.x = x;
    result.y = y;
    return result;
}

sponge_Vec2 sponge_vec2_add(sponge_Vec2 v0, sponge_Vec2 v1) {
    return (sponge_Vec2){ .x = v0.x + v1.x, .y = v0.y + v1.y };
}
sponge_Vec2 sponge_vec2_sub(sponge_Vec2 v0, sponge_Vec2 v1) {
    return (sponge_Vec2){ .x = v0.x - v1.x, .y = v0.y - v1.y };
}
float       sponge_vec2_dot(sponge_Vec2 v0, sponge_Vec2 v1) {
    return (v0.x * v1.x) + (v0.y * v1.y);
}

sponge_ColorF  sponge_color32_to_colorf(sponge_Color32 col) {
    return (sponge_ColorF){ .a = (float)col.a, .r = (float)col.r, .g = (float)col.g, .b = (float)col.b };
}
sponge_Color32 sponge_colorf_to_color32(sponge_ColorF  col) {
    return (sponge_Color32){ .a = (uint8_t)col.a, .r = (uint8_t)col.r, .g = (uint8_t)col.g, .b = (uint8_t)col.b };
}

sponge_Vec4 sponge_vec4_add(sponge_Vec4 v0, sponge_Vec4 v1) {
    v0.a += v1.a;
    v0.r += v1.r;
    v0.g += v1.g;
    v0.b += v1.b;
    return v0;
}
sponge_Vec4 sponge_vec4_mul(sponge_Vec4 v0, float f) {
    v0.a *= f;
    v0.r *= f;
    v0.g *= f;
    v0.b *= f;
    return v0;
}

// sponge__BarycentricContext sponge__barycentric_init(sponge_Vec2 t0, sponge_Vec2 t1, sponge_Vec2 t2) {
    // sponge__BarycentricContext result;
    // result.v0 = sponge_sub2(t1, t0);
    // result.v1 = sponge_sub2(t2, t0);
    // result.d00 = sponge_dot2(result.v0, result.v0);
    // result.d01 = sponge_dot2(result.v0, result.v1);
    // result.d11 = sponge_dot2(result.v1, result.v1);
    // result.denom = (result.d00 * result.d11) - (result.d01 * result.d01);
    // return result;
// }
// 
// void sponge__barycentric(
    // sponge__BarycentricContext ctx, sponge_Vec2 p,
    // sponge_Vec2 t0,
    // sponge_Vec2 t1,
    // sponge_Vec2 t2,
    // float *u, float *v, float *w
// ) {
    // sponge_Vec2 v2 = sponge_sub2(p, t0);
    // float d20 = sponge_dot2(v2, ctx.v0);
    // float d21 = sponge_dot2(v2, ctx.v1);
    // float vr = (ctx.d11 * d20 - ctx.d01 * d21) / ctx.denom;
    // float wr = (ctx.d00 * d21 - ctx.d01 * d20) / ctx.denom;
    // float ur = 1.0f - vr - wr;
    // *u = ur;
    // *v = vr;
    // *w = wr;
// }


int sponge_texture_valid(sponge_Texture tex) {
    return tex.width != 0 && tex.height != 0 && tex.stride_pixels != 0;
}

void sponge_clear(sponge_Texture c, sponge_Color32 col) {
    SPONGE_ASSERT(sponge_texture_valid(c));
    sponge_Color32 *row = c.pixels;

    for (uint32_t y = 0; y < c.height; y++, row += c.stride_pixels) {
        for (uint32_t x = 0; x < c.width; x++) {
            row[x] = col;
        }
    }
}

void sponge_draw_rect(sponge_Texture c, sponge_Vec2I v0, sponge_Vec2I v1, sponge_Color32 col) {
    SPONGE_ASSERT(sponge_texture_valid(c));
    sponge_Vec2I min, max;
    min.x = SPONGE_MIN(v0.x, v1.x);
    min.y = SPONGE_MIN(v0.y, v1.y);
    max.x = SPONGE_MAX(v0.x, v1.x);
    max.y = SPONGE_MAX(v0.y, v1.y);
    if (min.x >= (int32_t)c.width || min.y >= (int32_t)c.height || max.x < 0 || max.y < 0)
        return;
    min.x = SPONGE_MAX(min.x, 0);
    min.y = SPONGE_MAX(min.y, 0);
    max.x = SPONGE_MIN(max.x, (int32_t)c.width - 1);
    max.y = SPONGE_MIN(max.y, (int32_t)c.height - 1);

    sponge_Color32 *row = c.pixels + (min.y * c.stride_pixels);

    for (int32_t y = min.y; y <= max.y; y++, row += c.stride_pixels) {
        for (int32_t x = min.x; x <= max.x; x++) {
            row[x] = col;
        }
    }
}

void sponge_draw_line(sponge_Texture c, sponge_Vec2I v0, sponge_Vec2I v1, sponge_Color32 col) {
    SPONGE_ASSERT(sponge_texture_valid(c));

    int32_t dx = SPONGE_ABS(v1.x - v0.x);
    int32_t dy = SPONGE_ABS(v1.y - v0.y);
    int32_t d = (2 * dy) - dx;

    int32_t sx = (v0.x < v1.x) ? 1 : -1;
    int32_t sy = (v0.y < v1.y) ? 1 : -1;
    int32_t err = dx - dy;

    while (1) {
        if (v0.x >= 0 && v0.x < (int32_t)c.width && v0.y >= 0 && v0.y < (int32_t)c.height)
            c.pixels[(v0.y * c.stride_pixels) + v0.x] = col;

        if (v0.x == v1.x && v0.y == v1.y) break;

        int32_t e2 = 2 * err;
        if (e2 > -dy) { err -= dy; v0.x += sx; }
        if (e2 < dx)  { err += dx; v0.y += sy; }
    }
}

void sponge_draw_texture(sponge_Texture c, sponge_Vec2I offset, sponge_Texture tex) {
    SPONGE_ASSERT(sponge_texture_valid(c));
    SPONGE_ASSERT(sponge_texture_valid(tex));

    sponge_Vec2I min, max;
    min.x = SPONGE_MAX(offset.x, 0);
    min.y = SPONGE_MAX(offset.y, 0);
    max.x = SPONGE_MIN(offset.x + tex.width - 1, c.width - 1);
    max.y = SPONGE_MIN(offset.y + tex.height - 1, c.height - 1);
    sponge_Color32 *row_dst = c.pixels + (min.y * c.stride_pixels);
    sponge_Color32 *row_src = (tex.pixels - (offset.y * c.stride_pixels)) + (min.y * c.stride_pixels);
    uint32_t src_offset = min.x - offset.x;

    for (int32_t y = min.y; y <= max.y; y++, row_src += tex.stride_pixels, row_dst += c.stride_pixels) {
        for (int32_t x = min.x, x_src = src_offset; x <= max.x; x++, x_src++) {
            row_dst[x] = row_src[x_src];
        }
    }
}

/*
void sponge_draw_triangle_col(sponge_Texture c, int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t color) {
    sponge_draw_triangle_col3(c, x0, y0, x1, y1, x2, y2, color, color, color);
}

// NOTE(kard):
// this does not accept floats on purpose.
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


    uint32_t *row = c.pixels + (min_y * c.stride);

    sponge__ColorF color0f = sponge__colf_unpack(color0);
    sponge__ColorF color1f = sponge__colf_unpack(color1);
    sponge__ColorF color2f = sponge__colf_unpack(color2);

    for (uint32_t y = min_y; y <= max_y; y++, row += c.stride) {
        for (uint32_t x = x0; x <= max_x; x++) {
            float u, v, w;
            sponge_Vec2 p = { .x = (float)x, .y = (float)y };
            sponge__BarycentricContext ctx = sponge__barycentric_init(t0, t1, t2);
            sponge__barycentric(ctx, p, t0, t1, t2, &u, &v, &w);
            if (u > 0.0f && v > 0.0f && w > 0.0f)
            {
                sponge__ColorF c0 = sponge__colf_mul(color0f, u);
                sponge__ColorF c1 = sponge__colf_mul(color1f, v);
                sponge__ColorF c2 = sponge__colf_mul(color2f, w);
                sponge__ColorF result = sponge__colf_add(c0, sponge__colf_add(c1, c2));
                row[x] = sponge__colf_pack(result);
            }
        }
    }
}

void sponge_draw_triangle_uv(
    sponge_Texture c, int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2,
    uint32_t color0, uint32_t color1, uint32_t color2,
    sponge_Vec2 uv0, sponge_Vec2 uv1, sponge_Vec2 uv2, sponge_Texture tex) {
}
*/

#endif // SPONGE_IMPLEMENTATION
