#ifndef CW_SOFTPIPE_H
#define CW_SOFTPIPE_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    void *(*alloc)(size_t size, size_t alignment);
    void (*free)(void *ptr);
} SoftpipeAllocator;

struct stFramebuffer;
typedef struct st_framebuffer SoftpipeFramebuffer;

SoftpipeFramebuffer *spCreateFramebuffer(size_t width,
                                         size_t height,
                                         SoftpipeAllocator *allocator);

void spDeleteFramebuffer(SoftpipeFramebuffer *fb,
                         SoftpipeAllocator *allocator);

typedef struct {
    size_t size;
    size_t alignment;
} SoftpipeBlockDescriptor;

typedef struct {
    size_t numVertices;
    size_t vertexIndex;
} SoftpipeVertexShaderGlobals;

typedef struct {
    size_t framebufferWidth;
    size_t framebufferHeight;
    size_t fragmentX;
    size_t fragmentY;
    _Bool isFrontFacing;
} SoftpipeFragmentShaderGlobals;

typedef _Bool (*SoftpipeVertexShader)(void *vertexShaderInputBlock,
                                      void *vertexShaderOutputBlock,
                                      const void *uniformBlock,
                                      const SoftpipeVertexShaderGlobals *globals);

typedef struct {
    float r, g, b, a;
} SoftpipeFragmentColor;

typedef SoftpipeFragmentColor (*SoftpipeFragmentShader)(
    void *fragmentShaderInputBlock,
    const void *uniformBlock,
    const SoftpipeFragmentShaderGlobals *globals);

struct stSoftpipeContext;
typedef struct stSoftpipeContext SoftpipeContext;

SoftpipeContext *spCreateContext(SoftpipeAllocator *allocator);
void spDeleteContext(SoftpipeContext *context, SoftpipeAllocator *allocator);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* CW_SOFTPIPE_H */
