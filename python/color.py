import attr


@attr.s
class RGB:
    r = attr.ib(default=0)
    g = attr.ib(default=0)
    b = attr.ib(default=0)


@attr.s
class HSV:
    h = attr.ib(default=0)
    s = attr.ib(default=0)
    v = attr.ib(default=0)


VALS_R = [0, 3, 13, 29, 52, 81, 117, 159, 208]
VALS_G = [0, 3, 14, 31, 56, 87, 126, 171, 224]
VALS_B = [0, 3, 15, 33, 60, 93, 135, 183, 240]
WHITE_R = 38
WHITE_G = 41
WHITE_B = 44

# FRAC8 = [0, 1, 4, 9, 16, 25, 36, 49, 64]
FRAC8 = [0, 1, 2, 5, 8, 13, 18, 25, 32]

def norm32(x, d):
    if x == 0:
        return 0
    if x >= d:
        return 32

    while d < 32:
        d = d << 1
        x = x << 1

    v = x
    t = d
    r = 0

    t = t >> 1
    if x >= t:
        v -= t
        r += 16

    t = t >> 1
    if x >= t:
        v -= t
        r += 8

    t = t >> 1
    if x >= t:
        v -= t
        r += 4

    t = t >> 1
    if x >= t:
        v -= t
        r += 2

    t = t >> 1
    if x >= t:
        v -= t
        r += 1

    return r

def interp32(s, e, x):
    if s == e:
        return s
    if x == 0:
        return s
    if x >= 32:
        return e

    d = e - s if s < e else s - e
    r = 0

    d = d >> 1
    if x & 0x10:
        r += d

    d = d + 1
    d = d >> 1
    if x & 0x08:
        r += d

    d = d >> 1
    if x & 0x04:
        r += d

    d = d + 1
    d = d >> 1
    if x & 0x02:
        r += d

    d = d >> 1
    if x & 0x01:
        r += d

    return s + r if s < e else s - r


def hue_color(hue):
    while hue >= 24:
        hue -= 24

    if hue < 8:
        return RGB(VALS_R[8 - hue], VALS_G[hue], 0)
    elif hue < 16:
        return RGB(0, VALS_G[16 - hue], VALS_B[hue - 8])
    else:
        return RGB(VALS_R[hue - 16], 0, VALS_B[24 - hue])


def hue_chan(y, s, e):
    f = FRAC8[y]
    r = interp32(s, e, f)
    if r == 0 and y > 0 and (s > 0 or e > 0):
        r = 1

    return r


def sat_chan(s, c, w):
    if c > w:
        s = 8 - s
        f = FRAC8[s]
        f = 32 - f
    else:
        f = FRAC8[s]

    r = interp32(c, w, f)
    if r == 0 and s > 0:
        r = 1

    return r


def val_chan(v, c):
    r = interp32(c, 0, FRAC8[v])
    if r == 0 and v < 8 and c > 0:
        r = 1

    return r


def unhsv(hsv, rot=0):
    rgb = RGB()
    hsv.h += rot
    while hsv.h >= 192:
        hsv.h -= 192

    x = hsv.h >> 3
    s = hue_color(x)
    e = hue_color(x + 1)

    y = hsv.h & 0x7
    rgb.r = hue_chan(y, s.r, e.r)
    rgb.g = hue_chan(y, s.g, e.g)
    rgb.b = hue_chan(y, s.b, e.b)

    rgb.r = sat_chan(hsv.s, rgb.r, WHITE_R)
    rgb.g = sat_chan(hsv.s, rgb.g, WHITE_G)
    rgb.b = sat_chan(hsv.s, rgb.b, WHITE_B)

    rgb.r = val_chan(hsv.v, rgb.r)
    rgb.g = val_chan(hsv.v, rgb.g)
    rgb.b = val_chan(hsv.v, rgb.b)

    return rgb


def blend(c0, c1, x, d, rot=0):
    s = unhsv(c0, rot)
    e = unhsv(c1, rot)
    f = norm32(x, d)
    return RGB(
        interp32(s.r, e.r, f),
        interp32(s.g, e.g, f),
        interp32(s.b, e.b, f),
    )
