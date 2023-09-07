#include "sputnik.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/nearptr.h>
#include <dpmi.h>
#include <pc.h>

static inline void setupVGAPalette(void);

struct stSPUTWindow {
    bool active;
};

static SPUTWindow g_WindowInstance = { 0 };

SPUTWindow *sputCreateWindow(SP_CONST char *title,
                             size_t width,
                             size_t height) {
		(void)title;

		assert(width == 320);
    assert(height == 200);
    assert(!g_WindowInstance.active);

    __dpmi_regs regs;
    memset(&regs, 0, sizeof(regs));
    regs.x.ax = 0x13;
    __dpmi_int(0x10, &regs);

    setupVGAPalette();

    g_WindowInstance.active = true;
    return &g_WindowInstance;
}

bool sputWindowDisplay(SPUTWindow *window, SoftpipeFramebuffer *fb) {
		(void)window;

    size_t fbWidth, fbHeight;
    spGetFramebufferSize(fb, &fbWidth, &fbHeight);
    assert(fbWidth == 320);
    assert(fbHeight == 200);

    if (__djgpp_nearptr_enable() == 0) {
        return 0;
    }
    uint8_t *videomem = (uint8_t*)(
				(uintptr_t)__djgpp_conventional_base +
				(uintptr_t)0xA0000
		);
    spReadPixelRGB332(fb, videomem);
    __djgpp_nearptr_disable();
    return 1;
}

void sputWindowMainLoop(SPUTWindow *window) {
    if (window->active) {
        getchar();
    }
}

void sputDestroyWindow(SPUTWindow *window) {
    if (window->active) {
        window->active = 0;
    }

    __dpmi_regs regs;
    memset(&regs, 0, sizeof(regs));
    regs.x.ax = 0x10;
    __dpmi_int(0x10, &regs);

    system("mode 80:28");
}

static inline void setupVGAPalette(void) {
    static bool initialised = 0;
    if (initialised) {
        return;
    }

    outportb(0x3C8, 0);
    for (size_t i = 0; i <= 255; i++) {
        size_t r = (i >> 5) & 0x7;
        size_t g = (i >> 2) & 0x7;
        size_t b = i & 0x3;

        outportb(0x3C9, r * 9);
        outportb(0x3C9, g * 9);
        outportb(0x3C9, b * 21);
    }

    initialised = 1;
}
