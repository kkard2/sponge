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
sponge_ColorF sponge_colorf_make(uint32_t value);

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
int32_t sponge_texture_valid(sponge_Texture tex);

// first argument is always rendering target (canvas)
void sponge_clear       (sponge_Texture c, sponge_Color32 col);

void sponge_draw_rect   (sponge_Texture c, sponge_Vec2I v0, sponge_Vec2I v1, sponge_Color32 col);
void sponge_draw_line   (sponge_Texture c, sponge_Vec2I v0, sponge_Vec2I v1, sponge_Color32 col);
void sponge_draw_texture(sponge_Texture c, sponge_Vec2I offset, sponge_Texture tex);

// tests on which side of the line p is (negative on the left, positive on the right)
int32_t sponge_edge_function(sponge_Vec2I p, sponge_Vec2I v0, sponge_Vec2I v1);

// out_area2 is optional
void    sponge_draw_triangle_init(
    sponge_Texture c, sponge_Vec2I v0, sponge_Vec2I v1, sponge_Vec2I v2,
    sponge_Vec2I *out_min, sponge_Vec2I *out_max, float *out_area2);

// non-zero if p inside triangle, outputs only valid if inside
int32_t sponge_draw_triangle_iter(
    sponge_Vec2I p, sponge_Vec2I v0, sponge_Vec2I v1, sponge_Vec2I v2, float area2,
    float *w0, float *w1, float *w2);

void sponge_draw_triangle_col3(
    sponge_Texture c,
    sponge_Vec2I  v0,   sponge_Vec2I  v1,   sponge_Vec2I  v2,
    sponge_ColorF col0, sponge_ColorF col1, sponge_ColorF col2);

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
sponge_ColorF sponge_colorf_make(uint32_t value) {
    sponge_ColorF result;
    sponge_Color32 c32 = sponge_color32_make(value);
    result.a = (float)c32.a;
    result.r = (float)c32.r;
    result.g = (float)c32.g;
    result.b = (float)c32.b;
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


int32_t sponge_texture_valid(sponge_Texture tex) {
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

int32_t sponge_edge_function(sponge_Vec2I v0, sponge_Vec2I v1, sponge_Vec2I p) {
    return ((p.x - v0.x) * (v1.y - v0.y)) - ((p.y - v0.y) * (v1.x - v0.x));
}

void    sponge_draw_triangle_init(
    sponge_Texture c, sponge_Vec2I v0, sponge_Vec2I v1, sponge_Vec2I v2,
    sponge_Vec2I *out_min, sponge_Vec2I *out_max, float *out_area2
) {
    *out_min = sponge_vec2i_make(
        SPONGE_MAX(0, SPONGE_MIN(SPONGE_MIN(v0.x, v1.x), v2.x)),
        SPONGE_MAX(0, SPONGE_MIN(SPONGE_MIN(v0.y, v1.y), v2.y)));
    *out_max = sponge_vec2i_make(
        SPONGE_MIN((int32_t)c.width - 1,  SPONGE_MAX(SPONGE_MAX(v0.x, v1.x), v2.x)),
        SPONGE_MIN((int32_t)c.height - 1, SPONGE_MAX(SPONGE_MAX(v0.y, v1.y), v2.y)));
    if (out_area2) {
        *out_area2 = (float)sponge_edge_function(v0, v1, v2);
    }
}

// non-zero if p inside triangle, outputs only valid if inside
int32_t sponge_draw_triangle_iter(
    sponge_Vec2I p, sponge_Vec2I v0, sponge_Vec2I v1, sponge_Vec2I v2, float area2,
    float *out_w0, float *out_w1, float *out_w2
) {
    int32_t w0 = sponge_edge_function(v1, v2, p);
    int32_t w1 = sponge_edge_function(v2, v0, p);
    int32_t w2 = sponge_edge_function(v0, v1, p);
    // NOTE(kard): this is <= not >= because our y goes down not up, and we want clockwise winding
    if (w0 <= 0 && w1 <= 0 && w2 <= 0) {
        *out_w0 = (float)w0 / area2;
        *out_w1 = (float)w1 / area2;
        *out_w2 = (float)w2 / area2;
        return 1;
    }
    return 0;
}

void sponge_draw_triangle_col3(
    sponge_Texture c,
    sponge_Vec2I  v0,   sponge_Vec2I  v1,   sponge_Vec2I  v2,
    sponge_ColorF col0, sponge_ColorF col1, sponge_ColorF col2
) {
    sponge_Vec2I min, max;
    float area2;
    sponge_draw_triangle_init(c, v0, v1, v2, &min, &max, &area2);
    sponge_Color32 *row = c.pixels + (min.y * c.stride_pixels);

    for (int32_t y = min.y; y <= max.y; y++, row += c.stride_pixels) {
        for (int32_t x = min.x; x <= max.x; x++) {
            float w0, w1, w2;
            if (sponge_draw_triangle_iter(sponge_vec2i_make(x, y), v0, v1, v2, area2, &w0, &w1, &w2)) {
                sponge_ColorF result = sponge_vec4_make(0.0f, 0.0f, 0.0f, 0.0f);
                result = sponge_vec4_add(result, sponge_vec4_mul(col0, w0));
                result = sponge_vec4_add(result, sponge_vec4_mul(col1, w1));
                result = sponge_vec4_add(result, sponge_vec4_mul(col2, w2));
                row[x] = sponge_colorf_to_color32(result);
            }
        }
    }
}

#endif // SPONGE_IMPLEMENTATION
