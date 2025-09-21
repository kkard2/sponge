#include <stdint.h>

#define PI ((float)3.14159265358979323846)

void init();
void draw_frame(sponge_Texture c);
void mouse_move(int32_t x, int32_t y);
void draw_frame_3d(sponge_Texture c, float *depths);

#ifdef SPONGE_EXAMPLE_IMPLEMENTATION

#ifndef SPONGE_EXAMPLE_INIT_DEFINED
void init() {}
#endif // SPONGE_EXAMPLE_INIT_DEFINED

#ifndef SPONGE_EXAMPLE_MOUSE_MOVE_DEFINED
void mouse_move(int32_t x, int32_t y) {}
#endif // SPONGE_EXAMPLE_MOUSE_MOVE_DEFINED

#ifndef SPONGE_EXAMPLE_DRAW_FRAME_3D_DEFINED
void draw_frame_3d(sponge_Texture c, float *depths) {
    draw_frame(c);
}
#endif // SPONGE_EXAMPLE_DRAW_FRAME_3D_DEFINED

#endif // SPONGE_EXAMPLE_IMPLEMENTATION
