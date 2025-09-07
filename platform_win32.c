#define UNICODE
#include <windows.h>
#include <assert.h>
#include <stdio.h>
#include <io.h>

#include <fcntl.h>

#define SPONGE_IMPLEMENTATION
#include "sponge.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "example.h"

#define DEFAULT_WIDTH 256
#define DEFAULT_HEIGHT 256

static size_t pixels_buffer_count;
static sponge_Texture canvas;
static BITMAPINFO bmi;

int update_canvas(sponge_Texture *canvas, uint32_t new_width, uint32_t new_height) {
    size_t new_count = new_width * new_height;
    if (new_count > pixels_buffer_count) {
        uint32_t *new_pixels_buffer = malloc(new_count * sizeof(uint32_t));
        if (!new_pixels_buffer)
            return 0;
        free(canvas->pixels);
        canvas->pixels = new_pixels_buffer;
        pixels_buffer_count = new_count;
    }
    canvas->width = new_width;
    canvas->height = new_height;
    canvas->stride = new_width; // TODO(kard): think about this if we want image to stay the same without redraw

    ZeroMemory(&bmi, sizeof(bmi));
    bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth       = new_width;
    bmi.bmiHeader.biHeight      = -(int32_t)new_height; // negative = top-down
    bmi.bmiHeader.biPlanes      = 1;
    bmi.bmiHeader.biBitCount    = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    return 1;
}

// TODO(kard): MSVC complains about freopen (should be freopen_s apparently)
void redirect_io_to_console() {
    AllocConsole();
    FILE *fp;

    fp = freopen("CONOUT$", "w", stdout);
    setvbuf(stdout, NULL, _IONBF, 0);

    fp = freopen("CONOUT$", "w", stderr);
    setvbuf(stderr, NULL, _IONBF, 0);

    fp = freopen("CONIN$", "r", stdin);
    setvbuf(stdin, NULL, _IONBF, 0);
}

LRESULT WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_DESTROY: {
            PostQuitMessage(0);
            return 0;
        }
        case WM_SIZE: {
            uint32_t width = LOWORD(lParam);
            uint32_t height = HIWORD(lParam);
            // TODO(kard): handle properly? (what does that even mean)
            assert(update_canvas(&canvas, width, height));
        }
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

int WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR     lpCmdLine,
    int       nShowCmd
) {
    redirect_io_to_console();

    const wchar_t CLASS_NAME[]  = L"sponge";

    WNDCLASS wc = { 0 };

    wc.lpfnWndProc   = WindowProc;
    wc.hInstance     = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        L"sponge",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, DEFAULT_WIDTH, DEFAULT_HEIGHT,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (hwnd == NULL)
        return 1;

    update_canvas(&canvas, DEFAULT_WIDTH, DEFAULT_HEIGHT);

    ShowWindow(hwnd, nShowCmd);

    MSG msg = {0};
    BOOL running = TRUE;

    init();

    while (running) {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                running = FALSE;
                break;
            }

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (!running) break;

        draw_frame(canvas);

        HDC hdc = GetDC(hwnd);
        StretchDIBits(
            hdc,
            // TODO(kard): check what happens if canvas is bigger than window (it shouldn't differ currently)
            0, 0, canvas.width, canvas.height, // dest rectangle
            0, 0, canvas.width, canvas.height, // src rectangle
            canvas.pixels,
            &bmi,
            DIB_RGB_COLORS,
            SRCCOPY
        );

        InvalidateRect(hwnd, NULL, FALSE);
        UpdateWindow(hwnd);

        // TODO(kard): proper frame limiting
        Sleep(16);
    }

    return (int) msg.wParam;
}
