# https://stackoverflow.com/questions/2049582/how-to-determine-if-a-point-is-in-a-2d-triangle

def __sign(v1, v2, v3):
    return (v1.x - v3.x) * (v2.y - v3.y) - (v2.x - v3.x) * (v1.y - v3.y)


def isinside(v1, v2, v3, v):
    d1 = __sign(v, v1, v2)
    d2 = __sign(v, v2, v3)
    d3 = __sign(v, v3, v1)

    has_negative = d1 < 0 or d2 < 0 or d3 < 0
    has_positive = d1 > 0 or d2 > 0 or d3 > 0

    return not (has_negative and has_positive)
