import attr
import copy


class Hue:
    RED = 0
    ORANGE = 16
    YELLOW = 32
    LEMON = 48
    GREEN = 64
    SEA = 80
    CYAN = 96
    SKY = 112
    BLUE = 128
    PURPLE = 144
    MAGENTA = 160
    FUSHIA = 176


def dbl(x):
    return int(x * 2)


class ChangeMode:
    OFF = 0
    THROUGH = 1
    THROUGH_ALT = 5
    OVER = 3
    OVER_ALT = 7


class ColorMode:
    BASIC = 0
    GROUP = 1
    REVERSE = 2
    SHIFT = 3


class Drop:
    ONE = 0
    ALL = 1


class RotDir:
    CLOCK = 0
    COUNTERCLOCK = 1
    CLOCK_ALT = 2
    COUNTERCLOCK_ALT = 3


class EffectDir:
    TO = 0
    FROM = 1
    TO_ALT = 2
    FROM_ALT = 3


class DimMode:
    OFF = 0
    FADE_IN = 1
    FADE_OUT = 2
    PULSE = 3


@attr.s
class HSV:
    h = attr.ib(default=0)
    s = attr.ib(default=0)
    v = attr.ib(default=0)

    def to_array(self):
        return [self.h, self.s, self.v]


@attr.s
class Color:
    typ_ = attr.ib(default=0)
    numc = attr.ib(default=1)
    dir_ = attr.ib(default=0)
    timer = attr.ib(default=[0, 0, 0, 0])
    colors0 = attr.ib(default=HSV())
    colors1 = attr.ib(default=HSV())
    colors2 = attr.ib(default=HSV())
    colors3 = attr.ib(default=HSV())

    def to_array(self):
        return [
            self.typ_,
            self.numc,
            self.dir_,
            self.timer[0],
            self.timer[1],
            self.timer[2],
            self.timer[3],
            0, 0, 0, 0, 0,
        ] \
            + self.colors0.to_array() \
            + self.colors1.to_array() \
            + self.colors2.to_array() \
            + self.colors3.to_array()


class ColorStatic(Color):
    @classmethod
    def new(cls, color)
        return cls(typ_=0, colors0=color)


class ColorCompound(Color):
    @classmethod
    def new(cls, colors):
        rtn = cls(typ_=1, numc=len(colors), colors0=colors[0], colors1=colors[1])
        if len(colors) > 1: rtn.colors2 = colors[2]
        if len(colors) > 2: rtn.colors3 = colors[3]
        return rtn


class ColorReactive(Color):
    @classmethod
    def new(cls, colors):
        rtn = cls(typ_=2, colors0=colors[0], colors1=colors[1])
        if len(colors) > 1: rtn.colors2 = colors[2]
        if len(colors) > 2: rtn.colors3 = colors[3]
        return rtn


class ColorMorph(Color):
    @classmethod
    def new(cls, colors, timer):
        rtn = cls(typ_=3, numc=len(colors), colors0=colors[0], colors1=colors[1])
        if len(colors) > 1: rtn.colors2 = colors[2]
        if len(colors) > 2: rtn.colors3 = colors[3]
        rtn.timers0 = timer[0]
        if len(timer) > 1: rtn.timers1 = timer[1]
        if len(timer) > 2: rtn.timers2 = timer[2]
        if len(timer) > 3: rtn.timers3 = timer[3]
        return rtn


class ColorStreak(Color):
    @classmethod
    def new(cls, colors, rev=False):
        rtn = cls(typ_=4, numc=len(colors), colors0=colors[0], colors1=colors[1])
        if len(colors) > 1: rtn.colors2 = colors[2]
        if len(colors) > 2: rtn.colors3 = colors[3]
        if rev: rtn.dir_ = 1
        return rtn


class ColorRotate(Color):
    """
    timer is how long rotation takes in seconds
    delta is how far
    """
    @classmethod
    def new(cls, color, timer, delta, rev=False, alt=False):
        rtn = cls(typ_=5, colors0=color)
        rtn.colors1 = copy.deepcopy(color)
        rtn.dir_ = 2 if alt else 0
        if rev:
            rtn.dir_ += 1
            rtn.colors1.h -= delta
        else:
            rtn.colors1.h += delta
        rtn.colors1.h %= 192
        rtn.timers0 = dbl(timer[0])
        if len(timer) > 1: rtn.timers1 = dbl(timer[1])
        if len(timer) > 2: rtn.timers2 = dbl(timer[2])
        if len(timer) > 3: rtn.timers3 = dbl(timer[3])
        return rtn


class ColorRainbow(Color):
    """
    delta is for the timer, gives 4 levels of rainbow delta
    rev is clockwise if false, counterclockwise if true
    into is to color for true, from color if false
    """
    @classmethod
    def new(cls, color, delta=[0, 0, 0, 0], rev=False, into=False):
        d = 0
        if rev: d &= 0x1
        if into: d &= 0x2
        return cls(typ_=6, colors0=color, shift_delta=delta, dir_=d)


@attr.s
class Segment:
    numc = attr.ib(default=1)
    change_mode = attr.ib(default=0)
    color_mode = attr.ib(default=0)
    dim_mode = attr.ib(default=0)
    cmod1 = attr.ib(default=0)
    cmod2 = attr.ib(default=0)
    strobe = attr.ib(default=[0, 0, 0, 0])
    strobe_end = attr.ib(default=[0, 0, 0, 0])
    blank = attr.ib(default=[0, 0, 0, 0])
    blank_end = attr.ib(default=[0, 0, 0, 0])
    gap = attr.ib(default=[0, 0, 0, 0])
    strobe_count = attr.ib(default=[1, 1, 1, 1])
    repeat_count = attr.ib(default=[1, 1, 1, 1])
    change_count = attr.ib(default=[1, 1, 1, 1])
    shift_delta = attr.ib(default=[0, 0, 0, 0])
    palette0 = attr.ib(default=Color())
    palette1 = attr.ib(default=Color())
    palette2 = attr.ib(default=Color())
    palette3 = attr.ib(default=Color())
    palette4 = attr.ib(default=Color())
    palette5 = attr.ib(default=Color())
    palette6 = attr.ib(default=Color())
    palette7 = attr.ib(default=Color())

    def to_array(self):
        return [
            self.numc,
            self.change_mode,
            self.color_mode,
            self.dim_mode,
            self.cmod1,
            self.cmod2,
            self.strobe[0],
            self.strobe[1],
            self.strobe[2],
            self.strobe[3],
            self.strobe_end[0],
            self.strobe_end[1],
            self.strobe_end[2],
            self.strobe_end[3],
            self.blank[0],
            self.blank[1],
            self.blank[2],
            self.blank[3],
            self.blank_end[0],
            self.blank_end[1],
            self.blank_end[2],
            self.blank_end[3],
            self.gap[0],
            self.gap[1],
            self.gap[2],
            self.gap[3],
            self.strobe_count[0],
            self.strobe_count[1],
            self.strobe_count[2],
            self.strobe_count[3],
            self.repeat_count[0],
            self.repeat_count[1],
            self.repeat_count[2],
            self.repeat_count[3],
            self.change_count[0],
            self.change_count[1],
            self.change_count[2],
            self.change_count[3],
            self.shift_delta[0],
            self.shift_delta[1],
            self.shift_delta[2],
            self.shift_delta[3],
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        ] \
            + self.palette0.to_array() \
            + self.palette1.to_array() \
            + self.palette2.to_array() \
            + self.palette3.to_array() \
            + self.palette4.to_array() \
            + self.palette5.to_array() \
            + self.palette6.to_array() \
            + self.palette7.to_array()


@attr.s
class Mode:
    nums = attr.ib(default=1)
    numt = attr.ib(default=1)
    falloff = attr.ib(default=[8, 8, 8, 8])
    trigger = attr.ib(default=[0, 0, 0, 0])
    thresh = attr.ib(default=[0, 0, 0, 0])
    jump = attr.ib(default=[0, 0, 0])
    segment0 = attr.ib(default=Segment())
    segment1 = attr.ib(default=Segment())
    segment2 = attr.ib(default=Segment())
    segment3 = attr.ib(default=Segment())

    def to_array(self):
        return [
            self.nums,
            self.numt,
            self.falloff[0],
            self.falloff[1],
            self.falloff[2],
            self.falloff[3],
            self.trigger[0],
            self.trigger[1],
            self.trigger[2],
            self.trigger[3],
            self.thresh[0],
            self.thresh[1],
            self.thresh[2],
            self.thresh[3],
            self.jump[0],
            self.jump[1],
            self.jump[2],
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        ] \
            + self.segment0.to_array() \
            + self.segment1.to_array() \
            + self.segment2.to_array() \
            + self.segment3.to_array()
