#include <assert.h>

#include "../sponge.h"
#include "../stb_image.h"

#define SPEED_X_ABS 2
#define SPEED_Y_ABS 2

sponge_Texture texture;

int32_t pos_x;
int32_t pos_y;
int32_t speed_x;
int32_t speed_y;

void init() {
    unsigned char *data = stbi_load("examples/kot.png", &texture.width, &texture.height, NULL, 4);
    if (!data) {
        printf("%s\n", stbi_failure_reason());
        assert(0);
    }

    texture.stride = texture.width;
    texture.pixels = (uint32_t *)data; // NOTE(kard): be careful if changing following loop

    // converting from stbi's RGBA to ARGB
    for (size_t i = 0; i < texture.width * texture.height; i++) {
        uint32_t result = 0;
        result |= data[i * 4 + 0] << 16; // R
        result |= data[i * 4 + 1] << 8;  // G
        result |= data[i * 4 + 2] << 0;  // B
        result |= data[i * 4 + 3] << 24; // A
        texture.pixels[i] = result;
    }

    pos_x = 20;
    pos_y = 10;
    speed_x = SPEED_X_ABS;
    speed_y = SPEED_Y_ABS;
}

void draw_frame(sponge_Texture c) {
    sponge_clear(c, 0xFF000000);

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

    sponge_draw_texture(c, pos_x, pos_y, texture);
}
