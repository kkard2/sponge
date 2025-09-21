#include <stdint.h>
#include <math.h>

typedef union {
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
sponge_Vec2 sponge_vec2_mul(sponge_Vec2 v0, float f);
float       sponge_vec2_dot(sponge_Vec2 v0, sponge_Vec2 v1);

sponge_Vec4 sponge_vec4_add(sponge_Vec4 v0, sponge_Vec4 v1);
sponge_Vec4 sponge_vec4_mul(sponge_Vec4 v0, float f);

sponge_ColorF  sponge_color32_to_colorf(sponge_Color32 col);
sponge_Color32 sponge_colorf_to_color32(sponge_ColorF  col);


// row-major
typedef union {
    struct {
        float c00; float c01; float c02; float c03;
        float c10; float c11; float c12; float c13;
        float c20; float c21; float c22; float c23;
        float c30; float c31; float c32; float c33;
    };
    float m[4][4];
    sponge_Vec4 rows[4];
} sponge_Mat4;

sponge_Mat4 sponge_mat4_mul_mat4(sponge_Mat4 m0, sponge_Mat4 m1);
sponge_Vec4 sponge_vec4_mul_mat4(sponge_Vec4 v0, sponge_Mat4 m0);
sponge_Vec3 sponge_vec3_mul_mat4(sponge_Vec3 v0, sponge_Mat4 m0); // sets .w to 1 and divides by .w at the end if necessary
sponge_Mat4 sponge_mat4_identity();
sponge_Mat4 sponge_mat4_translate(float x, float y, float z);
// TODO(kard): quaternion type beat
sponge_Mat4 sponge_mat4_rotate(float x, float y, float z); // y -> x -> z order
sponge_Mat4 sponge_mat4_scale(float x, float y, float z);
sponge_Mat4 sponge_mat4_projection(float fov_y, float aspect, float near, float far); // right-handed, looking down -z


// checks if tex.width or tex.height is equal to 0
// calling sponge functions with invalid textures is not supported
int32_t sponge_texture_valid(sponge_Texture tex);

// first argument is always rendering target (canvas)
void sponge_clear       (sponge_Texture c, sponge_Color32 col);
void sponge_clear_3d    (sponge_Texture c, float *depths, sponge_Color32 col);

void sponge_draw_rect   (sponge_Texture c, sponge_Vec2I v0, sponge_Vec2I v1, sponge_Color32 col);
void sponge_draw_line   (sponge_Texture c, sponge_Vec2I v0, sponge_Vec2I v1, sponge_Color32 col);
void sponge_draw_texture(sponge_Texture c, sponge_Vec2I offset, sponge_Texture tex);

sponge_Color32 sponge_sample_texture(sponge_Vec2 uv, sponge_Texture tex);

// tests on which side of the line p is (negative on the left, positive on the right)
int32_t sponge_edge_function(sponge_Vec2I p, sponge_Vec2I v0, sponge_Vec2I v1);

// out_area2 is optional
void    sponge_draw_triangle_init(
    sponge_Texture c, sponge_Vec2I v0, sponge_Vec2I v1, sponge_Vec2I v2,
    sponge_Vec2I *out_min, sponge_Vec2I *out_max, float *out_area2);

// non-zero if p inside triangle, outputs only valid if inside
int32_t sponge_draw_triangle_iter(
    sponge_Vec2I p, sponge_Vec2I v0, sponge_Vec2I v1, sponge_Vec2I v2, float area2,
    float *out_w0, float *out_w1, float *out_w2);

void sponge_draw_triangle_col3(
    sponge_Texture c,
    sponge_Vec2I  v0,   sponge_Vec2I  v1,   sponge_Vec2I  v2,
    sponge_ColorF col0, sponge_ColorF col1, sponge_ColorF col2);
void sponge_draw_triangle_uv(
    sponge_Texture c,
    sponge_Vec2I v0, sponge_Vec2I v1, sponge_Vec2I v2,
    sponge_Vec2 uv0, sponge_Vec2 uv1, sponge_Vec2 uv2, sponge_Texture tex);


typedef struct {
    int32_t t0, t1, t2;
    sponge_Vec3 v0_ndc, v1_ndc, v2_ndc;
    sponge_Vec2I v0_pixel, v1_pixel, v2_pixel;
    sponge_Vec2I min_pixel, max_pixel;
    float area2;
} sponge_DrawMeshTriangleContext;

sponge_DrawMeshTriangleContext sponge_draw_mesh_triangle_init(
    sponge_Texture c, sponge_Mat4 mvp,
    sponge_Vec3 *positions, int32_t *triangles, size_t triangle_start_index);

// non-zero if p inside triangle and depth test succeeds, outputs only valid if non-zero, depth is updated if non-zero
int32_t sponge_draw_mesh_triangle_iter(
    sponge_DrawMeshTriangleContext ctx, sponge_Vec2I p, float *inout_depth,
    float *out_w0, float *out_w1, float *out_w2);

void sponge_draw_mesh_col(
    sponge_Texture c, float *depths,
    sponge_Mat4 model, sponge_Mat4 view, sponge_Mat4 proj,
    sponge_Vec3 *positions, sponge_Color32 *colors, int32_t *triangles, size_t triangles_count);


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
sponge_Vec2 sponge_vec2_mul(sponge_Vec2 v0, float f) {
    return (sponge_Vec2){ .x = v0.x * f, .y = v0.y * f };
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

sponge_Mat4 sponge_mat4_mul_mat4(sponge_Mat4 m0, sponge_Mat4 m1) {
    sponge_Mat4 result;
    for (int32_t i = 0; i < 4; i++) {
        for (int32_t j = 0; j < 4; j++) {
            result.m[i][j] =
                (m0.m[i][0] * m1.m[0][j]) +
                (m0.m[i][1] * m1.m[1][j]) +
                (m0.m[i][2] * m1.m[2][j]) +
                (m0.m[i][3] * m1.m[3][j]);
        }
    }
    return result;
}
sponge_Vec4 sponge_vec4_mul_mat4(sponge_Vec4 v0, sponge_Mat4 m0) {
    sponge_Vec4 v;
    v.x = (v0.x * m0.c00) + (v0.y * m0.c10) + (v0.z * m0.c20) + (v0.w * m0.c30);
    v.y = (v0.x * m0.c01) + (v0.y * m0.c11) + (v0.z * m0.c21) + (v0.w * m0.c31);
    v.z = (v0.x * m0.c02) + (v0.y * m0.c12) + (v0.z * m0.c22) + (v0.w * m0.c32);
    v.w = (v0.x * m0.c03) + (v0.y * m0.c13) + (v0.z * m0.c23) + (v0.w * m0.c33);
    return v;
}
sponge_Vec3 sponge_vec3_mul_mat4(sponge_Vec3 v0, sponge_Mat4 m0) {
    sponge_Vec4 v = sponge_vec4_mul_mat4(sponge_vec4_make(v0.x, v0.y, v0.z, 1.0f), m0);
    if (v.w != 1.0f && v.w != 0.0f)
        return sponge_vec3_make(v.x / v.w, v.y / v.w, v.z / v.w);
    else
        return sponge_vec3_make(v.x, v.y, v.z);
}

sponge_Mat4 sponge_mat4_identity() {
    sponge_Mat4 m;
    m.c00 = 1.0f; m.c01 = 0.0f; m.c02 = 0.0f; m.c03 = 0.0f;
    m.c10 = 0.0f; m.c11 = 1.0f; m.c12 = 0.0f; m.c13 = 0.0f;
    m.c20 = 0.0f; m.c21 = 0.0f; m.c22 = 1.0f; m.c23 = 0.0f;
    m.c30 = 0.0f; m.c31 = 0.0f; m.c32 = 0.0f; m.c33 = 1.0f;
    return m;
}

sponge_Mat4 sponge_mat4_translate(float x, float y, float z) {
    sponge_Mat4 m = sponge_mat4_identity();
    m.c30 = x;
    m.c31 = y;
    m.c32 = z;
    return m;
}

sponge_Mat4 sponge_mat4_rotate(float x, float y, float z) {
    // TOOD(kard): make this not stupid
    float sx = sinf(x); float cx = cosf(x);
    float sy = sinf(y); float cy = cosf(y);
    float sz = sinf(z); float cz = cosf(z);
    sponge_Mat4 mx = sponge_mat4_identity();
    mx.c00 =   1; mx.c01 =   0; mx.c02 =   0;
    mx.c10 =   0; mx.c11 =  cx; mx.c12 =  sx;
    mx.c20 =   0; mx.c21 = -sx; mx.c22 =  cx;
    sponge_Mat4 my = sponge_mat4_identity();
    my.c00 =  cy; my.c01 =   0; my.c02 = -sy;
    my.c10 =   0; my.c11 =   1; my.c12 =   0;
    my.c20 =  sy; my.c21 =   0; my.c22 =  cy;
    sponge_Mat4 mz = sponge_mat4_identity();
    mz.c00 =  cz; mz.c01 =  sz; mz.c02 =   0;
    mz.c10 = -sz; mz.c11 =  cz; mz.c12 =   0;
    mz.c20 =   0; mz.c21 =   0; mz.c22 =   1;
    return sponge_mat4_mul_mat4(sponge_mat4_mul_mat4(my, mx), mz);
}

sponge_Mat4 sponge_mat4_scale(float x, float y, float z) {
    sponge_Mat4 m = sponge_mat4_identity();
    m.c00 = x;
    m.c11 = y;
    m.c22 = z;
    return m;
}

sponge_Mat4 sponge_mat4_projection(float fov_y, float aspect, float znear, float zfar) {
    float f = 1.0f / tanf(fov_y / 2.0f);
    sponge_Mat4 m;
    m.c00 = f / aspect; m.c01 = 0.0f; m.c02 = 0.0f;                   m.c03 = 0.0f;
    m.c10 = 0.0f;       m.c11 = f;    m.c12 = 0.0f;                   m.c13 = 0.0f;
    m.c20 = 0.0f;       m.c21 = 0.0f; m.c22 = -zfar / (zfar - znear); m.c23 = (-znear * zfar) / (zfar - znear);
    m.c30 = 0.0f;       m.c31 = 0.0f; m.c32 = -1.0f;                  m.c33 = 0.0f;
    return m;
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
void sponge_clear_3d(sponge_Texture c, float *depths, sponge_Color32 col) {
    sponge_clear(c, col);
    float *row = depths;
    for (uint32_t y = 0; y < c.height; y++, row += c.stride_pixels) {
        for (uint32_t x = 0; x < c.width; x++) {
            row[x] = INFINITY;
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

sponge_Color32 sponge_sample_texture(sponge_Vec2 uv, sponge_Texture tex) {
    SPONGE_ASSERT(sponge_texture_valid(tex));
    uint32_t x = SPONGE_CLAMP((uint32_t)((float)tex.width  * uv.x), 0, tex.width  - 1);
    uint32_t y = SPONGE_CLAMP((uint32_t)((float)tex.height * uv.y), 0, tex.height - 1);
    return tex.pixels[(y * tex.stride_pixels) + x];
}

int32_t sponge_edge_function(sponge_Vec2I v0, sponge_Vec2I v1, sponge_Vec2I p) {
    return ((p.x - v0.x) * (v1.y - v0.y)) - ((p.y - v0.y) * (v1.x - v0.x));
}

void    sponge_draw_triangle_init(
    sponge_Texture c, sponge_Vec2I v0, sponge_Vec2I v1, sponge_Vec2I v2,
    sponge_Vec2I *out_min, sponge_Vec2I *out_max, float *out_area2
) {
    SPONGE_ASSERT(sponge_texture_valid(c));
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

void sponge_draw_triangle_uv(
    sponge_Texture c,
    sponge_Vec2I v0, sponge_Vec2I v1, sponge_Vec2I v2,
    sponge_Vec2 uv0, sponge_Vec2 uv1, sponge_Vec2 uv2, sponge_Texture tex
) {
    sponge_Vec2I min, max;
    float area2;
    sponge_draw_triangle_init(c, v0, v1, v2, &min, &max, &area2);
    sponge_Color32 *row = c.pixels + (min.y * c.stride_pixels);

    for (int32_t y = min.y; y <= max.y; y++, row += c.stride_pixels) {
        for (int32_t x = min.x; x <= max.x; x++) {
            float w0, w1, w2;
            if (sponge_draw_triangle_iter(sponge_vec2i_make(x, y), v0, v1, v2, area2, &w0, &w1, &w2)) {
                sponge_Vec2 uv = sponge_vec2_make(0.0f, 0.0f);
                uv = sponge_vec2_add(uv, sponge_vec2_mul(uv0, w0));
                uv = sponge_vec2_add(uv, sponge_vec2_mul(uv1, w1));
                uv = sponge_vec2_add(uv, sponge_vec2_mul(uv2, w2));
                row[x] = sponge_sample_texture(uv, tex);
            }
        }
    }
}


sponge_DrawMeshTriangleContext sponge_draw_mesh_triangle_init(
    sponge_Texture c, sponge_Mat4 mvp,
    sponge_Vec3 *positions, int32_t *triangles, size_t triangle_start_index
) {
    sponge_DrawMeshTriangleContext result;
    result.t0 = triangles[triangle_start_index + 0];
    result.t1 = triangles[triangle_start_index + 1];
    result.t2 = triangles[triangle_start_index + 2];

    result.v0_ndc = sponge_vec3_mul_mat4(positions[result.t0], mvp);
    result.v1_ndc = sponge_vec3_mul_mat4(positions[result.t1], mvp);
    result.v2_ndc = sponge_vec3_mul_mat4(positions[result.t2], mvp);

    // TODO(kard): clipping?

    uint32_t halfw = c.width / 2;
    uint32_t halfh = c.height / 2;
    result.v0_pixel = sponge_vec2i_make((int32_t)((result.v0_ndc.x + 1.0f) * halfw), (int32_t)((result.v0_ndc.y + 1.0f) * halfh));
    result.v1_pixel = sponge_vec2i_make((int32_t)((result.v1_ndc.x + 1.0f) * halfw), (int32_t)((result.v1_ndc.y + 1.0f) * halfh));
    result.v2_pixel = sponge_vec2i_make((int32_t)((result.v2_ndc.x + 1.0f) * halfw), (int32_t)((result.v2_ndc.y + 1.0f) * halfh));
    result.v0_pixel.y = c.height - result.v0_pixel.y;
    result.v1_pixel.y = c.height - result.v1_pixel.y;
    result.v2_pixel.y = c.height - result.v2_pixel.y;

    sponge_draw_triangle_init(c, result.v0_pixel, result.v1_pixel, result.v2_pixel, &result.min_pixel, &result.max_pixel, &result.area2);

    return result;
}

int32_t sponge_draw_mesh_triangle_iter(
    sponge_DrawMeshTriangleContext ctx, sponge_Vec2I p, float *inout_depth,
    float *out_w0, float *out_w1, float *out_w2
) {
    if (sponge_draw_triangle_iter(p, ctx.v0_pixel, ctx.v1_pixel, ctx.v2_pixel, ctx.area2, out_w0, out_w1, out_w2)) {
        float depth = (ctx.v0_ndc.z * (*out_w0)) + (ctx.v1_ndc.z * (*out_w1)) + (ctx.v2_ndc.z * (*out_w2));
        if (depth < *inout_depth) {
            *inout_depth = depth;
            return 1;
        }
    }

    return 0;
}

void sponge_draw_mesh_col(
    sponge_Texture c, float *depths,
    sponge_Mat4 model, sponge_Mat4 view, sponge_Mat4 proj,
    sponge_Vec3 *positions, sponge_Color32 *colors, int32_t *triangles, size_t triangles_count
) {
    sponge_Mat4 mvp = sponge_mat4_mul_mat4(sponge_mat4_mul_mat4(model, view), proj);
    for (size_t i = 0; i < triangles_count; i += 3) {
        sponge_DrawMeshTriangleContext ctx = sponge_draw_mesh_triangle_init(c, mvp, positions, triangles, i);
        sponge_Color32 *row = c.pixels + (ctx.min_pixel.y * c.stride_pixels);
        float *depth_row = depths + (ctx.min_pixel.y * c.stride_pixels);

        for (int32_t y = ctx.min_pixel.y; y <= ctx.max_pixel.y; y++, row += c.stride_pixels, depth_row += c.stride_pixels) {
            for (int32_t x = ctx.min_pixel.x; x <= ctx.max_pixel.x; x++) {
                float w0, w1, w2;
                if (sponge_draw_mesh_triangle_iter(ctx, sponge_vec2i_make(x, y), &depth_row[x], &w0, &w1, &w2)) {
                    sponge_ColorF c0 = sponge_color32_to_colorf(colors[ctx.t0]);
                    sponge_ColorF c1 = sponge_color32_to_colorf(colors[ctx.t1]);
                    sponge_ColorF c2 = sponge_color32_to_colorf(colors[ctx.t2]);
                    sponge_ColorF result = sponge_vec4_make(0.0f, 0.0f, 0.0f, 0.0f);
                    result = sponge_vec4_add(result, sponge_vec4_mul(c0, w0));
                    result = sponge_vec4_add(result, sponge_vec4_mul(c1, w1));
                    result = sponge_vec4_add(result, sponge_vec4_mul(c2, w2));
                    row[x] = sponge_colorf_to_color32(result);
                }
            }
        }
    }
}

#endif // SPONGE_IMPLEMENTATION
