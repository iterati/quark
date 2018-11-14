from modes import *

darkside = Mode(
    nums=2,
    numt=4,
    thresh=[0, 40, 60, 90],
    segment0=Segment(
        strobe=[1, 10, 5, 1],
        blank=[0, 0, 5, 9],
        gap=[0, 0, 5, 9],
        strobe_count=[3, 12, 12, 12],
        palette0=ColorRainbow.new(HSV(0, 0, 0), delta=[192, 192, 192, 192], rev=True),
    ),
    segment1=Segment(
        strobe=[1, 1, 1, 1],
        strobe_count=[60, 60, 0, 0],
        palette0=ColorHSV(HSV(0, 0, 8)),
    ),
)

sourcery = Mode(
    nums=2,
    numt=4,
    thresh=[5, 25, 60, 90],
    segment0=Segment(
        strobe=[15, 10, 10, 0],
        blank=[0, 0, 0, 10],
        gap=[1, 1, 1, 10],
        strobe_count=[5, 5, 5, 5],
        palette0=ColorReactive([HSV(0, 0, 8), HSV(0, 0, 8), HSV(0, 0, 7), HSV(0, 0, 6)]),
    ),
    segment1=Segment(
        strobe=[0, 5, 5, 5],
        gap=[1, 1, 1, 10],
        numc=3,
        palette0=ColorMorph([HSV(112, 0, 0), HSV(112, 3, 0)], [5, 5, 5, 5]),
        palette1=ColorMorph([HSV(144, 0, 0), HSV(144, 3, 0)], [6, 6, 6, 6]),
        palette2=ColorMorph([HSV(176, 0, 0), HSV(176, 3, 0)], [7, 7, 7, 7]),
    ),
)

bleeding_edge = Mode(
    nums=4,
    numt=4,
    thresh=[10, 20, 70, 90],
    segment0=Segment(
        dim_mode=DimMode.FADE_IN,
        color_mode=ColorMode.SHIFT,
        cmod1=RotDir.CLOCK_ALT,
        cmod2=EffectDir.TO,
        strobe=[5, 4, 3, 2],
        shift_delta=[0, 24, 48, 96],
        palette0=ColorRotate(HSV(128, 0, 0), delta=192, timer=[30, 30, 30, 30])
    ),
    segment1=Segment(
        strobe=[6, 4, 2, 1],
        palette0=ColorRotate(HSV(128, 0, 0), delta=192, timer=[30, 30, 30, 30])
    ),
    segment2=Segment(
        dim_mode=DimMode.FADE_OUT,
        color_mode=ColorMode.SHIFT,
        cmod1=RotDir.CLOCK_ALT,
        cmod2=EffectDir.FROM,
        shift_delta=[0, 24, 48, 96],
        strobe=[5, 4, 3, 2],
        palette0=ColorRotate(HSV(128, 0, 0), delta=192, timer=[30, 30, 30, 30])
    ),
    segment3=Segment(
        strobe=[1, 1, 1, 1],
        strobe_count=[30, 30, 25, 20],
        palette0=ColorStatic(HSV(0, 0, 8)),
    ),
)

trailshot = Mode(
    nums=2,
    numt=4,
    thresh=[20, 40, 60, 80],
    jump=[1, 1, 1],
    segment0=Segment(
        strobe=[10, 8, 5, 3],
        blank=[0, 15, 12, 10],
        strobe_count=[2, 2, 4, 6],
        gap=[20, 20, 20, 20],
        palette0=ColorRotate(HSV(80, 0, 0), delta=32, alt=True, timer=[20, 20, 20, 20]),
    ),
    segment1=Segment(
        strobe=[20, 15, 10, 5],
        gap=[20, 20, 20, 20],
        numc=3,
        palette0=ColorStatic(HSV(176, 0, 0)),
        palette1=ColorStatic(HSV(0, 0, 0)),
        palette2=ColorStatic(HSV(16, 0, 0)),
    ),
)

inside_out = Mode(
    nums=4,
    numt=1,
    thresh=[0, 0, 0, 0],
    segment0=Segment(
        change_mode=ChangeMode.OVER,
        change_count=[9, 9, 9, 9],
        strobe=[0, 0, 0, 0],
        strobe_end=[8, 8, 8, 8],
        numc=3,
        palette0=ColorRotate(HSV(160, 1, 0), delta=192, timer=[180, 180, 180, 180]),
        palette1=ColorRotate(HSV( 32, 1, 0), delta=192, timer=[180, 180, 180, 180]),
        palette2=ColorRotate(HSV( 96, 1, 0), delta=192, timer=[180, 180, 180, 180]),
    ),
    segment1=Segment(
        change_mode=ChangeMode.OVER,
        change_count=[9, 9, 9, 9],
        strobe_count=[2, 2, 2, 2],
        strobe=[8, 8, 8, 8],
        strobe_end=[0, 0, 0, 0],
        numc=3,
        palette0=ColorRotate(HSV( 96, 1, 0), delta=192, timer=[180, 180, 180, 180]),
        palette1=ColorRotate(HSV(160, 1, 0), delta=192, timer=[180, 180, 180, 180]),
        palette2=ColorRotate(HSV( 32, 1, 0), delta=192, timer=[180, 180, 180, 180]),
    ),
    segment2=Segment(
        change_mode=ChangeMode.OVER,
        change_count=[9, 9, 9, 9],
        strobe=[0, 0, 0, 0],
        strobe_end=[8, 8, 8, 8],
        numc=3,
        palette0=ColorRotate(HSV(160, 1, 0), delta=192, timer=[180, 180, 180, 180]),
        palette1=ColorRotate(HSV( 32, 1, 0), delta=192, timer=[180, 180, 180, 180]),
        palette2=ColorRotate(HSV( 96, 1, 0), delta=192, timer=[180, 180, 180, 180]),
    ),
    segment3=Segment(
        strobe=[1, 1, 1, 1],
        strobe_count=[32, 32, 32, 32],
        palette0=ColorStatic(HSV(0, 0, 8)),
    ),
)

paintbrush = Mode(
    nums=1,
    numt=3,
    thresh=[0, 10, 90, 0],
    jump=[1, 0, 0],
    segment0=Segment(
        color_mode=ColorMode.GROUP;
        cmod1=5,
        cmod2=Drop.ALL,
        strobe=[25, 24, 3, 3],
        gap=[0, 1, 22, 22],
        strobe_count=[1, 2, 5, 5],
        numc=5,
        palette0=ColorRotate(HSV(0, 0, 0), delta=192, timer=[60, 60, 60, 60], alt=True),
        palette1=ColorRotate(HSV(8, 0, 0), delta=192, timer=[60, 60, 60, 60], alt=True),
        palette2=ColorRotate(HSV(16, 0, 0), delta=192, timer=[60, 60, 60, 60], alt=True),
        palette3=ColorRotate(HSV(24, 0, 0), delta=192, timer=[60, 60, 60, 60], alt=True),
        palette4=ColorRotate(HSV(32, 0, 0), delta=192, timer=[60, 60, 60, 60], alt=True),
    ),
)

waveform = Mode(
    nums=1,
    numt=4,
    segment0=Segment(
        change_mode=ChangeMode.THROUGH_ALT,
        strobe_count=[4, 8, 16, 32],
        strobe=[16, 10, 5, 3],
        strobe_end=[16, 10, 5, 3],
        blank=[32, 16, 8, 0],
        blank_end=[32, 32, 32, 32],
        gap=[32, 16, 8, 0],
        numc=4,
        palette0=ColorStatic(HSV(72, 2, 0)),
        palette1=ColorStatic(HSV(88, 2, 0)),
        palette2=ColorStatic(HSV(136, 2, 0)),
        palette3=ColorStatic(HSV(152, 2, 0)),
    ),
)

arrowhead = Mode(
    nums=3,
    numt=1,
    segment0=Segment(
        change_mode=ChangeMode.THROUGH,
        strobe_count=[3, 3, 3, 3],
        strobe=[2, 2, 2, 2],
        strobe_end=[6, 6, 6, 6],
        blank=[8, 8, 8, 8],
        blank_end=[8, 8, 8, 8],
        gap=[8, 8, 8, 8],
        numc=2,
        palette0=ColorRainbow(HSV(0, 0, 0), delta=[32, 32, 32, 32], into=True),
        palette1=ColorRainbow(HSV(24, 0, 0), delta=[32, 32, 32, 32], into=True),
        palette2=ColorRainbow(HSV(48, 0, 0), delta=[32, 32, 32, 32], into=True),
        palette3=ColorRainbow(HSV(72, 0, 0), delta=[32, 32, 32, 32], into=True),
        palette4=ColorRainbow(HSV(96, 0, 0), delta=[32, 32, 32, 32], into=True),
        palette5=ColorRainbow(HSV(120, 0, 0), delta=[32, 32, 32, 32], into=True),
        palette6=ColorRainbow(HSV(144, 0, 0), delta=[32, 32, 32, 32], into=True),
        palette7=ColorRainbow(HSV(168, 0, 0), delta=[32, 32, 32, 32], into=True),
    ),
    segment1=Segment(
        strobe=[6, 6, 6, 6],
        gap=[8, 8, 8, 8],
        numc=2,
        palette0=ColorStatic(HSV(0, 0, 0)),
        palette1=ColorStatic(HSV(24, 0, 0)),
        palette2=ColorStatic(HSV(48, 0, 0)),
        palette3=ColorStatic(HSV(72, 0, 0)),
        palette4=ColorStatic(HSV(96, 0, 0)),
        palette5=ColorStatic(HSV(120, 0, 0)),
        palette6=ColorStatic(HSV(144, 0, 0)),
        palette7=ColorStatic(HSV(168, 0, 0)),
    ),
    segment2=Segment(
        change_mode=ChangeMode.THROUGH,
        strobe_count=[3, 3, 3, 3],
        strobe=[6, 6, 6, 6],
        strobe_end=[2, 2, 2, 2],
        blank=[8, 8, 8, 8],
        blank_end=[8, 8, 8, 8],
        gap=[32, 32, 32, 32],
        numc=2,
        palette0=ColorRainbow(HSV(0, 0, 0), delta=[32, 32, 32, 32]),
        palette1=ColorRainbow(HSV(24, 0, 0), delta=[32, 32, 32, 32]),
        palette2=ColorRainbow(HSV(48, 0, 0), delta=[32, 32, 32, 32]),
        palette3=ColorRainbow(HSV(72, 0, 0), delta=[32, 32, 32, 32]),
        palette4=ColorRainbow(HSV(96, 0, 0), delta=[32, 32, 32, 32]),
        palette5=ColorRainbow(HSV(120, 0, 0), delta=[32, 32, 32, 32]),
        palette6=ColorRainbow(HSV(144, 0, 0), delta=[32, 32, 32, 32]),
        palette7=ColorRainbow(HSV(168, 0, 0), delta=[32, 32, 32, 32]),
    ),
)

chase = Mode(
    nums=2,
    numt=2
    segment0=Segment(
        change_mode=ChangeMode.OVER,
        change_count=[8, 8, 8, 8],
        strobe=[0, 0, 0, 0],
        strobe_end=[16, 8, 8, 8],
        gap=[2, 2, 2, 2],
        numc=2,
        palette0=ColorRotate(HSV(0, 0, 0), delta=192, timer=[120, 120, 120, 120]),
        palette1=ColorRotate(HSV(96, 0, 0), delta=192, timer=[120, 120, 120, 120]),
    ),
    segment1=Segment(
        change_mode=ChangeMode.OVER,
        change_count=[8, 8, 8, 8],
        strobe=[16, 8, 8, 8],
        strobe_end=[0, 0, 0, 0],
        gap=[32, 20, 20, 20],
        numc=2,
        palette0=ColorRotate(HSV(96, 0, 0), delta=192, timer=[120, 120, 120, 120]),
        palette1=ColorRotate(HSV(0, 0, 0), delta=192, timer=[120, 120, 120, 120]),
    ),
)

high_contrast = Mode(
    nums=3,
    numt=4,
    thresh=[10, 40, 80, 100],
    segment0=Segment(
        color_mode=ColorMode.SHIFT,
        cmod1=RotDir.COUNTERCLOCK,
        cmod2=EffectDir.TO,
        shift_delta=[8, 16, 32, 64],
        strobe=[10, 8, 5, 3],
        gap=[5, 10, 15, 15],
        palette0=ColorRotate(HSV(Hue.RED), delta=192, timer=[10, 10, 10, 10])
    ),
    segment1=Segment(
        strobe=[10, 8, 5, 3],
        gap=[5, 10, 15, 15],
        palette0=ColorRotate(HSV(Hue.RED), delta=192, timer=[10, 10, 10, 10])
    ),
    segment2=Segment(
        color_mode = ColorMode.SHIFT,
        cmod1=RotDir.CLOCK,
        cmod2=EffectDir.FROM,
        shift_delta=[8, 16, 32, 64],
        strobe=[10, 8, 5, 3],
        gap=[30, 22, 15, 15],
        palette0=ColorRotate(HSV(Hue.RED), delta=192, timer=[10, 10, 10, 10])
    ),
)
