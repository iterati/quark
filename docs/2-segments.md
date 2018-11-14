# Segments

In the introduction I mentioned how 4 segments made up a pattern, but I didn't really explain what
a segment was other than having a pattern and a palette. This is a more in depth breakdown of
segments and how each part came to exist. Before we dive in, though, I want to use an example to
show you how building something newish is easy with Quark.

Tracers are fun to mess with. Without even getting into motion or fancy colors, we can build a new
pattern that's a twist on the tracer.

```
Rainbow Tracer:
W.R.W.Y.W.G.W.C.W.B.W.M.

Segment 1:
pattern: X.
palette: W

Segment 2:
pattern X.
palette: R Y G C B M

W - White
R - Red
Y - Yellow
G - Green
C - Cyan
B - Blue
M - Magenta
. - Blank
```

Let's spice it up:

```
Hyper Rainbow Tracer:
W.R.Y.G.W.Y.G.C.W.G.C.B.W.C.B.M.W.B.M.R.W.M.R.Y.

Segment 1:
pattern: X.
palette: W

Segment 2:
pattern: X.Y.Z.
palette: R Y G C B M
```

So now we're strobing three colors between every strobe. Neat.


## The Basics

The most basic parameters for a segment are the strobe and blank lengths. As such, these were the
first two to be added. These are represented the letters (strobes) and dots (blanks) in my
examples. There can be a number of strobes in the segment. After the last strobe, before moving to
the next segment, we have a gap.  With these building blocks (strobe, blank, number of strobes, and
gap) for segments and 4 segments to work with, it's easy to build almost any pattern available on
any light. It nearly encompasses everything Vectr can do.  Strobes (including ribbons, dops,
hyperstrobe, etc), tracers, and edge were all covered in the intro. Here are a few more examples
that incorporate gap into the pattern.

```
- is gap

Blink-e
Segment 1: XYZ-

EL Dashdops
Segment 1: X.X.X.-
Segment 2: XYZ-

FL Dashdops
Segment 1: XXX-
Segment 2: X.Y.Z.-

Vex:
Segment 1: X.X.X.-
Segment 2: X-
```

Up to now we've done everything except for a few outliers. Specifically, stuff like candy strobe
(or hyperloop/vortex should you be an FL fan). This is where it gets a little less basic.


## Willy Wonka Time

Patterns have to have **something** to do with colors for something like candy strobe to work.
The basic gist of candy strobe is:

```
Simplified Candy Strobe:
R.G.R.G.R.G.B.G.B.G.B.R.B.R.B.
```

So pick two colors for 5 strobes then drop the first color and add another. Simple enough when put
in those terms, but how do you represent that to the pattern? The answer lies in pick.

Pick is a non-motion reactive variable that can range from 1 (only strobe one color through the
segment) to the number of colors chosen (if the number of colors chosen are more than the number
of colors in the palette, those colors are not shown). It tells the algorithm how many colors to
strobe through before resetting. In the example above, pick is 2 and the number of strobes is 5.
Here's candy strobe and two simple variations:


```
Candy Strobe
1: Pick of 2 with 5 strobes
R.G.R.G.R.G.B.G.B.G.B.R.B.R.B.

Hyperloop
1: Pick of 3 with 3 strobes
R.Y.G.Y.G.C.G.C.B.C.B.M.B.M.R.M.R.Y.

Elemental
1: Pick of 1 with 3 strobes
R.R.R.G.G.G.B.B.B
```

Let's say we wanted to switch up our first creation. Let's not repeat any colors when we strobe and
strobe the tracer 3 times:

```
Two Tone
1: 3 strobes, pick 3, skip all
2: 3 strobes
W.W.W.R.Y.G.W.W.W.C.B.M.
```

Increment all is an option. Either we can increment one color at a time or the number we are
picking. Number of repeats is also added to allow us to repeat a color pattern over multiple loops.
Why don't I allow skipping 2 or 3? Because I don't want to complicate things. It's complex enough
without something that will see little if any usage (I'm talking from experience here with Vectr)
complicating the issue. Pick not being motion reactive is also due to this ideal.

If pick is 0, we just go through the colors in order without regard to the segment.

```
Dr Mario
1: Pick of 0 with 2 strobes
RG-BR-GB-
```


## Changing Sizes

The Aethers are good lights to look to for challenging modes. Take these (not exact) replicas and
variants:

```
Anticipation
1: 3 strobes, blank start 1 end 3
R.G..B...R.G..B...

Dop Wave
1: 3 strobes (note 6 strobes due to alternate), blank start 0 end 2, alternate
RG.B..R..G.BRG.B..R..G.B

Stretch
1: 3 strobes, strobe start 1 end 3, blank start 2 end 0, gap 3, pick 1
R..RR.RRR---R..RR.RRR---

Shapeshifter
1: 3 strobes, strobe start 1 end 3, blank 1, gap 2, change over 3
R.G.B.--RR.GG.BB.--RRR.GGG.BBB.--

Chase
1: 1 strobe, strobe start 4 end 1, change over 4
2: 1 strobe, strobe start 0 end 3, blank 1, change over 4
RRRR.RRRG.RRGG.RGGG.GGGG.GGGR.GGRR.GRRR.

Inside Out
1: 1 strobe, strobe start 3 end 1, change over 3
2: 1 strobe, strobe start 0 end 2, change over 3
3: 1 strobe, strobe start 0 end 2, change over 3
4: 1 strobe, strobe start 3 end 1, blank 1, change over 3
RRRRRR.RRGGRR.RGGGGR.GGGGGG.GGRRGG.GRRRRG.
```

Basic idea is that the gap grows longer (then shorter maybe). It either does this over a segments'
strobes or a number of loops. For this we have 4 new variables: strobe end, blank end, change over,
and size alternates. Old strobe and blank get the shiny new "start" suffix. If change size on is 0,
we change every strobe over the number of strobes. If it's above 0, we change over that many loops
and each strobe is the same length. Size alternates allows for going from start to end to start..
rather than a one-way change. Niether of these two values are motion reactive.


## Halo

I built Halo mode for the Ubers because I love the effect of contrasting colors. To get the effect
of Halo mode, there needs to be some way to display colors differently than if one were showing
the color itself and instead showing a blend contrasting away from it on the color wheel. This is
hard to put into words. First, the color wheel, for those that don't know, is the colors from red
to yellow to green to cyan to blue to magenta back to red. That will be clockwise. To go from red
to magenta to blue.. is counter-clockwise. Halo mode doesn't use the concept of pick - each segment
one color is used across the entire segment.

Instead, the motion reactive delta variable determines how far along the color wheel to blend.
Rotation determines if the blend is clockwise, counter-clockwise, or an alternatating version of
either. The direction will start with the chosen direction and swap on the next loop if the
alternating version is chosen. The direction of the blend is also chosable and able to be
alternated. The direction determines if the blend goes from the color or to it.

```
Halo with RGB
1: 1 strobe, no blank or gap, clockwise rotation, to color
2: 1 strobe, no blank or gap
3: 1 strobe, gap 1, clockwise rotation, from color
YRY-CGC-MBM-

```

To enable Halo mode, a color mode value is used. There may be more color modes in the future, but
for now the two options are Palette for using the pick/increment values and Halo for this method.


## Delta Shift

Delta Shift is like Halo mode, but colors are shifted for the entire segment. Delta Shift is weird
with Reactive colors and doesn't make much sense to use with them.


## Fading

Halo mode didn't just blend the spectrum - it also faded the color. As such, there's a fading
value that will fade the color either in or out. The fade is handled over the number of strobes,
so a single strobe will be faded half way (1 strobe means fade over 2 steps). A 2 strobe fade would
be 2/3 and 1/3, etc. I've considered alternating as an option for this, but I'm not sure how to
handle most cases.

```
Pulse with RGB
1: 1 strobe, no blank or gap, fade in
2: 1 strobe, no blank or gap, no fade
3: 1 strobe, gap 1, fade out
rRr-gGg-bBb-
```


## All Together

So, all together, the variables for a segment in Quark are:

* Strobe length to start with
* Strobe length to end with
* Blank length to start with
* Blank length to end with
* Gap length between segments
* Number of strobes per segment
* Halo and delta shift value

* Size change alternates direction
* Size changes over single segment or over n segments
* Color mode is either palette or halo
* Pick n colors per segment in palette mode
* Increment color one or $pick in palette mode
* Increment color every n loops
* Color wheel rotation direction for halo and delta modes
* Color wheel blend direction (to/from color) for halo mode
* Fading option is off, fade in, or fade out

The first set of 7 are motion reactive values while the second set of 9 are static values. This is
the same for all 4 sequences that can make up a single mode. In Quark, each sequence gets it's own
set of up to 8 colors. With traditional colors and this Lego-like pattern structure, almost every
pattern is reproducible. Still missing are morphs and may of the motion reactive patterns I made
for the Ubers, and that's where that variety of color options comes into play. Until next time.
