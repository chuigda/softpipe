#include <Windows.h>
#include "softpipe.h"
#include "sput.h"

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

    return (SoftpipeCoordinate){v->x, v->y, 0.0f, 1.0f};
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
                        SoftpipeCoordinate (*const vertices)[3],
                        void* (*const vsOutput)[3],
                        float w[3]) {
    VSOutput *out = (VSOutput *)interpolated;
    for (size_t i = 0; i < 3; i++) {
        out->r += w[i] * ((VSOutput *)vsOutput[i])->r;
        out->g += w[i] * ((VSOutput *)vsOutput[i])->g;
        out->b += w[i] * ((VSOutput *)vsOutput[i])->b;
    }
}

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpszCmdLine,
                   int nCmdShow)
{
    SoftpipeFramebuffer *fb = spCreateFramebuffer(640, 480, NULL);
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
        { 0.0f, 0.5f, 1.0f, 0.0f, 0.0f},
        {-0.5f, -0.5f, 0.0f, 1.0f, 0.0f},
        { 0.5f, -0.5f, 0.0f, 0.0f, 1.0f}
    };

    spRender(softpipe, fb, NULL, vertices, 3, NULL, 0);

    SPUTWindow *window = sputCreateWindow("Softpipe", 640, 480);
    sputWindowDisplay(window, fb);

    sputWindowMainLoop(window);
    sputDestroyWindow(window);
}
