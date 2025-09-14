#include <assert.h>

#include "../sponge.h"
#define SPONGE_EXAMPLE_IMPLEMENTATION
#define SPONGE_EXAMPLE_INIT_DEFINED
#define SPONGE_EXAMPLE_MOUSE_MOVE_DEFINED
#include "../example.h"
#include "../stb_image.h"

sponge_Texture texture;

int32_t mouse_x;
int32_t mouse_y;

void init() {
    unsigned char *data = stbi_load("examples/kot.png", &texture.width, &texture.height, NULL, 4);
    if (!data) {
        printf("%s\n", stbi_failure_reason());
        assert(0);
    }

    texture.stride_pixels = texture.width;
    texture.pixels = (sponge_Color32 *)data; // NOTE(kard): be careful if changing following loop

    // converting from stbi's RGBA
    for (size_t i = 0; i < texture.width * texture.height; i++) {
        sponge_Color32 result;
        result.r = data[i * 4 + 0];
        result.g = data[i * 4 + 1];
        result.b = data[i * 4 + 2];
        result.a = data[i * 4 + 3];
        texture.pixels[i] = result;
    }

    mouse_x = 100;
    mouse_y = 100;
}

void mouse_move(int32_t x, int32_t y) {
    mouse_x = x;
    mouse_y = y;
}

void draw_frame(sponge_Texture c) {
    sponge_clear(c, sponge_color32_make(0xFF000000));
    sponge_Vec2I v0 = sponge_vec2i_make(mouse_x, mouse_y);
    sponge_Vec2I v1 = sponge_vec2i_make(c.width - 100, 100);
    sponge_Vec2I v2 = sponge_vec2i_make(100, c.height - 100);
    sponge_Vec2I v3 = sponge_vec2i_make(c.width - 100, c.height - 100);
    sponge_Vec2 uv0 = sponge_vec2_make(0.0f, 0.0f);
    sponge_Vec2 uv1 = sponge_vec2_make(1.0f, 0.0f);
    sponge_Vec2 uv2 = sponge_vec2_make(0.0f, 1.0f);
    sponge_Vec2 uv3 = sponge_vec2_make(1.0f, 1.0f);

    sponge_draw_triangle_uv(c, v0, v1, v3, uv0, uv1, uv3, texture);
    sponge_draw_triangle_uv(c, v0, v3, v2, uv0, uv3, uv2, texture);
}
