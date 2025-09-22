#include <assert.h>

#include "../sponge.h"
#define SPONGE_EXAMPLE_IMPLEMENTATION
#define SPONGE_EXAMPLE_INIT_DEFINED
#include "example.h"
#include "vendor/stb_image.h"

#define SPEED_X_ABS 2
#define SPEED_Y_ABS 2

sponge_Texture texture;

int32_t pos_x;
int32_t pos_y;
int32_t speed_x;
int32_t speed_y;

void init() {
    unsigned char *data = stbi_load("examples/assets/kot.png", &texture.width, &texture.height, NULL, 4);
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

    pos_x = 20;
    pos_y = 10;
    speed_x = SPEED_X_ABS;
    speed_y = SPEED_Y_ABS;
}

void draw_frame(sponge_Texture c) {
    sponge_clear(c, sponge_color32_make(0xFF000000));

    pos_x += speed_x;
    pos_y += speed_y;

    if (pos_x < 0) {
        pos_x = 0;
        speed_x = SPEED_X_ABS;
    } else if (pos_x + texture.width > c.width) {
        speed_x = -SPEED_X_ABS;
    }

    if (pos_y < 0) {
        pos_y = 0;
        speed_y = SPEED_X_ABS;
    } else if (pos_y + texture.height > c.height) {
        speed_y = -SPEED_Y_ABS;
    }

    sponge_draw_texture(c, sponge_vec2i_make(pos_x, pos_y), texture);
}
