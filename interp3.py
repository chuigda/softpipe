# https://codeplea.com/triangular-interpolation
from functools import reduce

from data import *


def __barycentric(v1, v2, v3, v):
    denom = (v2.y - v3.y) * (v1.x - v3.x) + (v3.x - v2.x) * (v1.y - v3.y)
    w1 = ((v2.y - v3.y) * (v.x - v3.x) + (v3.x - v2.x) * (v.y - v3.y)) / denom
    w2 = ((v3.y - v1.y) * (v.x - v3.x) + (v1.x - v3.x) * (v.y - v3.y)) / denom
    w3 = 1.0 - w1 - w2
    return w1, w2, w3


def __generic_num_mul(num, value):
    if isinstance(value, Vec2) \
            or isinstance(value, Vec3) \
            or isinstance(value, Vec4) \
            or isinstance(value, Color):
        return num_mul_vec(num, value)
    elif isinstance(value, int) or isinstance(value, float):
        return num * value
    else:
        assert False


def __generic_sum(values):
    assert len(values) > 0
    if isinstance(values[0], Vec2):
        return reduce(vec_add, values, Vec2(0.0, 0.0))
    elif isinstance(values[0], Vec3):
        return reduce(vec_add, values, Vec3(0.0, 0.0, 0.0))
    elif isinstance(values[0], Vec4):
        return reduce(vec_add, values, Vec4(0.0, 0.0, 0.0, 0.0))
    elif isinstance(values[0], Color):
        return reduce(vec_add, values, Color(0.0, 0.0, 0.0, 0.0))
    elif isinstance(values[0], int) or isinstance(values[0], float):
        return reduce(lambda x, y: x + y, values, 0.0)
    else:
        assert False


def interp3(v_outputs1, v_outputs2, v_outputs3, v):
    v1 = v_outputs1['v_position']
    v2 = v_outputs2['v_position']
    v3 = v_outputs3['v_position']

    w1, w2, w3 = __barycentric(v1, v2, v3, v)
    if w1 < 0 or w2 < 0 or w3 < 0:
        return None

    result = dict()
    for k in v_outputs1.keys():
        value1 = v_outputs1[k]
        value2 = v_outputs2[k]
        value3 = v_outputs3[k]

        result[k] = __generic_sum([
            __generic_num_mul(w1, value1),
            __generic_num_mul(w2, value2),
            __generic_num_mul(w3, value3),
        ])

    return result
