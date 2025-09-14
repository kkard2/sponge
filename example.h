#include <stdint.h>

void init();
void draw_frame(sponge_Texture c);
void mouse_move(int32_t x, int32_t y);

#ifdef SPONGE_EXAMPLE_IMPLEMENTATION

#ifndef SPONGE_EXAMPLE_INIT_DEFINED
void init() {}
#endif // SPONGE_EXAMPLE_INIT_DEFINED

#ifndef SPONGE_EXAMPLE_MOUSE_MOVE_DEFINED
void mouse_move(int32_t x, int32_t y) {}
#endif // SPONGE_EXAMPLE_MOUSE_MOVE_DEFINED

#endif // SPONGE_EXAMPLE_IMPLEMENTATION
