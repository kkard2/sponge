// TODO(kard): destroy this file, i don't care about platform layer rn this is some gpt trash
// it flickers so bad

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// TODO(kard): we should probably put more thought into this
#include "example.c"

#define DEFAULT_WIDTH 256
#define DEFAULT_HEIGHT 256

static sponge_Canvas canvas = { 0 };
static Display *dpy;
static XImage *ximg = 0;


void init_canvas(uint32_t width, uint32_t height) {
    if (ximg) {
        XDestroyImage(ximg); // also frees data (why??)
        // TODO(kard): you can set ximg->data to null before XDestroyImage
        ximg = NULL;
    }

    canvas.pixels = (uint32_t*)malloc(width * height * 4);
    if (!canvas.pixels) {
        fprintf(stderr, "failed to allocate pixel buffer\n");
        exit(1);
    }
    canvas.width = width;
    canvas.height = height;
    canvas.stride = width;

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
    XSelectInput(dpy, win, ExposureMask | KeyPressMask | StructureNotifyMask);

    GC gc = DefaultGC(dpy, screen);

    XMapWindow(dpy, win);

    init_canvas(DEFAULT_WIDTH, DEFAULT_HEIGHT);

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
                    // TODO(kard): this is some implicit signed to unsigned conversion
                    init_canvas(ev.xconfigure.width, ev.xconfigure.height);
                    break;
                case KeyPress:
                    break;
            }
        }

        draw_frame(canvas);
        XPutImage(dpy, win, gc, ximg, 0, 0, 0, 0, canvas.width, canvas.height);

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
