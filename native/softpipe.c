#include "softpipe.h"
#include <stdlib.h>
#include <string.h>

static void* defaultAlloc(size_t size, size_t alignment) {
    (void)alignment;
    return malloc(size);
}

static void defaultFree(void* ptr) {
    free(ptr);
}

static SoftpipeAllocator g_DefaultAllocator = {
    .alloc = defaultAlloc,
    .free = defaultFree
};

struct stFramebuffer {
    size_t width;
    size_t height;
    uint8_t colorBuffer[0];
};

SoftpipeFramebuffer *spCreateFramebuffer(size_t width,
                                         size_t height,
                                         SoftpipeAllocator *allocator) {
    if (!allocator) {
        allocator = &g_DefaultAllocator;
    }

    SoftpipeFramebuffer *fb = 
        allocator->alloc(sizeof(SoftpipeFramebuffer) + width * height * 4, 1);
    if (!fb) {
        return NULL;
    }

    fb->width = width;
    fb->height = height;
    memset(fb->colorBuffer, 0, width * height * 4);
    return fb;
}

void spDeleteFramebuffer(SoftpipeFramebuffer *fb,
                         SoftpipeAllocator *allocator) {
    if (!allocator) {
        allocator = &g_DefaultAllocator;
    }

    allocator->free(fb);
}
