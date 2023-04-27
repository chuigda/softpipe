from collections import namedtuple
from math import sqrt


Vec2 = namedtuple('Vec2', ('x', 'y'))
Vec3 = namedtuple('Vec3', ('x', 'y', 'z'))
Vec4 = namedtuple('Vec4', ('x', 'y', 'z', 't'))
Color = namedtuple('Color', ('r', 'g', 'b', 'a'))


def __square(x):
    return x * x


def distance_vec2(v1, v2):
    return sqrt(__square(v2.x - v1.x) + __square(v2.y - v1.y))


def __apply_elemwise(v, f):
    if isinstance(v, Vec2):
        return Vec2(f(v.x), f(v.y))
    elif isinstance(v, Vec3):
        return Vec3(f(v.x), f(v.y), f(v.z))
    elif isinstance(v, Vec4):
        return Vec4(f(v.x), f(v.y), f(v.z), f(v.t))
    elif isinstance(v, Color):
        return Color(f(v.r), f(v.g), f(v.b), f(v.a))
    else:
        assert False


def __apply_elemwise2(v1, v2, f):
    assert v1.__class__ == v2.__class__
    if isinstance(v1, Vec2):
        return Vec2(f(v1.x, v2.x), f(v1.y, v2.y))
    elif isinstance(v1, Vec3):
        return Vec3(f(v1.x, v2.x), f(v1.y, v2.y), f(v1.z, v2.z))
    elif isinstance(v1, Vec4):
        return Vec4(f(v1.x, v2.x), f(v1.y, v2.y), f(v1.z, v2.z), f(v1.t, v2.t))
    elif isinstance(v1, Color):
        return Color(f(v1.r, v2.r), f(v1.g, v2.g), f(v1.b, v2.b), f(v1.a, v2.a))
    else:
        assert False


def vec_add(v1, v2):
    return __apply_elemwise2(v1, v2, lambda x, y: x + y)


def vec_sub(v1, v2):
    return __apply_elemwise2(v1, v2, lambda x, y: x - y)


def vec_mul(v1, v2):
    return __apply_elemwise2(v1, v2, lambda x, y: x * y)


def num_mul_vec(num, v):
    return __apply_elemwise(v, lambda x: x * num)


def vec_dot(v1, v2):
    v = vec_mul(v1, v2)
    if isinstance(v, Vec2):
        return v.x + v.y
    elif isinstance(v, Vec3):
        return v.x + v.y + v.z
    elif isinstance(v, Vec4):
        return v.x + v.y + v.z + v.t


def vec_cross(v1, v2):
    assert isinstance(v1, Vec3) and isinstance(v2, Vec3)
    return Vec3(
        v1.y * v2.z - v2.y * v1.z,
        v1.x * v2.z - v2.x * v1.z,
        v1.x * v2.y - v2.x * v1.y
    )
