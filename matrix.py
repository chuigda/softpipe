from math import sin, cos, tan

from data import Vec


class Matrix:
    def __init__(self, buffer=None):
        if buffer is not None:
            self._buffer = buffer
        else:
            self._buffer = [[0.0 for _ in range(4)] for _ in range(4)]

    def __getitem__(self, index):
        return self._buffer[index]
    
    def __setitem__(self, index, value):
        self._buffer[index] = value

    def __str__(self):
        return str(self._buffer)

    def __mul__(self, other):
        result = Matrix()
        for i in range(4):
            for j in range(4):
                result[i][j] = sum([self[i][k] * other[k][j] for k in range(4)])
        return result

    def num_mul(self, other):
        result = Matrix()
        for i in range(4):
            for j in range(4):
                result[i][j] = self[i][j] * other
        return result
    
    def mul_vec(self, other):
        result = []
        for i in range(4):
            result.append(sum([self[i][k] * other[k] for k in range(4)]))
        return Vec(*result)


def identity():
    return Matrix(buffer=[
        [1.0, 0.0, 0.0, 0.0],
        [0.0, 1.0, 0.0, 0.0],
        [0.0, 0.0, 1.0, 0.0],
        [0.0, 0.0, 0.0, 1.0]
    ])


def perspective(fov, aspect, near, far):
    f = 1 / tan(fov / 2)
    d = near - far

    return Matrix([
        [f / aspect, 0.0,              0.0,                0.0],
        [       0.0,   f,              0.0,                0.0],
        [       0.0, 0.0, (far + near) / d, 2 * far * near / d],
        [       0.0, 0.0,             -1.0,                0.0]
    ])


def rotate_x(angle):
    cosine = cos(angle)
    sine = sin(angle)

    return Matrix([
        [1.0,    0.0,    0.0, 0.0],
        [0.0, cosine,  -sine, 0.0],
        [0.0,   sine, cosine, 0.0],
        [0.0,    0.0,    0.0, 1.0]
    ])


def rotate_y(angle):
    cosine = cos(angle)
    sine = sin(angle)

    return Matrix([
        [cosine, 0.0,   sine, 0.0],
        [0.0,    1.0,    0.0, 0.0],
        [-sine,  0.0, cosine, 0.0],
        [0.0,    0.0,    0.0, 1.0]
    ])


def rotate_z(angle):
    cosine = cos(angle)
    sine = sin(angle)

    return Matrix([
        [cosine,  -sine, 0.0, 0.0],
        [  sine, cosine, 0.0, 0.0],
        [   0.0,    0.0, 1.0, 0.0],
        [   0.0,    0.0, 0.0, 1.0]
    ])


def translate(x, y, z):
    return Matrix([
        [1.0, 0.0, 0.0,   x],
        [0.0, 1.0, 0.0,   y],
        [0.0, 0.0, 1.0,   z],
        [0.0, 0.0, 0.0, 1.0]
    ])


def scale(x, y, z):
    return Matrix([
        [  x, 0.0, 0.0, 0.0],
        [0.0,   y, 0.0, 0.0],
        [0.0, 0.0,   z, 0.0],
        [0.0, 0.0, 0.0, 1.0]
    ])
