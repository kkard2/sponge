#include <math.h>

#include "../sponge.h"
#define SPONGE_EXAMPLE_IMPLEMENTATION
#include "example.h"

#define SPEED (2.0f / 360.0f * 2 * PI)
static float angle = 0.0f;

void draw_frame(sponge_Texture c) {
    angle += SPEED;

    float rf = (sinf(angle) + 1.0f) * 255.0f / 2.0f;
    float gf = (sinf(angle + (PI * 2.0f / 3.0f)) + 1.0f) * 255.0f / 2.0f;
    float bf = (sinf(angle + (PI * 4.0f / 3.0f)) + 1.0f) * 255.0f / 2.0f;

    sponge_Color32 color;
    color.a = 0xFF;
    color.r = (((int)rf) & 0xFF);
    color.g = (((int)gf) & 0xFF);
    color.b = (((int)bf) & 0xFF);
    sponge_clear(c, color);
}
