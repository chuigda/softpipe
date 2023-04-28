from matplotlib import pyplot as plt
import time

from data import Vec, Color
from matrix import rotate_x, rotate_y, translate, perspective
from deg2rad import deg2rad
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


def vs(inputs, uniform):
    return {
        'v_position': (uniform['projection'] * uniform['modelview']).mul_vec(inputs['v_position'].pad_to_4d()),
        'color': inputs['color']
    }


def fs(inputs, _):
    return inputs['color']


if __name__ == '__main__':
    uniform = {
        'projection': perspective(deg2rad(45.0), 1.0, 0.1, 200.0),
        'modelview': translate(0.0, 0.0, 0.0) * rotate_x(deg2rad(45.0)) * rotate_y(deg2rad(45.0))
    }

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
        fs,
        uniform=uniform
    )
    end = time.time()
    print('softpipe_render took %.3f seconds' % (end - start))

    plt.imshow(framebuffer2pil(framebuffer, 600, 600))
    plt.show()
