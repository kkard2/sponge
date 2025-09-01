#define UNICODE
#include <windows.h>

#define SOFTRENDER_IMPLEMENTATION
#include "softrender.h"

// TODO(kard): ofc some resizing stuff
#define WIDTH 256
#define HEIGHT 256

static uint32_t pixel_buffer[WIDTH * HEIGHT];

LRESULT WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_DESTROY: {
            PostQuitMessage(0);
            return 0;
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
    const wchar_t CLASS_NAME[]  = L"softrender";

    WNDCLASS wc = { 0 };

    wc.lpfnWndProc   = WindowProc;
    wc.hInstance     = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        L"softrender",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, // bla bla you can't resize window basically
        CW_USEDEFAULT, CW_USEDEFAULT, WIDTH, HEIGHT,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (hwnd == NULL)
        return 1;

    ShowWindow(hwnd, nShowCmd);

    BITMAPINFO bmi;
    ZeroMemory(&bmi, sizeof(bmi));
    bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth       = WIDTH;
    bmi.bmiHeader.biHeight      = -HEIGHT; // negative = top-down
    bmi.bmiHeader.biPlanes      = 1;
    bmi.bmiHeader.biBitCount    = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    soft_Canvas canvas = {
        .pixels = pixel_buffer,
        .width = WIDTH,
        .height = HEIGHT,
        .stride = WIDTH,
    };

    MSG msg = {0};
    BOOL running = TRUE;

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


        // NOTE(kard): ACTUAL RENDERING CODE HERE !!!
        soft_clear(canvas, 0xFF000000);
        soft_draw_rect(canvas, 80, 70, 80 + 20, 70 + 40, 0xFFFF00FF);
        soft_draw_rect(canvas, 140, 70, 140 + 20, 70 + 40, 0xFFFF00FF);


        HDC hdc = GetDC(hwnd);
        StretchDIBits(
            hdc,
            0, 0, WIDTH, HEIGHT, // dest rectangle
            0, 0, WIDTH, HEIGHT, // src rectangle
            pixel_buffer,
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
