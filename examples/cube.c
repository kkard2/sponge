#include <assert.h>

#include "../sponge.h"
#define SPONGE_EXAMPLE_IMPLEMENTATION
#define SPONGE_EXAMPLE_INIT_DEFINED
#define SPONGE_EXAMPLE_MOUSE_MOVE_DEFINED
#define SPONGE_EXAMPLE_DRAW_FRAME_3D_DEFINED
#include "example.h"

#include "vendor/stb_image.h"

#define FOV (60.0f / PI * 2.0f)
#define DISTANCE (10.0f)

static sponge_Texture texture;
static int32_t mouse_x;
static int32_t mouse_y;

void init() {
    unsigned char *data = stbi_load("examples/assets/kothd.png", &texture.width, &texture.height, NULL, 4);
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
}

void mouse_move(int32_t x, int32_t y) {
    mouse_x = x;
    mouse_y = y;
}

void draw_frame_3d(sponge_Texture c, float *depths) {
    sponge_clear_3d(c, depths, sponge_color32_make(0xFF000000));

    // NOTE(kard): these directions are not intuitive at all, but it works
    sponge_Vec3 positions[] = {
        // front
        sponge_vec3_make( 0.5f,  0.5f, -0.5f),
        sponge_vec3_make(-0.5f,  0.5f, -0.5f),
        sponge_vec3_make( 0.5f, -0.5f, -0.5f),
        sponge_vec3_make(-0.5f, -0.5f, -0.5f),
        // right
        sponge_vec3_make(-0.5f,  0.5f, -0.5f),
        sponge_vec3_make(-0.5f,  0.5f,  0.5f),
        sponge_vec3_make(-0.5f, -0.5f, -0.5f),
        sponge_vec3_make(-0.5f, -0.5f,  0.5f),
        // left
        sponge_vec3_make( 0.5f,  0.5f,  0.5f),
        sponge_vec3_make( 0.5f,  0.5f, -0.5f),
        sponge_vec3_make( 0.5f, -0.5f,  0.5f),
        sponge_vec3_make( 0.5f, -0.5f, -0.5f),
        // top
        sponge_vec3_make( 0.5f,  0.5f,  0.5f),
        sponge_vec3_make(-0.5f,  0.5f,  0.5f),
        sponge_vec3_make( 0.5f,  0.5f, -0.5f),
        sponge_vec3_make(-0.5f,  0.5f, -0.5f),
        // bottom
        sponge_vec3_make(-0.5f, -0.5f,  0.5f),
        sponge_vec3_make( 0.5f, -0.5f,  0.5f),
        sponge_vec3_make(-0.5f, -0.5f, -0.5f),
        sponge_vec3_make( 0.5f, -0.5f, -0.5f),
        // there is no back
    };
    // NOTE(kard): do not ask any questions
    sponge_Vec2 uvs[] = {
        // front
        sponge_vec2_make(0.33f, 0.33f),
        sponge_vec2_make(0.66f, 0.33f),
        sponge_vec2_make(0.33f, 0.66f),
        sponge_vec2_make(0.66f, 0.66f),
        // right
        sponge_vec2_make(0.66f, 0.33f),
        sponge_vec2_make(1.00f, 0.00f),
        sponge_vec2_make(0.66f, 0.66f),
        sponge_vec2_make(1.00f, 1.00f),
        // left
        sponge_vec2_make(0.00f, 0.00f),
        sponge_vec2_make(0.33f, 0.33f),
        sponge_vec2_make(0.00f, 1.00f),
        sponge_vec2_make(0.33f, 0.66f),
        // top
        sponge_vec2_make(0.00f, 0.00f),
        sponge_vec2_make(1.00f, 0.00f),
        sponge_vec2_make(0.33f, 0.33f),
        sponge_vec2_make(0.66f, 0.33f),
        // bottom
        sponge_vec2_make(0.00f, 1.00f),
        sponge_vec2_make(1.00f, 1.00f),
        sponge_vec2_make(0.33f, 0.66f),
        sponge_vec2_make(0.66f, 0.66f),
    };

    int32_t triangles[] = {
        // front
        0, 1, 2,
        2, 1, 3,
        // right
        4, 5, 6,
        6, 5, 7,
        // left
        8, 9, 10,
        10, 9, 11,
        // top
        12, 13, 14,
        14, 13, 15,
        // bottom
        16, 17, 18,
        18, 17, 19,
    };


    sponge_Mat4 model = sponge_mat4_identity();

    float rot_y = (float)mouse_x / (float)c.width * PI * 2.0f;
    float rot_x = ((float)mouse_y / (float)c.height * PI) - (PI * 0.5f);
    sponge_Mat4 view = sponge_mat4_identity();
    view = sponge_mat4_mul_mat4(view, sponge_mat4_rotate(rot_x, rot_y, 0.0f));
    view = sponge_mat4_mul_mat4(view, sponge_mat4_translate(0.0f, 0.0f, -5.0f));

    sponge_Mat4 proj = sponge_mat4_projection(FOV, (float)c.width / (float)c.height, 1.0f, 100.0f);

    sponge_draw_mesh_uv(c, depths, model, view, proj, positions, uvs, triangles, 30, texture);
}
