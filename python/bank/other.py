

candy_strobe = Mode(
    nums=1,
    numt=1,
    segment0=Segment(
        color_mode=ColorMode.GROUP,
        cmod1=2,
        cmod2=Drop.ONE,
        strobe_count=[7, 7, 7, 7],
        strobe=[8, 8, 8, 8],
        blank=[17, 17, 17, 17],
        gap=[17, 17, 17, 17],
        palette0=ColorStatic(HSV(Hue.RED)),
        palette1=ColorStatic(HSV(Hue.GREEN)),
        palette2=ColorStatic(HSV(Hue.BLUE)),
    ),
)

hyperloop = Mode(
    nums=1,
    numt=1,
    segment0=Segment(
        color_mode=ColorMode.GROUP,
        cmod1=3,
        cmod2=Drop.ONE,
        strobe_count=[7, 7, 7, 7],
        strobe=[14, 14, 14, 14],
        blank=[14, 14, 14, 14],
        gap=[14, 14, 14, 14],
    ),
)
