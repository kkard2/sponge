#include <math.h>

#include "../sponge.h"

#define PI ((float)3.14159265358979323846)
#define SPEED (0.5f / 360.0f * 2 * PI)
static float angle = 0.0f;
#define MIN(x, y) ((x) < (y) ? (x) : (y))

void init() {}

void draw_frame(sponge_Texture c) {
    sponge_clear(c, 0xFF000000);
    angle += SPEED;

    float x = sinf(angle);
    float y = cosf(angle);
    int32_t half = (int32_t)MIN(c.width, c.height) / 2;

    sponge_draw_line(
        c,
        half + (int32_t)( x * (float)half), half + (int32_t)( y * (float)half),
        half + (int32_t)(-x * (float)half), half + (int32_t)(-y * (float)half),
        0xFFFF00FF);
}
