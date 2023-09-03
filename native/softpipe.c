#include "softpipe.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

static void* defaultAlloc(size_t size, size_t alignment) {
    (void)alignment;
    return malloc(size);
}

static void defaultFree(void* ptr) {
    free(ptr);
}

static SoftpipeAllocator g_DefaultAllocator = (SoftpipeAllocator) {
    defaultAlloc,
    defaultFree
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

    SoftpipeFramebuffer *fb = allocator->alloc(
        sizeof(SoftpipeFramebuffer) + width * height * 4, 1
    );
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

struct stDepthbuffer {
    size_t width;
    size_t height;
    float depthBuffer[0];
};

SoftpipeDepthbuffer *spCreateDepthbuffer(size_t width,
                                         size_t height,
                                         SoftpipeAllocator *allocator) {
    if (!allocator) {
        allocator = &g_DefaultAllocator;
    }

    SoftpipeDepthbuffer *db = allocator->alloc(
        sizeof(SoftpipeDepthbuffer) + width * height * sizeof(float), 1
    );
    if (!db) {
        return NULL;
    }

    db->width = width;
    db->height = height;
    memset(db->depthBuffer, 0, width * height * sizeof(float));
    return db;
}

void spDeleteDepthbuffer(SoftpipeDepthbuffer *db,
                         SoftpipeAllocator *allocator) {
    if (!allocator) {
        allocator = &g_DefaultAllocator;
    }

    allocator->free(db);
}

struct stSoftpipe {
    size_t vertexSize;
    size_t vsOutputSize;

    SoftpipeVertexShader vertexShader;
    SoftpipeFragmentShader fragmentShader;
    SoftpipeInterpolator interpolator;
    SoftpipeBlendFunc blendFunc;

    _Bool cullFace;
    _Bool depthTest;
    _Bool alphaTest;
    _Bool blending;

    SP_CONST void *uniformBlock;
    void *vsOutput[3];
    void *vsOutputInterpolated;
    uint64_t buffer[0];
};

Softpipe *spCreateSoftpipe(size_t vertexSize,
                           size_t vsOutputSize,
                           SoftpipeAllocator *allocator) {
    if (!allocator) {
        allocator = &g_DefaultAllocator;
    }

    size_t extraSize = vsOutputSize * 4;
    Softpipe *sp = allocator->alloc(sizeof(Softpipe) + extraSize, 8);
    if (!sp) {
        return NULL;
    }

    sp->vertexSize = vertexSize;
    sp->vsOutputSize = vsOutputSize;
    sp->vertexShader = NULL;
    sp->fragmentShader = NULL;
    sp->interpolator = NULL;
    sp->blendFunc = NULL;

    sp->cullFace = 0;
    sp->depthTest = 0;
    sp->alphaTest = 0;
    sp->blending = 0;

    sp->uniformBlock = NULL;
    sp->vsOutput[0] = sp->buffer;
    sp->vsOutput[1] = sp->buffer + vsOutputSize;
    sp->vsOutput[2] = sp->buffer + vsOutputSize * 2;
    sp->vsOutputInterpolated = sp->buffer + vsOutputSize * 3;

    return sp;
}

void spDeleteSoftpipe(Softpipe *sp, SoftpipeAllocator *allocator) {
    if (!allocator) {
        allocator = &g_DefaultAllocator;
    }

    allocator->free(sp);
}

void spUniform(Softpipe *sp, SP_CONST void *uniformBlock) {
    sp->uniformBlock = uniformBlock;
}

void spVertexShader(Softpipe *sp, SoftpipeVertexShader vertexShader) {
    sp->vertexShader = vertexShader;
}

void spFragmentShader(Softpipe *sp,
                      SoftpipeFragmentShader fragmentShader) {
    sp->fragmentShader = fragmentShader;
}

void spInterpolator(Softpipe *sp, SoftpipeInterpolator interpolator) {
    sp->interpolator = interpolator;
}

void spBlendFunc(Softpipe *sp, SoftpipeBlendFunc blendFunc) {
    sp->blendFunc = blendFunc;
}

void spEnable(Softpipe *sp, uint32_t feature) {
    switch (feature) {
    case SP_CULL_FACE:
        sp->cullFace = 1;
        break;
    case SP_DEPTH_TEST:
        sp->depthTest = 1;
        break;
    case SP_ALPHA_TEST:
        sp->alphaTest = 1;
        break;
    case SP_BLENDING:
        sp->blending = 1;
        break;
    }
}

void spDisable(Softpipe *sp, uint32_t feature) {
    switch (feature) {
    case SP_CULL_FACE:
        sp->cullFace = 0;
        break;
    case SP_DEPTH_TEST:
        sp->depthTest = 0;
        break;
    case SP_ALPHA_TEST:
        sp->alphaTest = 0;
        break;
    case SP_BLENDING:
        sp->blending = 0;
        break;
    }
}

static inline void standardize(SoftpipeCoordinate *v) {
    v->x = v->x / v->t;
    v->y = v->y / v->t;
    v->z = v->z / v->t;
    v->t = 1.0f;
}

static inline float min3(float f1, float f2, float f3) {
    float min = f1;
    if (f2 < min) {
        min = f2;
    }
    if (f3 < min) {
        min = f3;
    }
    return min;
}

static inline float max3(float f1, float f2, float f3) {
    float max = f1;
    if (f2 > max) {
        max = f2;
    }
    if (f3 > max) {
        max = f3;
    }
    return max;
}

static void barycentric(SoftpipeCoordinate (*v)[3],
                        float (*w)[3],
                        SoftpipeCoordinate vi) {
    float denom = (v[1]->y - v[2]->y) * (v[0]->x - v[2]->x) +
                  (v[2]->x - v[1]->x) * (v[0]->y - v[2]->y);
    float w1 = ((v[1]->y - v[2]->y) * (vi.x - v[2]->x) +
                (v[2]->x - v[1]->x) * (vi.y - v[2]->y)) / denom;
    float w2 = ((v[2]->y - v[0]->y) * (vi.x - v[2]->x) +
                (v[0]->x - v[2]->x) * (vi.y - v[2]->y)) / denom;
}

void spRender(Softpipe *sp,
              SoftpipeFramebuffer *framebuffer,
              SoftpipeDepthbuffer *depthbuffer,
              SP_CONST void *vertices,
              size_t numVertices,
              SP_CONST size_t *indices,
              size_t numIndices) {
    assert(sp->vertexShader);
    assert(sp->fragmentShader);

    if (indices == NULL || numIndices == 0) {
        numIndices = numVertices;
    }

    SoftpipeVertexShaderGlobals vertexShaderGlobals;
    vertexShaderGlobals.numVertices = numVertices;
    for (size_t i = 0; i < numIndices; i += 3) {
        SoftpipeCoordinate v[3];
        for (size_t subIndex = 0; subIndex < 3; subIndex++) {
            size_t vertexId = indices ?
                indices[i + subIndex] :
                i + subIndex;
            void *vertex = (void *)(
                (uintptr_t)vertices + vertexId * sp->vertexSize
            );
            void *vsOutput = sp->vsOutput[subIndex];

            vertexShaderGlobals.vertexIndex = i + subIndex;
            v[subIndex] = sp->vertexShader(
                vertex,
                vsOutput,
                sp->uniformBlock,
                &vertexShaderGlobals
            );
            standardize(&v[subIndex]);
        }

        float xMin = min3(v[0].x, v[1].x, v[2].x);
        float yMin = min3(v[0].y, v[1].y, v[2].y);
        float xMax = max3(v[0].x, v[1].x, v[2].x);
        float yMax = max3(v[0].y, v[1].y, v[2].y);

        xMin = xMin < -1.0 ? -1.0 : xMin;
        yMin = yMin < -1.0 ? -1.0 : yMin;
        xMax = xMax > 1.0 ? 1.0 : xMax;
        yMax = yMax > 1.0 ? 1.0 : yMax;

        size_t xMinPix = (xMin + 1.0) * 0.5 * framebuffer->width;
        size_t yMinPix = (yMin + 1.0) * 0.5 * framebuffer->height;
        size_t xMaxPix = (xMax + 1.0) * 0.5 * framebuffer->width;
        size_t yMaxPix = (yMax + 1.0) * 0.5 * framebuffer->height;

        if (xMinPix > framebuffer->width || xMaxPix < 0 ||
            yMinPix > framebuffer->height || yMaxPix < 0) {
            continue;
        }

        if (xMaxPix > framebuffer->width) {
            xMaxPix = framebuffer->width;
        }

        if (yMaxPix > framebuffer->height) {
            yMaxPix = framebuffer->height;
        }

        for (size_t yPix = yMinPix; yPix < yMaxPix; yPix++) {
            for (size_t xPix = xMinPix; xPix < xMaxPix; xPix++) {
                float x = (float)xPix / framebuffer->width * 2.0 - 1.0;
                float y = (float)yPix / framebuffer->height * 2.0 - 1.0;

                SoftpipeCoordinate vi =
                    (SoftpipeCoordinate) { x, y, 0.0f, 1.0f };
                float w[3];
                barycentric(&v, &w, vi);
                if (w[0] < 0.0f || w[1] < 0.0f || w[2] < 0.0f) {
                    continue;
                }

                vi.z = v[0].z * w[0] + v[1].z * w[1] + v[2].z * w[2];

                size_t linearCoord = yPix * framebuffer->width + xPix;
                if (sp->depthTest) {
                    if (vi.z < depthbuffer->depthBuffer[linearCoord]) {
                        continue;
                    }
                    depthbuffer->depthBuffer[linearCoord] = vi.z;
                }

                sp->interpolator(
                    sp->vsOutputInterpolated,
                    v,
                    sp->vsOutput,
                    w
                );

                SoftpipeFragmentShaderGlobals fragmentShaderGlobals;
                fragmentShaderGlobals.fragmentX = xPix;
                fragmentShaderGlobals.fragmentY = yPix;
                fragmentShaderGlobals.framebufferWidth =
                    framebuffer->width;
                fragmentShaderGlobals.framebufferHeight =
                    framebuffer->height;
                fragmentShaderGlobals.isFrontFacing = true;
                SoftpipeColor color = sp->fragmentShader(
                    sp->vsOutputInterpolated,
                    sp->uniformBlock,
                    &fragmentShaderGlobals
                );

                if (sp->blending) {
                    SoftpipeColor oldColor = {
                        framebuffer->colorBuffer[linearCoord * 4 + 0],
                        framebuffer->colorBuffer[linearCoord * 4 + 1],
                        framebuffer->colorBuffer[linearCoord * 4 + 2],
                        framebuffer->colorBuffer[linearCoord * 4 + 3]
                    };
                    color = sp->blendFunc(oldColor, color);
                }

                framebuffer->colorBuffer[linearCoord * 4 + 0] = color.r;
                framebuffer->colorBuffer[linearCoord * 4 + 1] = color.g;
                framebuffer->colorBuffer[linearCoord * 4 + 2] = color.b;
                framebuffer->colorBuffer[linearCoord * 4 + 3] = color.a;
            }
        }
    }
}
