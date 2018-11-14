# My Modes as Quark Modes

I didn't want to title this article "Converting Mode to Quark Modes" because I'm not teaching a
lesson I'm giving examples of converting a few modes that I've built for firmwares to Quark modes
for fun and excitement. It helps me, it helps you, it's great stuff. I've explained a ton of junk
in the previous articles, so now it's off to the show.


## Metamorph and Basic Strobes

An easy mode to do, Metamorph is a slow blend that goes from hyperstrobe to ribbon to dops.

```
Metamorph (Quark Edition)
1 segment
3 thresholds

Segment 1:
    1: Blend from red to red clockwise

    strobe:     12.5ms      25.0ms      1.5ms
    blank:      12.5ms      0.0ms       23.0ms
```

Meta mode from the Kinetics is similar, any palette works:

```
Meta (Quark Edition)
1 segment
3 thresholds

Segment 1:
    strobe:     5.0ms       17.0ms      1.5ms
    blank:      9.0ms       17.0ms      21.0ms
```

A one-segment mode with all defaults only cares about strobe, blank, and gap times. Very simple.


## Breaker and Basic Tracers

Breaker is a tracer that breaks apart. Any palette again:

```
Breaker (Quark Edition)
2 segments
2 thresholds

Segment 1:
    strobe:     5.0ms       5.0ms
    gap:        1.0ms       7.5ms

Segment 2:
    #strobes:   5           5
    strobe:     10.0ms      1.0ms
    blank       0.0ms       9.0ms
    gap:        1.0ms       7.5ms
```

Tracers are simple two-segment modes. Put your tracer color in the tracer segment and all other
colors in the other segment and it's your classic tracer. Dashdops:

```
Dashdops (Quark Edition)
2 segments
3 thresholds

Segment 1: multi color, dash -> dash -> dops
    #strobes:   6           6           6
    strobe:     4.0ms       4.0ms       1.5ms
    blank:      0.0ms       0.0ms       11.5ms
    gap:        11.5ms      24.0ms      11.5ms

Segment 2: single color, dops -> dash -> dash
    #strobes:   6           6           6
    strobe:     1.5ms       4.0ms       4.0ms
    blank:      11.5ms      0.0ms       0.0ms
    gap:        11.5ms      24.0ms      11.5ms
```

This one's a little cheeky as always. It switches between something like EL's dashdops and FL's
dashdops. In the middle it's a dashdash.

Ghost's tracer repeats more times the faster you go:

```
Ghost (Quark Edition)
2 segments
4 thresholds

Segment 1: multi color
    #strobes:   3           3           3           3
    strobe:     15.0ms      10.0ms      7.5ms       5.0ms
    blank:      15.0ms      17.5ms      20.0ms      20.0ms
    gap:        15.0ms      15.0ms      15.0ms      15.0ms

Segment 2: ghost
    #strobes:   1           2           3           4
    strobe:     10.0ms      10.0ms      7.5ms       5.0ms
    blank:      15.0ms      15.0ms      15.0ms      15.0ms
    gap:        15.0ms      15.0ms      15.0ms      15.0ms
```


## Edge

Edge is a good case for a three-segment mode. Let's start with the pattern and discuss the palette:

```
Edge (Quark Edition)
3 segments
2 thresholds

Segment 1: front edge
    #strobes:   2           2
    strobe:     2.0ms       0.5ms

Segment 2: center
    strobe:     6.0ms       2.0ms

Segment 3: rear edge and gap
    color mode palette reversed
    #strobes:   2           2
    strobe:     2.0ms       0.5ms
```

Segment 2 only needs the center color for the edge. That's easy enough. With color mode reversed we
can just copy the palette between segments 1 and 3. Instead of picking the first color for the
first slot, the third segment picks the second and visa versa. If there were three colors, they
would be chosen in reverse order. Reversed color mode is for mirroring color progressions without
needing to do any additional work.

As no previous pattern I'm aware of requires a fourth segment, I'll start detailing more complex
modes.


## Darkside of the Mode

We rebuilt this bad boy in the last article. Darkside is an optical illusion. Strobe colors quickly
enough and they look whit should they balance. I go from a blink-e that uses this illusion to a
very long hyperstrobe which is also a rainbow. Once surpassing a threshold, the rainbow breaks into
pieces, each it's own color of the rainbow. I initially did it for the pun. Now I do it because it
reminds me of my wife, a woman who sees rainbows in everything.

```
Darkside of the Mode (Quark Edition 2.0)
2 segments
4 thresholds, switch from 2->3

Segment 1:
    1: Rainbow from red to red counterclockwise

    #strobes:   6           6           8           16
    strobe:     0.5ms       10.0ms      7.5ms       0.5ms
    blank:      0.0ms       0.0ms       0.0ms       7.0ms

Segment 2:
    1: Blank

    #strobes:   6           6           8           1
    blank:      19.5ms      10.0ms      7.5ms       0.0ms
```

The first segment has one color - a Rainbow that goes from red counterclockwise to red over the
span of a single segment. Our segment is 6 strobes long, but each strobe starts off at only 0.5ms
long. The rainbow will be 6 evenly-spaced colors around the color wheel and get longer and have
more variation as motion intensifies. The middle two thresholds switch between them, so the fact
that the timings and number of strobes change doesn't matter. The pattern plays the same under
either condition. The number changing between levels 3 and 4 does matter and the result will be
somewhat unpredictable in overall length of the fill segment but doesn't matter much under usage.

The second segment is only there to provide the large gap of a blink-e pattern for the low and
middle parts of the pattern. It fades out to nothing as it nears the high end. The palette is a
single blank as empty palettes are not valid palettes. Since there is never any strobe length, the
blank won't even ever be shown.

Prism on the Ubers is Darkside with Streak colors.


## Sourcery

Behind Darkside, this is my favorite mode. With the right settings, you feel like a wizard. I'm not
sure if I've ever given a good show with Sourcery on. If I had, the reactions didn't show it. While
teleport modes have been done before, the reactivity of the *vectr* engine is put to the test with
Sourcery, and the Uber version was expertly tweaked by Jarrad to feel great. I'm not giving away
his thresholds, but since you can program your own in Vectr before and Quark forthcoming, you can
find your own sweet spot. However, my Sourcery from Vectr requires some dim tracer love and the
Uber version doesn't have it. Sure, maybe people will use it more, but that's why I build for both
others and myself rather than for both at once. Choice is grand!

My newest version of Sourcery is blank under level 1 with purples and magentas that fade due to
very low blink times. The 1->2 transition happens fairly quickly, with level 2 being a Strobie-
like timing. The 2->3 transition is solely that of a tracer that fades in. This is a lower-mid
speed. The 3->4 transition is of the tracer breaking into pieces until it becomes a Vex-like
pattern with 3 red dops in between dashes of the purple and pink.

```
Sourcery (Quark Edition)
2 segments
4 thresholds

Segment 1:
    1: Streak from purple to hot pink
    2: Streak from hot pink to magenta
    3: Streak from magenta to purple

    strobe:         0.0ms       2.5ms       2.5ms       10.0ms
    gap:            0.0ms       0.0ms       0.0ms       5.0ms

Segment 2:
    1: Reactive blank/blank/low red/lowest red

    #strobes:       3           3           3           3
    strobe:         15.0ms      15.0ms      15.0ms      1.0ms
    blank:          0.0ms       0.0ms       0.0ms       9.0ms
    gap:            0.0ms       0.0ms       0.0ms       5.0ms
```

Upgrades! First off, to digest the pattern, you need to understand how the color will work for the
second segment as it is motion reactive. Until level 2, the color is blank, so strobe time is blank
time. From 2 to 3, the color fades to a low red. The level 3 being a solid strobe means it will be
a solid tracer between showings of the first segment. Finally, the strobe shortens and the blank
increases until we have something shorter and more rapid than a dops. The first segment is the
strobie. For it I am using streak colors for quick blends. As the streaks will never be longer than
10.0ms long, the blends will be quite fast. The colors are all close to one another on the color
wheel, so the result doesn't stand out as much as it accentuates the colors. The gap is used to
maintain a spacing between the dops version of the tracer and the streak.


## Heatwave

Heatwave was one of my favorites in Vectr. The change-over-time patterns never saw much love and it
was the first one that really grabbed me. It's a single segment Quark mode, as the pattern is a dop
wave. The color palette would then blend from blues to reds.

```
Heatwave (Quark Edition)
1 segment
2 thresholds

Segment 1:
    1: Reactive from Blue to Red
    2: Reactive from Purple to Orange
    3: Reactive from Sky to Hot Pink

    change over n segments alterating
    change n:       25(50)      25(50)
    strobe start:   2.5ms       1.0ms
    strobe end:     2.5ms       1.0ms
    blank start:    0.0ms       0.0ms
    blank end:      50.0ms      30.0ms
```

Here we use the start/end features of Quark to make a reactive wave. The wave will "fast-forward"
during periods of higher motion. Quark handles motion reactivity for dynamic patterns much better
than Vectr had. Stretch patterns are also easy:

```
Stretch (Quark Edition)
1 segment
2 thresholds

Segment 1:
    change over n segments
    change n:       8           8
    strobe start:   0.0ms       0.0ms
    strobe end:     25.0ms      10.0ms
    blank start:    25.0ms      10.0ms
    blank end:      0.0ms       0.0ms
```

Chase is a pattern I got from a fiber optic whip. I believe X-Change on the Matrix set also does
a similar effect. The idea is that one color chases the other away.

```
RRRR.RRRG.RRGG.RGGG.GGGG.GGGR.GGRR.GRRR.
```

Here it is in Quark:

```
Chase (Quark Edition)
2 segments
2 thresholds

Segment 1:
    change size over 8 steps
    strobe start:   0.0ms       0.0ms
    strobe end:     25.0ms      15.0ms

Segment 2:
    change size over 8 steps
    strobe start:   25.0ms      15.0ms
    strobe end:     0.0ms       0.0ms
    gap:            25.0ms      15.0ms
```

The palette for segment 1 should move the first color of the palette to the end of the palette.
It's color should match up against the next color in the second segment at all times. Over 8 steps,
the first segment will grow longer and longer until it's the entire strobe and the sequence
restarts.


## Using Halo

Bleeding Edge is a mode I created for a personal set. The mode has been adapted for the Ubers as
Halo and has a whole color mode devoted to it here. I consider it my second-best creation after
Darkside. Like Halo, the palette doesn't matter, but it needs to be the same for all 3 segments.

```
Halo (Quark Edition)
3 segments
2 thresholds

Segment 1:
    halo to
    halo clockwise alternating
    fade in
    increment color every 2 loops
    delta:          0           64
    strobe:         0.0ms       5.0ms

Segment 2:
    increment color every 2 loops
    strobe:         10.0ms       2.5ms

Segment 3:
    halo from
    halo clockwise alternating
    fade out
    increment color every 2 loops
    delta:          0           64
    strobe:         0.0ms       5.0ms
    gap:            30.0ms      15.0ms
```

The first and third segments mostly mirror one another with the gap in the third providing the only
blank spacing in the pattern. The halo direction alternates so the color must increment every other
loop to get both contrasts. Fading is used to dim the edges of the halo so the center stands out
above the contrasting edges. Bleeding Edge would simplify and use a single Blend color for the
palettes, but this gave me a change to explain the increment color every option which hasn't seen
use.

This is a combination of two modes I really love. Rainbow strobe would strobe a group of colors
that were close together and slowly step through the rainbow. High contrast would strobe colors
that got further apart on the color wheel based on the speed of the strobe. We use Halo for this
mode. Halo delta is measures in 1/8 hues. So with 24 hues, there are 193 possible values for delta
(from 0 to 192). Delta is motion reactive. It'll make more sense after, you have to trust me:

```
Rainbow Saiyan
2 segments
2 thresholds

Segment 1:
    1: Blend from green to green clockwise alternating

    halo to alternating
    halo counterclockwise alternating
    delta:          0           32

    Pattern:
    #strobes:       2           2
    strobe:         4.5ms       1.5ms
    blank:          9.0ms       11.0ms

Segment 2:
    Palette:
    1: Blend from green to green clockwise alternating

    Pattern:
    strobe:         4.5ms       1.5ms
    blank:          9.0ms       11.0ms
```

The first segment initially choses two colors up to 4 and 8 hues lower (counterclockwise) than the
hue of the constantly blending color in the color palette. It then alternates between lower and
higher every loop. The colors begin haloing to the color and then alternate to from the color.  The
second segment is nearly identical to the first and only differs in not having any halo and being a
single strobe. The result will be strobes all in close proximity to each other on the color wheel,
getting increasingly constrasing with higher motion.


## Quite a Collection

<words>
