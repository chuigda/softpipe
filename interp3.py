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
    if isinstance(value, Vec):
        return value.num_mul(num)
    else:
        return num * value


def interp3(v_outputs1, v_outputs2, v_outputs3, v, user_interp):
    v1 = v_outputs1['v_position']
    v2 = v_outputs2['v_position']
    v3 = v_outputs3['v_position']

    w1, w2, w3 = __barycentric(v1, v2, v3, v)
    if w1 < 0 or w2 < 0 or w3 < 0:
        return None

    if user_interp is not None:
        return user_interp(w1, w2, w3, v_outputs1, v_outputs2, v_outputs3)
    else:
        result = dict()
        for k in v_outputs1.keys():
            value1 = v_outputs1[k]
            value2 = v_outputs2[k]
            value3 = v_outputs3[k]

            result[k] = __generic_num_mul(w1, value1) \
                + __generic_num_mul(w2, value2) \
                + __generic_num_mul(w3, value3)

        return result
