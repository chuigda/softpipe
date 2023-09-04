#include "sput.h"

#include <Windows.h>
#include <stdlib.h>

struct stSPUTWindow {
    HWND hWnd;
    HBITMAP hBitmap;
    HBRUSH hBrush;
};

static LPCWSTR g_lpszClassName = L"SPUTWindow";
static WNDCLASSW g_wndClass;
static BOOL g_wndClassInitialised = FALSE;

static LRESULT CALLBACK WndProc(HWND hWnd,
                                UINT msg,
                                WPARAM wParam,
                                LPARAM lParam);

static BOOL initWndClass(void) {
    if (!g_wndClassInitialised) {
        g_wndClass.style = CS_HREDRAW | CS_VREDRAW;
        g_wndClass.lpfnWndProc = WndProc;
        g_wndClass.cbClsExtra = 0;
        g_wndClass.cbWndExtra = 0;
        g_wndClass.hInstance = GetModuleHandle(NULL);
        g_wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
        g_wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
        g_wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
        g_wndClass.lpszMenuName = NULL;
        g_wndClass.lpszClassName = g_lpszClassName;

        if (!RegisterClassW(&g_wndClass)) {
            return FALSE;
        }

        g_wndClassInitialised = TRUE;
    }

    return TRUE;
}

SPUTWindow *sputCreateWindow(const char *title,
                             size_t width,
                             size_t height) {
    if (!initWndClass()) {
        return NULL;
    }

    SPUTWindow *window = malloc(sizeof(SPUTWindow));
    if (!window) {
        return NULL;
    }

    WCHAR wTitle[1024];
    MultiByteToWideChar(CP_UTF8, 0, title, -1, wTitle, 1024);

    HWND hWnd = CreateWindowW(
        g_lpszClassName,
        wTitle,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        width,
        height,
        NULL,
        NULL,
        GetModuleHandle(NULL),
        NULL
    );
    if (!hWnd) {
        free(window);
        return NULL;
    }

    window->hWnd = hWnd;
    window->hBitmap = NULL;
    window->hBrush = NULL;
    SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)window);

    return window;
}

bool sputWindowDisplay(SPUTWindow *window, SoftpipeFramebuffer *fb) {
    size_t fbWidth, fbHeight;
    spGetFramebufferSize(fb, &fbWidth, &fbHeight);

    float *pixels = malloc(fbWidth * fbHeight * 4 * sizeof(float));
    if (!pixels) {
        return false;
    }
    spReadPixel(fb, pixels);

    uint8_t *pixels_u8 =
        malloc(fbWidth * fbHeight * 4 * sizeof(uint8_t));
    if (!pixels_u8) {
        free(pixels);
        return false;
    }

    for (size_t i = 0; i < fbWidth * fbHeight * 4; i++) {
        pixels_u8[i] = (uint8_t)(pixels[i] * 255.0f);
    }

    HBITMAP hBitmap = CreateBitmap(fbWidth, fbHeight, 1, 32, pixels_u8);
    if (!hBitmap) {
        free(pixels);
        free(pixels_u8);
        return false;
    }

    HBRUSH hBrush = CreatePatternBrush(hBitmap);
    if (!hBrush) {
        DeleteObject(hBitmap);
        free(pixels);
        free(pixels_u8);
        return false;
    }

    if (window->hBitmap) {
        DeleteObject(window->hBitmap);
    }
    if (window->hBrush) {
        DeleteObject(window->hBrush);
    }

    window->hBitmap = hBitmap;
    window->hBrush = hBrush;
    return true;
}

void sputWindowMainLoop(SPUTWindow *window) {
    ShowWindow(window->hWnd, SW_SHOWDEFAULT);
    UpdateWindow(window->hWnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
}

void sputDestroyWindow(SPUTWindow *window) {
    if (window->hBitmap) {
        DeleteObject(window->hBitmap);
    }
    if (window->hBrush) {
        DeleteObject(window->hBrush);
    }
    DestroyWindow(window->hWnd);
    free(window);
}

static LRESULT CALLBACK WndProc(HWND hWnd,
                                UINT msg,
                                WPARAM wParam,
                                LPARAM lParam) {
    SPUTWindow *window =
        (SPUTWindow*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

    switch (msg) {
    case WM_PAINT: {
        if (!window->hBitmap || !window->hBrush) {
            break;
        }

        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        RECT rect;
        GetClientRect(hWnd, &rect);

        HDC hMemDC = CreateCompatibleDC(hdc);
        HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, window->hBitmap);

        FillRect(hMemDC, &rect, window->hBrush);

        BitBlt(hdc, 0, 0, rect.right, rect.bottom, hMemDC, 0, 0, SRCCOPY);

        SelectObject(hMemDC, hOldBitmap);
        DeleteDC(hMemDC);

        EndPaint(hWnd, &ps);
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProcW(hWnd, msg, wParam, lParam);
    }

    return 0;
}