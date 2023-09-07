#include <stdio.h>
#include "softpipe.h"
#include "sputnik.h"

#ifdef WIN32
#include <Windows.h>
#endif

typedef struct {
    float x, y;
    float r, g, b;
} Vertex;

typedef struct {
    float r, g, b;
} VSOutput;

static SoftpipeCoordinate vertexShader(
    void *vertexShaderOutput,
    const void *vertex,
    const void *uniformBlock,
    const SoftpipeVertexShaderGlobals *globals
) {
    const Vertex *v = (const Vertex *)vertex;
    VSOutput *out = (VSOutput *)vertexShaderOutput;
    out->r = v->r;
    out->g = v->g;
    out->b = v->b;

    return (SoftpipeCoordinate) {v->x, v->y, 0.0f, 1.0f};
}

static SoftpipeColor fragmentShader(
    const void *fragmentShaderInput,
    const void *uniformBlock,
    const SoftpipeFragmentShaderGlobals *globals
) {
    const VSOutput *in = (const VSOutput *)fragmentShaderInput;
    return (SoftpipeColor){in->r, in->g, in->b, 1.0f};
}

static void interpolate(void *interpolated,
                        SoftpipeCoordinate vertices[3],
                        void* vsOutput[3],
                        float w[3]) {
    VSOutput *out = (VSOutput *)interpolated;
    out->r = 0.0f;
    out->g = 0.0f;
    out->b = 0.0f;

    for (size_t i = 0; i < 3; i++) {
        out->r += w[i] * ((VSOutput *)vsOutput[i])->r;
        out->g += w[i] * ((VSOutput *)vsOutput[i])->g;
        out->b += w[i] * ((VSOutput *)vsOutput[i])->b;
    }
}

#ifdef WIN32
int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpszCmdLine,
                   int nCmdShow)
#else
int main(void)
#endif
{
    SoftpipeFramebuffer *fb = spCreateFramebuffer(320, 200, NULL);
    if (!fb) {
        return 1;
    }

    Softpipe *softpipe = spCreateSoftpipe(
        sizeof(Vertex),
        sizeof(VSOutput),
        NULL
    );
    if (!softpipe) {
        spDeleteFramebuffer(fb, NULL);
        return 1;
    }

    spVertexShader(softpipe, vertexShader);
    spFragmentShader(softpipe, fragmentShader);
    spInterpolator(softpipe, interpolate);
    spDisable(softpipe, SP_BLENDING);
    spDisable(softpipe, SP_DEPTH_TEST);

    Vertex vertices[3] = {
        { 0.0f, 1.0f, 1.0f, 0.0f, 0.0f},
        {-1.0f, -1.0f, 0.0f, 1.0f, 0.0f},
        { 1.0f, -1.0f, 0.0f, 0.0f, 1.0f}
    };

    spRender(softpipe, fb, NULL, vertices, 3, NULL, 0);

    SPUTWindow *window = sputCreateWindow("SPUT Window", 320, 200);
    if (!window) {
        spDeleteSoftpipe(softpipe, NULL);
        spDeleteFramebuffer(fb, NULL);
        return 1;
    }

    if (!sputWindowDisplay(window, fb)) {
        sputDestroyWindow(window);
        spDeleteSoftpipe(softpipe, NULL);
        spDeleteFramebuffer(fb, NULL);
        return 1;
    }

    sputWindowMainLoop(window);

    sputDestroyWindow(window);
    spDeleteSoftpipe(softpipe, NULL);
    spDeleteFramebuffer(fb, NULL);

    return 0;
}

