# Glossary

You'll run across these words as you read along. Here's what I mean by them:

* mode - combination of pattern and palette
* pattern - what the on/off pattern of the light looks like combined with how the palette is used
* palette - set of chosen colors
* segment - a segment is part of a pattern and patterns can have up to 4 segments
* piece - either a strobe or a blank in a segment
* strobe - light is on showing a color (color could be blank)
* blank - light is showing no color (not even a blank color)
* traversal - to go from beginning to end
* loop - one full traversal of a pattern


# Basic Strobes and Single-Segment Modes

Single segment modes are sufficient to cover many patterns: strobe, hyperstrobe, strobie, dops, blinke, and ribbon
are all single segment modes. Strobie and dops, two of the most popular patterns, only use one segment and the
`strobe` and `blank` variables.

```
Dops
Segment 1:
    strobe:             1.5ms
    blank:              21.0ms
```

Blinke, crush, and stribbon all require an extra variable `gap`. Gap comes after the final piece in a segment.

```
Blinke
Segment 1:
    strobe:             1.5ms
    blank:              0.0ms
    gap:                60.0ms
```

For patterns similar to crush where there's a blank between the strobes and a trailing gap, the `skip_last_blank` is
available. For crush we want a 3ms strobe with a 5ms gap between each strobe and 40ms after the final strobe. With
`skip_last_blank` false, the gap would be 35ms (40 - 5). With it set to true, the gap is simply 40ms.

```
Crush
Segment 1:
    skip_last_blank:    True

    strobe:             3.0ms
    blank:              5.0ms
    gap:                40.0ms
```

Color modes will be explained more later, and the color modes of the above patterns make little difference. To make a
candy strobe/hyperloop/vortex pattern, **Palette** mode will need to be used. Palette (and Reversed) mode have `pick`
and `skip_all` variables to define how to use the segment's palette. `pick` allows selecting only a subset of colors
from a palette for use during a segment. For `pick` to have any effect on a pattern, `strobe_count` must be greater
than 1 as there is only one strobe to show colors. If `strobe_count` is ever less than `pick`, there will be colors
not shown. After a segment completes, the colors will change based on the `skip_all` value. If `skip_all` is false,
the first color will be dropped and the next color will be added to the end of the set. If true, all colors are
dropped and a set is chosen from the next sequence of colors in the palette. The `repeat_count` value can be used to
repeat the same group of colors a number of times.

```
Candy Strobe
Segment 1:
    color_mode:         Palette
    pick:               2
    skip_all:           False

    strobe:             5.0ms
    blank:              9.0ms
    strobe_count:       7
    repeat_count:       1
```

So, to summarize the values to now:

* `strobe` - length of strobe
* `blank` - length of blank between strobes
* `gap` - length of blank after last piece
* `skip_last_blank` - if true, there is no blank after the final strobe in a segment
* `pick` - how many colors to chose for one segment traversal
* `skip_all` - if true, all colors are dropped when moving to the next loop while only one is dropped if false
* `strobe_count` - number of strobes in a segment
* `repeat_count` - number of loops to repeat segment


# Basic Tracers and Double-Segment Modes

The most common double segment mode is the tracer. Tracers are not just the "tracer" pattern, but any pattern where
one color is chosen as a dominant color between the rest of the colors which are secondary, like vex and dashdops.
Not all double segment modes are tracers, but all tracers are double segment modes. When using double segment modes,
`skip_last_blank` is commonly set to true so that the gap is easier to calculate and reason about.

```
Tracer
Segment 1: the tracer
    strobe:             25.0ms

Segment 2: the colors
    strobe:             5.0ms
```

```
Vex
Segment 1: the dops
    skip_last_blank:    True

    strobe:             1.5ms
    blank:              15.0ms
    gap:                10.0ms
    strobe_count:       3

Segment 2: the colors
    strobe:             5.0ms
    gap:                10.0ms
```

```
Dashdops
Segment 1: the dops
    skip_last_blank:    True

    strobe:             1.5ms
    blank:              15.0ms
    gap:                10.0ms
    strobe_count:       3

Segment 2: the colors
    strobe:             3.0ms
    gap:                10.0ms
    strobe_count:       8
```


# Basic Edges and Triple-Segment Modes

Edges were the inspiration for triple segment modes and introduced the problem of mirroring to cases that needed to
be handled. An edge walks a palette in a reversed order from the first segment to the last, so the color mode
**Reversed** was originally built to handle these cases.

```
Edge
Segment 1: front egde
    color_mode:         Palette

    strobe:             1.0ms
    strobe_count:       3

Segment 2: center
    strobe:             2.5ms

Segment 3: rear edge
    color_mode:         Reversed

    strobe:             1.0ms
    gap:                50ms
```


# Strobe and Blank End

Dynamic patterns are how I refer to patterns that change over time. Patterns like wave, stretch, and shapeshifter
are examples of dynamic patterns. Dynamic patterns come with a new set of variables to define how it behaves.
`strobe_end` and `blank_end` are the target values that the strobe and blank lengths will move to as the pattern
plays out with `strobe` and `blank` being the starting points. `change_mode` defines how the change takes place.
Off indicates that no dynamic change will take place, through segment indicates that the change will take place
over `strobe_count` steps within a single segment, and over segments indicates that the change will take place over
`change_count` segments. If the change will only take place over 1 step for any reason, only the starting value
will be used. Both through segment and over segments have *alternating* versions where rather than going from
start to end repeatedly it will go from start to end and back to start before repeating.

```
Shapeshifter
Segment 1:
    skip_last_blank:    True
    change_mode:        Over segments
    change_count:       3

    strobe:             2.5ms
    strobe_end:         7.5ms
    blank:              5.0ms
    blank_end:          5.0ms
    gap:                30.0ms
```

Because wave goes forward and back, it needs two sections that mirror `blank` and `blank_end`.  They must also use
*through segment* for the `change_mode` value.

```
Wave
Segment 1:
    change_mode:        Through segment
    change_count:       16

    strobe:             1.5ms
    strobe_end:         1.5ms
    blank:              0.0ms
    blank_end:          45.0ms

Segment 2:
    change_mode:        Through segment
    change_count:       16

    strobe:             1.5ms
    strobe_end:         1.5ms
    blank:              45.0ms
    blank_end:          0.0ms
```

X-Change uses two segments that mirror each other's `strobes` and `blanks`. A palette of ABC will need to have ABC
in the second segment and BCA (move the first element to the end) for the effect to work.

```
X-Change
Segment 1:
    change_mode:        Over segments
    change_count:       8

    strobe:             0.0ms
    strobe_end:         20.0ms

Segment 2:
    change_mode:        Over segments
    change_count:       8

    strobe:             20.0ms
    strobe_end:         0.0ms
    gap:                20.0ms
```



# Halo Color Mode

**Palette** and **Reversed** color modes will use multiple palette entries per segment if possible. Halo color mode
will only use one. Halo mode will either blend to or away from the color over a segment providing a shift along the
color wheel. `delta` indicates the distance of the blend with 192 being a "full rotation" from a color back to
itself. `rotation_dir` is the direction the blend travels along, clockwise being red to green and counter being red
to blue. `effect_dir` indicates if the blend is going to or from the color. Halo from the Ubers is where this comes
from.

```
Halo
Segment 1:
```


# Morph vs Streak Colors

If looking to mimic classic morph modes such as strobe morph, you actually want *Streak* colors. Traditionally the
morph would happen over a number of strobes.

```
Strobe Morph - use Streak colors
Segment 1:
    strobe_count:       8
    strobe:             5.0ms
    blank:              9.0ms
```

*Streak* colors are aligned to segments while *Morph* colors are time- dependent. Use *Morph* when you want your
lights to stay in sync, but remember that morph colors are ALWAYS morphing, even when not shown.

```
Paintbrush - use Morph colors and multiple stroke/blank combinations stay in sync
    strobe_count:       1
    strobe:             5.0ms       17.0ms      1.5ms
    blank:              9.0ms       17.0ms      15.0ms
```


# Blend vs Rainbow Colors

*Blend* and *Rainbow* colors have no analog to modes on lights that I've made, just personal firmwares. These modes
mirror *Morph* and *Streak* in how they vary as they play, segment-based or time-dependent. These colors blend from
one position on the color wheel to another. Both use the `direction` variable to determine clockwise vs counter-
clockwise rotation, but *Blend* also has the option of "bouncing" when it hits the target and returning before
repeating.


