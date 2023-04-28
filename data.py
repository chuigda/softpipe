from math import sqrt


class Vec:
    def __init__(self, x, y, *varargsin):
        self._x = x
        self._y = y
        self._z = varargsin[0] if len(varargsin) >= 1 else None
        self._t = varargsin[1] if len(varargsin) >= 2 else None

    @property
    def x(self):
        return self._x
    
    @property
    def y(self):
        return self._y
    
    @property
    def z(self):
        return self._z
    
    @property
    def t(self):
        return self._t
    
    @property
    def r(self):
        return self._x
    
    @property
    def g(self):
        return self._y
    
    @property
    def b(self):
        return self._z
    
    @property
    def a(self):
        return self._t
    
    @x.setter
    def set_x(self, value):
        self._x = value

    @y.setter
    def set_y(self, value):
        self._y = value
    
    @z.setter
    def set_z(self, value):
        self._z = value
    
    @t.setter
    def set_t(self, value):
        self._t = value
    
    @r.setter
    def set_r(self, value):
        self._x = value
    
    @g.setter
    def set_g(self, value):
        self._y = value
    
    @b.setter
    def set_b(self, value):
        self._z = value
    
    @a.setter
    def set_a(self, value):
        self._t = value

    def __getitem__(self, key):
        if key == 0:
            return self._x
        elif key == 1:
            return self._y
        elif key == 2:
            return self._z
        elif key == 3:
            return self._t
        else:
            raise IndexError("Index out of bounds")

    def sum(self):
        return self._x \
            + self._y \
            + (self._z if self._z is not None else 0.0) \
            + (self._t if self._t is not None else 0.0)

    def __str__(self):
        return "Vec({}, {}, {}, {})".format(self._x, self._y, self._z, self._t)
    
    def __add__(self, other):
        return Vec(
            self._x + other._x,
            self._y + other._y,
            self._z + other._z if self._z is not None and other._z is not None else None,
            self._t + other._t if self._t is not None and other._t is not None else None
        )
    
    def __sub__(self, other):
        return Vec(
            self._x - other._x,
            self._y - other._y,
            self._z - other._z if self._z is not None and other._z is not None else None,
            self._t - other._t if self._t is not None and other._t is not None else None
        )

    def __mul__(self, other):
        return Vec(
            self._x * other._x,
            self._y * other._y,
            self._z * other._z if self._z is not None and other._z is not None else None,
            self._t * other._t if self._t is not None and other._t is not None else None
        )
    
    def num_mul(self, other):
        return Vec(
            self._x * other,
            self._y * other,
            self._z * other if self._z is not None else None,
            self._t * other if self._t is not None else None
        )

    def dot(self, other):
        return (self * other).sum()
    
    def cross(self, other):
        assert self._z is not None and other._z is not None, "Cross product is only defined for 3D vectors"
        return Vec(
            self._y * other._z - self._z * other._y,
            self._z * other._x - self._x * other._z,
            self._x * other._y - self._y * other._x
        )
    
    def pad_to_4d(self, value=1.0):
        return Vec(self._x, self._y, self._z if self._z else 0.0, value)


Color = Vec
