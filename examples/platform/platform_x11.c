// TODO(kard): destroy this file, i don't care about platform layer rn this is some gpt trash
// it flickers so bad

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define SPONGE_IMPLEMENTATION
#include "../../sponge.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../vendor/stb_image.h"

#include "../example.h"

#define DEFAULT_WIDTH 256
#define DEFAULT_HEIGHT 256

static size_t pixels_buffer_count = 0;
static sponge_Texture canvas = { 0 };
static float *depths = NULL;
static Display *dpy;
static XImage *ximg = NULL;


void init_canvas(uint32_t width, uint32_t height) {
    if (ximg) {
        ximg->data = NULL;
        XDestroyImage(ximg);
        ximg = NULL;
    }

    size_t new_count = width * height;
    if (new_count > pixels_buffer_count) {
        sponge_Color32 *new_pixels_buffer = malloc(new_count * sizeof(uint32_t));
        float *new_depths = malloc(new_count * sizeof(float));
        if (!new_pixels_buffer || !new_depths) {
            free(new_pixels_buffer);
            free(new_depths);
            exit(1);
            fprintf(stderr, "failed to allocate pixel buffer\n");
        }
        free(canvas.pixels);
        free(depths);
        canvas.pixels = new_pixels_buffer;
        depths = new_depths;
        pixels_buffer_count = new_count;
    }

    canvas.width = width;
    canvas.height = height;
    canvas.stride_pixels = width;

    // TODO(kard): i assume this can fail
    ximg = XCreateImage(
        dpy,
        DefaultVisual(dpy, 0),
        DefaultDepth(dpy, 0),
        ZPixmap,
        0,
        (char*)canvas.pixels,
        width,
        height,
        32,
        0
    );
}

int main() {
    dpy = XOpenDisplay(NULL);
    if (!dpy) {
        fprintf(stderr, "failed to open display\n");
        return 1;
    }

    int screen = DefaultScreen(dpy);

    // TODO(kard): i assume all of the following x functions can fail
    // clanker only checked XOpenDisplay for some reason

    Window win = XCreateSimpleWindow(
        dpy,
        RootWindow(dpy, screen),
        10, 10, DEFAULT_WIDTH, DEFAULT_HEIGHT,
        1,
        BlackPixel(dpy, screen),
        BlackPixel(dpy, screen)
    );

    XStoreName(dpy, win, "sponge");
    XSelectInput(dpy, win, ExposureMask | PointerMotionMask | StructureNotifyMask);

    GC gc = DefaultGC(dpy, screen);

    XMapWindow(dpy, win);

    init_canvas(DEFAULT_WIDTH, DEFAULT_HEIGHT);
    init();

    int running = 1;
    while (running) {
        while (XPending(dpy)) {
            XEvent ev;
            XNextEvent(dpy, &ev);
            switch (ev.type) {
                case Expose:
                    //draw_frame(canvas);
                    break;
                case ConfigureNotify:
                    init_canvas(ev.xconfigure.width, ev.xconfigure.height);
                    break;
                case MotionNotify:
                    mouse_move(ev.xmotion.x, ev.xmotion.y);
                    break;
            }
        }

        if (sponge_texture_valid(canvas)) {
            draw_frame_3d(canvas, depths);
            XPutImage(dpy, win, gc, ximg, 0, 0, 0, 0, canvas.width, canvas.height);
        }

        usleep(16000);
    }

    if (ximg) {
        ximg->data = NULL; // prevent double free
        XDestroyImage(ximg);
    }

    XDestroyWindow(dpy, win);
    XCloseDisplay(dpy);
    return 0;
}
