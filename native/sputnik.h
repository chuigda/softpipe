#ifndef CW_SOFTPIPE_UTIL_H
#define CW_SOFTPIPE_UTIL_H

#include "softpipe.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct stSPUTWindow SPUTWindow;

SPUTWindow *sputCreateWindow(SP_CONST char *title,
                             size_t width,
                             size_t height);
bool sputWindowDisplay(SPUTWindow *window, SoftpipeFramebuffer *fb);
void sputWindowMainLoop(SPUTWindow *window);
void sputDestroyWindow(SPUTWindow *window);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* CW_SOFTPIPE_UTIL_H */
