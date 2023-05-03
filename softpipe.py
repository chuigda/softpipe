from sys import float_info
from math import floor, ceil

from data import *
from interp3 import interp3


class FrameBuffer:
    def __init__(self, width, height, clear_color):
        self.width = width
        self.height = height
        self.buffer = [clear_color] * (width * height)

    def put_pixel(self, x, y, pixel, blend_func):
        assert x < self.width and y < self.height
        idx = y * self.width + x
        if blend_func is not None:
            src = self.buffer[idx]
            self.buffer[idx] = blend_func(src, pixel)
        else:
            self.buffer[idx] = pixel


class DepthBuffer:
    def __init__(self, width, height):
        self.width = width
        self.height = height
        self.buffer = [float_info.max] * (width * height)

    def depth_test(self, x, y, depth):
        assert x < self.width and y < self.height
        idx = y * self.width + x
        src = self.buffer[idx]
        if depth <= src:
            self.buffer[idx] = depth
            return True
        else:
            return False


def softpipe_render(viewport,
                    points,
                    vs,
                    fs,
                    uniform=None,
                    user_interp=None,
                    depth_test=True,
                    blend_func=None,
                    clear_color=None):
    width, height = viewport

    assert len(points) % 3 == 0
    assert width > 0
    assert height > 0

    frame_buffer = FrameBuffer(width, height, clear_color)
    if depth_test:
        depth_buffer = DepthBuffer(width, height)
    else:
        depth_buffer = None

    vs_outputs = [vs(point, uniform) for point in points]
    for vs_output in vs_outputs:
        vs_output['v_position'] = vs_output['v_position'].set_4d_to_1()

    half_width = width / 2.0
    half_height = height / 2.0

    for i in range(0, len(vs_outputs), 3):
        output1 = vs_outputs[i]
        output2 = vs_outputs[i + 1]
        output3 = vs_outputs[i + 2]

        position0 = output1['v_position']
        position1 = output2['v_position']
        position2 = output3['v_position']

        x_min = min([position0.x, position1.x, position2.x])
        x_max = max([position0.x, position1.x, position2.x])
        y_min = min([position0.y, position1.y, position2.y])
        y_max = max([position0.y, position1.y, position2.y])

        x_min_pix = floor(x_min * width + half_width)
        x_max_pix = ceil(x_max * width + half_width)
        y_min_pix = floor((y_min + 1.0) * half_height)
        y_max_pix = ceil((y_max + 1.0) * half_height)

        for y in range(y_min_pix if y_min_pix > 0 else 0, \
                       y_max_pix if y_max_pix < height else height):
            y_norm = (y - half_height) / half_height
            for x in range(x_min_pix if x_min_pix > 0 else 0, \
                           x_max_pix if x_max_pix < width else width):
                x_norm = (x - half_width) / half_width

                output = interp3(output1, output2, output3, Vec(x_norm, y_norm), user_interp)
                if output is None:
                    continue

                if depth_buffer is not None:
                    if not depth_buffer.depth_test(x, y, output['v_position'].z):
                        continue

                frame_buffer.put_pixel(x, y, fs(output, uniform), blend_func)

    return frame_buffer
