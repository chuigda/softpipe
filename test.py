from matplotlib import pyplot as plt
import time

from data import Vec, Color
from softpipe import softpipe_render


def framebuffer2pil(framebuffer, width, height):
    image = []
    for y in range(height - 1, -1, -1):
        row = []
        for x in range(0, width):
            idx = y * width + x
            pixel = framebuffer.buffer[idx]
            row.append([pixel.r, pixel.g, pixel.b, pixel.a])
        image.append(row)
    return image


def vs(inputs, _):
    return inputs


def fs(inputs, _):
    return inputs['color']


if __name__ == '__main__':
    start = time.time()
    framebuffer = softpipe_render(
        (600, 600),
        [
            { 'v_position': Vec(1.0, 0.0, 0.1), 'color': Color(0.0, 0.0, 1.0, 1.0) },
            { 'v_position': Vec(0.0, -1.0, 0.1), 'color': Color(1.0, 0.0, 0.0, 1.0) },
            { 'v_position': Vec(1.0, -1.0, 0.1), 'color': Color(0.0, 1.0, 0.0, 1.0) },

            { 'v_position': Vec(0.0, 1.0, 0.0), 'color': Color(1.0, 0.0, 0.0, 1.0) },
            { 'v_position': Vec(-1.0, -1.0, 0.0), 'color': Color(0.0, 1.0, 0.0, 1.0) },
            { 'v_position': Vec(1.0, -1.0, 0.0), 'color': Color(0.0, 0.0, 1.0, 1.0) }
        ],
        vs,
        fs
    )
    end = time.time()
    print('softpipe_render took %.3f seconds' % (end - start))

    plt.imshow(framebuffer2pil(framebuffer, 600, 600))
    plt.show()
