#ifndef CW_SOFTPIPE_H
#define CW_SOFTPIPE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
#define SP_CONST const
#else
#define SP_CONST
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    float x, y, z, t;
} SoftpipeCoordinate;

typedef struct {
    float r, g, b, a;
} SoftpipeColor;

typedef struct {
    void *(*alloc)(size_t size, size_t alignment);
    void (*free)(void *ptr);
} SoftpipeAllocator;

typedef struct stFramebuffer SoftpipeFramebuffer;

SoftpipeFramebuffer *spCreateFramebuffer(size_t width,
                                         size_t height,
                                         SoftpipeAllocator *allocator);

void spClearFramebuffer(SoftpipeFramebuffer *fb, SoftpipeColor color);

void spGetFramebufferSize(SoftpipeFramebuffer *fb,
                          size_t *width,
                          size_t *height);

void spReadPixelRGBA32(SoftpipeFramebuffer *fb, uint8_t *buffer);
void spReadPixelRGB24(SoftpipeFramebuffer *fb, uint8_t *buffer);

SoftpipeColor spTexture(SoftpipeFramebuffer *fb, float u, float v);

void spDeleteFramebuffer(SoftpipeFramebuffer *fb,
                         SoftpipeAllocator *allocator);

typedef struct stDepthbuffer SoftpipeDepthbuffer;

SoftpipeDepthbuffer *spCreateDepthBuffer(size_t width,
                                         size_t height,
                                         SoftpipeAllocator *allocator);

void spClearDepthBuffer(SoftpipeDepthbuffer *db);

void spDeleteDepthBuffer(SoftpipeDepthbuffer *db,
                         SoftpipeAllocator *allocator);

typedef struct {
    size_t numVertices;
    size_t vertexIndex;
} SoftpipeVertexShaderGlobals;

typedef struct {
    size_t framebufferWidth;
    size_t framebufferHeight;
    size_t fragmentX;
    size_t fragmentY;
    bool isFrontFacing;
} SoftpipeFragmentShaderGlobals;

typedef SoftpipeCoordinate (*SoftpipeVertexShader)(
    void *veretxShaderOutput,
    SP_CONST void *vertex,
    SP_CONST void *uniformBlock,
    SP_CONST SoftpipeVertexShaderGlobals *globals);

typedef SoftpipeColor (*SoftpipeFragmentShader)(
    SP_CONST void *fragmentShaderInput,
    SP_CONST void *uniformBlock,
    SP_CONST SoftpipeFragmentShaderGlobals *globals);

typedef void (*SoftpipeInterpolator)(
    void *interpolated,
    SoftpipeCoordinate vertices[3],
    void* vsOutput[3],
    float w[3]);

typedef SoftpipeColor (*SoftpipeBlendFunc)(
    SoftpipeColor src,
    SoftpipeColor dst);

typedef struct stSoftpipe Softpipe;

Softpipe *spCreateSoftpipe(size_t vertexSize,
                           size_t vsOutputSize,
                           SoftpipeAllocator *allocator);
void spDeleteSoftpipe(Softpipe *softpipe, SoftpipeAllocator *allocator);

void spUniform(Softpipe *softpipe, SP_CONST void *uniformBlock);
void spVertexShader(Softpipe *softpipe,
                    SoftpipeVertexShader vertexShader);
void spFragmentShader(Softpipe *softpipe,
                      SoftpipeFragmentShader fragmentShader);
void spInterpolator(Softpipe *softpipe,
                    SoftpipeInterpolator interpolator);
void spBlendFunc(Softpipe *softpipe, SoftpipeBlendFunc blendFunc);

#define SP_CULL_FACE 0x1235
#define SP_DEPTH_TEST 0x1236
#define SP_ALPHA_TEST 0x1237
#define SP_BLENDING 0x1238
#define SP_VERTEX_SHADER_MT_SAFE 0x1239
#define SP_FRAGMENT_SHADER_MT_SAFE 0x123A
#define SP_INTERPOLATOR_MT_SAFE 0x123B
#define SP_BLEND_FUNC_MT_SAFE 0x123C

void spEnable(Softpipe *softpipe, uint32_t feature);
void spDisable(Softpipe *softpipe, uint32_t feature);

void spRender(Softpipe *softpipe,
              SoftpipeFramebuffer *framebuffer,
              SoftpipeDepthbuffer *depthbuffer,
              SP_CONST void *vertices,
              size_t numVertices,
              SP_CONST size_t *indices,
              size_t numIndices);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* CW_SOFTPIPE_H */
