from math import sin, cos, tan

from data import Vec


class Matrix:
    def __init__(self):
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
    result = Matrix()
    result[0][0] = 1.0
    result[1][1] = 1.0
    result[2][2] = 1.0
    result[3][3] = 1.0
    return result


def perspective(fov, aspect, near, far):
    result = Matrix()
    f = 1 / tan(fov / 2)
    result[0][0] = f / aspect
    result[1][1] = f
    result[2][2] = (near + far) / (near - far)
    result[2][3] = (2 * near * far) / (near - far)
    result[3][2] = -1.0
    return result


def rotate_x(angle):
    result = Matrix()
    result[0][0] = 1
    result[1][1] = cos(angle)
    result[1][2] = sin(angle)
    result[2][1] = -sin(angle)
    result[2][2] = cos(angle)
    result[3][3] = 1
    return result


def rotate_y(angle):
    result = Matrix()
    result[0][0] = cos(angle)
    result[0][2] = sin(angle)
    result[2][0] = -sin(angle)
    result[1][1] = 1
    result[2][2] = cos(angle)
    result[3][3] = 1
    return result


def rotate_z(angle):
    result = Matrix()
    result[0][0] = cos(angle)
    result[0][1] = sin(angle)
    result[1][0] = -sin(angle)
    result[1][1] = cos(angle)
    result[2][2] = 1
    result[3][3] = 1
    return result


def translate(x, y, z):
    result = Matrix()
    result[0][0] = 1
    result[1][1] = 1
    result[2][2] = 1
    result[3][3] = 1
    result[3][0] = x
    result[3][1] = y
    result[3][2] = z
    return result


def scale(x, y, z):
    result = Matrix()
    result[0][0] = x
    result[1][1] = y
    result[2][2] = z
    result[3][3] = 1
    return result
