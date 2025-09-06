#include <stdio.h>
#include <math.h>



#include "../sponge.h"

#define PI ((float)3.14159265358979323846)
#define SPEED (2.0f / 360.0f * 2 * PI)
static float angle = 0.0f;

void draw_frame(sponge_Texture c) {
    angle += SPEED;

    float rf = (sinf(angle) + 1.0f) * 255.0f / 2.0f;
    float gf = (sinf(angle + (PI * 2.0f / 3.0f)) + 1.0f) * 255.0f / 2.0f;
    float bf = (sinf(angle + (PI * 4.0f / 3.0f)) + 1.0f) * 255.0f / 2.0f;

    int r = (((int)rf) & 0xFF) << 16;
    int g = (((int)gf) & 0xFF) << 8;
    int b = (((int)bf) & 0xFF) << 0;

    uint32_t color = 0xFF000000 | r | g | b;
    sponge_clear(c, color);
}
