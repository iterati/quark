from functools import partial


class Segment:
    numc = 0
    change_mode = 0
    color_mode = 0
    dim_mode = 0
    cmod1 = 0
    cmod2 = 0
    strobe = [0, 0, 0, 0]
    strobe_end = [0, 0, 0, 0]
    blank = [0, 0, 0, 0]
    blank_end = [0, 0, 0, 0]
    gap = [0, 0, 0, 0]
    strobe_count = [0, 0, 0, 0]
    repeat_count = [0, 0, 0, 0]
    change_count = [0, 0, 0, 0]
    shift_delta = [0, 0, 0, 0]


class Mode:
    nums = 0
    numt = 0
    falloff = 0
    trigger = 0
    thresh = [0, 0, 0, 0]
    jump = [0, 0, 0]
    segments = [
        Segment(),
        Segment(),
        Segment(),
        Segment(),
    ]


mode = Mode()


def in_ms(val):
    return "{}".format(round(val / 2.0, 1))

def the_value(val):
    return val

def mk_update(target):
    def update_func(self, value):
        setattr(self, target, value)

    return update_func


strobe = {
    "nums": 1,
    "falloff": 8,
    "trigger": 1,
    "segments": [
        {
            "color_types": ["static"],
            "change_mode": 0,
            "color_mode": {
                "mode": "palette",
                "group": 0,
                "drop": 1,
            },
            "dim_mode": 0,
            "strobe_count": 1,
            "repeat_count": 1,
            "change_count": 1,
        },
    ],
    "widgets": {
        "numt": {
            "widget": {
                "class": "Slider",
                "args": {
                    "default": 1,
                    "min": 1,
                    "max": 4,
                },
            },
            "updates": {
                mode.numt: the_value,
            },
        },
        "thresh": {
            "widget": {
                "class": "ThresholdEditor",
                "args": {
                    "default": [0, 30, 70, 100],
                }
            },
        },
        "strobe": {
            "widget": {
                "class": "ThreshSlider",  # comes with listens for mode.numt for thresholds
                "args": {
                    "default": 10,
                    "min": 0,
                    "max": 200,
                    "format": in_ms,
                },
            },
            "updates": {
                mode.segments[0].strobe: the_value,
            },
        },
        "blank": {
            "widet": {
                "class": "ThreshSlider",
                "args": {
                    "default": 20,
                    "min": 0,
                    "max": 200,
                    "format": in_ms,
                },
            },
            "updates": {
                mode.segments[0].gap: the_value,
            },
        },
    }
}


tracer = {
    "nums": 2,
    "falloff": 8,
    "trigger": 1,
    "segments": [
        {
            "change_mode": 0,
            "color_mode": {
                "mode": "palette",
                "group": 0,
                "drop": 1,
            },
            "dim_mode": 0,
            "repeat_count": 1,
            "change_count": 1,
        },
        {
            "change_mode": 0,
            "color_mode": {
                "mode": "palette",
            },
            "dim_mode": 0,
            "repeat_count": 1,
            "change_count": 1,
        },
    ],
    "widgets": {
        "numt": {
            "widget": {
                "class": "Slider",
                "args": {
                    "default": 1,
                    "min": 1,
                    "max": 4,
                },
            },
            "updates": {
                mode.numt: the_value,
            },
        },
        "thresh": {
            "widget": {
                "class": "ThresholdEditor",
                "args": {
                    "default": [0, 30, 70, 100],
                },
            },
        },
        "tracer_strobe": {
            "widet": {
                "class": "ThreshSlider",
                "args": {
                    "default": 45,
                    "min": 0,
                    "max": 200,
                    "format": in_ms,
                },
            },
            "updates": {
                mode.segments[0].strobe: the_value,
            },
            "listens": {
            },
        },
        "tracer_pieces": {
            "widet": {
                "class": "ThreshSlider",
                "args": {
                    "default": 1,
                    "min": 1,
                    "max": 32,
                    "format": in_ms,
                },
            },
            "updates": [
                mode.segments[0].strobe_count: the_value,
            ],
        },
        "tracer_blank": {
            "widet": {
                "class": "ThreshSlider",
                "args": {
                    "default": 0,
                    "min": 0,
                    "max": 200,
                    "format": in_ms,
                },
            },
            "updates": [
                mode.segments[0].blank: the_value,
            ],
            "listens": {
                mode.segments[0].strobe_count: "if all 1, hide me!",
            },
        },
        "color_strobe": {
            "widet": {
                "class": "ThreshSlider",
                "args": {
                    "default": 10,
                    "min": 0,
                    "max": 200,
                    "format": in_ms,
                },
            },
            "updates": [
                mode.segments[1].strobe: the_value,
            ],
        },
        "color_pieces": {
            "widet": {
                "class": "ThreshSlider",
                "args": {
                    "default": 1,
                    "min": 1,
                    "max": 32,
                    "format": in_ms,
                },
            },
            "updates": [
                mode.segments[1].strobe_count: the_value,
            ],
        },
        "color_blank": {
            "widet": {
                "class": "ThreshSlider",
                "args": {
                    "default": 20,
                    "min": 0,
                    "max": 200,
                    "format": in_ms,
                },
            },
            "updates": [
                mode.segments[1].blank: the_value,
            ],
            "listens": {
                mode.segments[1].strobe_count: "if all 1, hide me!",
            }
        },
        "color_repeat": {
            "widet": {
                "class": "Checkbox",
                "args": {
                    "default": False,
                    "format": lambda x: "Repeat" if True else "Cycle",
                },
            },
            "updates": [
                mode.segments[1].cmod1: the_value,
            ],
            "listens": {
                mode.segments[1].strobe_count: "if all 1, hide me!",
            }
        },  # if unchecked, group 0, if checked, group 1
        "gap": {
            "widet": {
                "class": "ThreshSlider",
                "args": {
                    "default": 2,
                    "min": 0,
                    "max": 200,
                    "format": in_ms,
                },
            },
            "updates": [
                mode.segments[0].gap: the_value,
                mode.segments[1].gap: the_value,
            ],
        },
    },
}
