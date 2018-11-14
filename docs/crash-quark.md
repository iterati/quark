# Crash Course on Quark

I'm iterati. I made [Primer](), [Vectr](), and the [Uber Nanos]() and Quark is
my new project. Years of work attempting to generalize LED patterns for use in
gloves and orbits has finally come together, and I view these ideas as the base
for what will be the most customizable and enabling firmware imaginable. This
document is meant as a sort of primer. I will walk through the philosophy and
methods of Quark by slowly building up the pieces that define a Quark mode, the
attributes that define those pieces, and how to relate them to concepts that
are familiar. The layout is meant to be read in order. It's not a story, but
each piece builds into the next. The structure is:

* Basic Patterns
  * modes
  * patterns
  * segments
  * strobe
  * blank
  * gap
  * strobe count
* Basic Colors
  * group size
  * drop
  * repeat count
* Basic Motion
  * motion
  * thresholds
  * blend vs jump
* Combining Parts
  * colors and patterns
  * motion and patterns
  * colors and motion
* Color Fundamentals
  * hue
  * saturation
  * value
  * rotate/blend
  * shift mode
  * delta
  * rotation direction
  * effect direction
  * alternate direction
  * dim mode
* Advanced Patterns
  * dynamic patterns
  * strobe end
  * blank end
  * change mode
  * alternating change
* Advanced Colors
  * types of colors
  * static
  * compound
  * reactive
  * morph
  * streak
  * rotate
  * rainbow
* Advanced Motion
  * falloff
  * trigger
  * threshold values
* One Whole


## Basic Patterns - Getting Started

I'll start with explaining a few terms and what they mean to me:

* mode - what we save on a light
* pattern - a repeating sequence of the light being on and off
* strobe - when the color is on
* blank - when the color is off - not the same as showing a blank color
* gap - a blank that breaks up a pattern
* piece - any of the above three

Pretty simple start. Those ideas can get us a lot of traction. For instance,
ribbon is just a series of strobes without blanks. Strobe, strobie, dops,
hyperstrobe, and more are just strobes + blanks. Blinke and stribbon require a
gap but no blank. Crush adds the blank. Lots of classic patterns with just
three attributes. What about tracer, vex, edge, and centerpoint? We'll get to
that. Because for that we need another term:

* segment - a subpattern within a pattern

For a tracer we would use two segments. The first segment would have a long
strobe and no blank. The second segment would have a short strobe and no blank.
Vex throws a wrench in the works. Vex is a strobe followed by a "tracer" of
multiple dops. For Vex to work, we have to think about how a segment is
structured in Quark. I'll explain why after:

```
S - strobe for segment 1
s - strobe for segment 2
B - blank for segment 1
b - blank for segment 2
G - gap for segment 1
g - gap for segment 2

SGsg
```

Wait, what? There's no blanks? Well, the overall structure is that the default
segment only has one strobe, as such, there's no blanks. The gaps come after
the last strobe, so the basic 2-segment pattern would be just strobes and gaps.
If that's the case, then how do we make vex and what did I mean earlier when I
said that all the patterns were strobes and blanks? Let's add another term:

* strobe count - the number of strobes in a segment


```
S - strobe
. - blank
- = gap

strobe count = 5
S.S.S.S.S-
```

There are the blanks! Blanks actually come between strobes in a segment. Gaps
come after a segment. Simple patterns like strobie can just be a strobe and a
gap. But if you want more than one in a subpattern, like in vex, then you will
want to up the strobe count of the subpatterns. Vex also has equal gaps on
both sides of the strobe, so the gaps are equal. They don't have to be, and
some patterns from Primer (bottlerocket is an example) used that to great
effect. Quark allows patterns to be built with up to four segments. Not all
must be used, but they are available. Edge and centerpoint would use three, but
I'm not aware of any pattern that I've seen which would require four.

```
A - vex color (color palette for 1)
B, C, and D - other colors (color palette for 2)

strobe counts = 3 and 2
A.A.A-B.C-A.A.A-D.B-A.A.A-C.D-
```

All values are per-segment, so the variation here is nearly endless. Dashdops
(both EL and FL's variants), dashdot, and endless new patterns are possible
with the segment concept.


## Basic Colors

To express within a pattern, color is needed. Quark gives each segment its own
color palette with up to eight colors per palette. Each segment also has the
ability to describe how it uses the color palette. This requires new terms:

* group size - how many colors are cycled through in a single loop of a segment
* drop - either one or all colors can be dropped when the segment ends
* repeat count - number of repeats before colors are dropped

This allows for candy strobe, hyperloop, and vortex to do what they do with
colors. For patterns such as edge, it's often important to be able to mirror
another segment's color motion. For this, a reversed mode exists for walking
the group in reverse mirroring the normal progression. Some examples:

```
A, B, C, and D - 4 colors strobing
. - blank
- - gap

group size = 0, strobe count = 5, drop ignored
A.B.C.D.A-B.C.D.A.B-C.D.A.B.C-D.A.B.C.D-

group size = 0, strobe count = 5, drop ignored, reversed
A.D.C.B.A-B.A.D.C.B-C.B.A.D.C-D.C.B.A.D-

group size = 1, strobe count = 5, drop doesn't matter
A.A.A.A.A-B.B.B.B.B-C.C.C.C.C-D.D.D.D.D-

group size = 2, strobe count = 5, drop one
A.B.A.B.A-B.C.B.C.B-C.D.C.D.C-D.A.D.A.D-

group size = 2, strobe count = 5, drop all
A.B.A.B.A-C.D.C.D.C-

group size = 3, strobe count = 5, drop one
A.B.C.A.B-B.C.D.B.C-C.D.A.C.D-D.A.B.D.A-

group size = 3, strobe count = 5, drop all
A.B.C.A.B-D.A.B.D.A-C.D.A.C.D-B.C.D.B.D-

group size = 3, strobe count = 5, drop all, reversed
C.B.A.C.B-B.A.D.B.A-A.C.D.A.C-D.C.B.D.C-

group size = 1, strobe count = 2, repeat count = 2
A.A-A.A-B.B-B.B-C.C-C.C-D.D-D.D-
```

Not all of those combinations are useful, but they exist and do a good job
demonstrating how the different options interact. Having group size of 0
disables any grouping, 1 repeats a single color, 2 represents a candy strobe,
and 3 a hyperloop. Drop all allows for subsets of colors rather than walking
color progressions.


## Basic Motion

Many (not all) of Quark's attributes may vary depending on the intensity of
motion. Let me explain:

* motion - how fast and hard is the light moving
* threshold - a level of motion that is tracked
* value - some attribute of a mode (e.g. strobe, repeat count)
* blend - to smoothly increase a value from one threshold to another
* jump - to quickly change a value from one threshold to another
* motion reactive - jumps or blends between thresholds based on motion

All pattern attributes described so far are motion reactive and none of the
color ones are. Adding colors is simply a matter of having a group size that is
larger than the strobe count with drop all set:

```
A, B, C, and D - 4 colors strobing
. - blank
- - gap

group size = 4, strobe count = 1, 2, 3, 4 based on threshold
thresh 1        A-
thresh 2        A.B-
thresh 3        A.B.C-
thresh 4        A.B.C.D-
```

By using blend, more than 4 visible levels of color could be seen:

```
A, B, C, D, E, F, and G - 7 colors strobing
. - blank
- - gap

group size = 4, strobe count = 1, 3, 5, 7 based on threshold
thresh 1        A-
thresh 1-2      A.B-
thresh 2        A.B.C-
thresh 2-3      A.B.C.D-
thresh 3        A.B.C.D.E-
thresh 3-4      A.B.C.D.E.F-
thresh 4        A.B.C.D.E.F.G-
```

The basics of motion reactivity allow for dead zones between two levels by
using the same value at two threshold levels and to jump from one value to
another with the jump switch. Jump is a value for the transition between
thresholds:

```
A, B, and C - 3 colors strobing
. - blank
- - gap

group size = 3, strobe count = 3, 1, 1, 3 based on threshold, 1-2 jumps
thresh 1        A.B.C-
thresh 1-2      A.B.C-
thresh 2        A-
thresh 2-3      A-
thresh 3        A-
thresh 3-4      A.B-
thresh 4        A.B.C-
```

Many different motion profiles and actions are available with just a simple
motion reactivity system that Quark has, but these have just been the basics.


## Combining Parts

Edge mode has not been covered yet, because edge mode requires understanding
both basic patterns and basic colors. Edge mode counts down, presents a single
color, and then counts back up. I'll demonstrate:

```
A - center color strobing (color palette for 2)
B and C - edge colors strobing (color palettes for 1 and 3)
- - gap

strobe counts = 2, 1, 2, first segment is reversed
CBABC-
```

More effects are possible using different settings for the mode, and Quark is
about allowing expression through that variation. Patterns and motion can also
combine in interesting ways. Breaker and sourcery are two great modes that use
motion to create novel effects. In breaker, a tracer fades to a vex as motion
increases:

```
A - tracer color strobing (color palette for 1)
B and C - other colors strobing (color palette for 2)
. - blank
- - gap

strobe counts = 3, 1, blank 0->1, gaps 0->1
thresh 1:   AAABAAAC
thresh 2:   A.A.A-B-A.A.A-C-
```

For sourcery, the light is blank at the start because strobe length is 0. As
motion increases, the strobe length does as well:

```
A, B, and C - 3 colors strobing
- - gap

strobe increases 0->1
thresh 1:   ---
thresh 2:   A-B-C-
```

These are all simple effects that can be used to great effect. Magic adds a
and fourth threshold and sets strobe to 0 in the middle for a blank deadzone:

```
A, B, and C - 3 colors strobing
- - gap

strobe 1->0->0->1
thresh 1:   A-B-C-
thresh 2:   ---
thresh 3:   ---
thresh 4:   A-B-C-
```

Combining motion and color requires glossing over something that will be
explained more in depth later, but each color can be motion reactive with up to
4 color layers per color.

```
(A->B->C), D, and E - three colors strobing, the first is motion reactive
- - gap

thresh 1: A-D-E-
thresh 2: B-D-E-
thresh 3: C-D-E-
```

All of these concepts can be mixed and matched. Quark is about fundamentals and
each of these is just a slight manipulation of the properties available to you.


## Color Fundamentals

Before being capable of understanding how some of the more advanced concepts
behave, understanding how colors work in Quark is fundamental. Quark colors are
built from an 8-bit, partially color brightness corrected, simplified HSV
format. HSV or Hue, Saturation, and Value colors are colors that are defined by
their hue (red through yellow to green through cyan to blue through magenta to
red again for a clockwise/additive rotation); saturation which is the level of
white that is in the color, with high saturation meaning less white; and value,
which is a measure of the brightness of the color, with low value meaning low
brightness. In Quark, colors use HSV but I invert the values for saturation and
brightness and there are four levels of each. Hue has 192 possible values, with
some inaccuracy at each level. Two special colors are also available: white and
blank. White is a saturation value of 4 (with 0 being full color and 3 being
the most pastel). Blank is a value of 4 (with 0 being full brightness and 3
being the dimmest possible).

HSV color allows for colors to rotate or blend around the spectrum. Here are
the new terms and terms so far:

* hue - "color" of a color
* saturation - amount of white in a color
* value - how bright a color is
* rotate/blend - hue is added to or subtracted from
* rotation direction - clockwise for additive and counterclockwise for
subtractive

How does this work with patterns up to now? The group size and drop options are
not the only mode a segment can use for displaying color. Shift mode will blend
a color over the span of the segment. To understand how, we have more terms:

* delta - how far to rotate (192 is a full rotation)
* effect direction - blending to or from the selected color
* alternate direction - alternate either the rotation or effect direction after
each segment

Halo mode on the Ubers is an edge-like mode that uses these ideas to make its
effect:

```
A, B, and C - 3 colors strobing (segment 1)
<a, <b, and <c - the edges of each color blending to the color (segment 2)
a>, b>, and c> - the edges of each color blending from the color (segment 3)
- - gap

strobe count = 2, 1, 2
<aAa>-<bBb>-<cCc>-
```

Delta is motion reactive but effect and rotation direction are not.

The final piece of the color puzzle is dimming. Dimming modes come in four
flavors: off, fade in, fade out, and pulse. Fade in causes the color to fade in
from blank over the span of a segment. Fade out causes the color to fade out to
blank. Pulse fades in and then out.


## Advanced Patterns

One pattern type not discussed yet covers a some different patterns: wave,
stretch, shapeshifter, and x-change. These patterns are what I call dynamic
patterns in that they change over time. It took me quite some effort to get a
good system for combining motion with dynamic patterns, and the system in Quark
finally allows for motion reactive dynamic patterns that are smooth as butter.

* change mode - either no change occurs, change happens through a single
segment, or change happens over multiple segments
* change count - number of segments change happens over if it happens over
multiple segments
* alternate change - change goes from start to end to start rather than just
start to end
* strobe end - the value of the final strobe in a change cycle
* blank end - the value of the final blank in a change cycle

The change mode has five options: off, through segment, over multiple segments,
and alternating versions of the last two. Through segment will change lengths
over the strobe count of a segment. Over multiple segments will change after
each segment, and the pattern will take change count segments to complete.
Alternating indicates that the change will go from start to end back to start
before repeating while the non-alternating versions just go from start to end
before repeating. When there aren't enough steps to enable a change (1 for a
non-alternating pattern or 1 or 2 for one that alternates), only the starting
value is shown.

For "through segment" changes, there are 3 cases to consider and each behaves
differently:

* non-alternating
* alternating with an odd strobe count
* alternating with an even strobe count

In the following examples strobe and blank both start and end at 1 and 3 (the
first line is the strobe timings, the second is the blank timings with -
indicating a gap):

```
non-alternating over 3
1 2 3|1
 1 2 -

alternating over 5
1 2 3 2 1|1
 1 2 2 1 -

alternating over 6
1 2 3 3 2 1|1
 1 2 3 2 1 -
```

In the non-alternating pattern, the final blank length is never reached. The
reason for this is because it makes having strobes and blanks that increase at
the same rate to be made. Having the gap value equal that of the blank end is
the remedy for this problem.

In the odd alternating pattern, the maximum value is again not shown. Due to
the way alternating patterns work, for odd numbers, the middle strobe is the
peak (end value), but blanks come between strobes. As such, there's no blank
between the peak value and the peak blank value is not used. Again, this allows
for more consistency. This time it's due to strobe count being motion reactive.
If the strobe count becomes positive, the even pattern is made. The peak strobe
value now spans two strobes, so the peak blank value can be used in between
these peak strobes to keep consistency.

These rules have little effect on patterns that care little for precision and
aim for more of a chaotic look, but when designing precise patterns, the mental
overhead is lower with these rules than without.

Something like wave would use the alternating through segment pattern:

```
A, B, and C - 3 colors strobing 1 frame
. - one blank frame

strobe count = 6
A.B..C...A...B..C.A
```

Stretch would be an alternating over segments pattern:

```
A, B, and C - 3 colors strobing 1 frame
. - one blank frame

A...BB..CCC.AA..B...
```

Shapeshifter is non-alternating and has a strobe count above 1. Chase is more
interesing. It's a 2-segment pattern where the second segment's palette must be
the same as the first's with the last color moved to the front. The first
segment's strobe start and end would be 0 and X while the second's would be X+1
and 1 and both repeat and change count are X+1.

```
AB - first palette
BA - second pallete

A and B - 2 colors strobing 1 frame
- - gap

repeat count = 4
-BBBB-A-BBB-AA-BB-AAA-b-AAAA-B-AAA-BB-AA-BBB-A-
```


## Advanced Colors

Most patterns have been laid to waste. A few remain - ones that require color
morphing, heartbeat, and tilt. Heartbeat's not a pattern I'm ever implementing,
so morph modes then. I will never use tilt so that's also out. Too hard to
build something I'll never have the motivation to test and tune. That leaves
morph modes. But before we tackle morph modes, I want to explain what Quark
does for colors and why. First the why - it enables things like morph in any
pattern along with many other options for colors beyond what we think of colors
now. Now the what - there are seven (7) types of colors in Quark.

I've already explained two.  Colors as you are familiar with are called
"Static" colors in Quark. They don't change over time or motion. They just are.
"Reactive" colors change based on motion. They were explained at the end of
**Combining Parts**. That leaves five more types not yet desribed: compound,
morph, streak, rotate, and rainbow.

Compound colors require understanding the speed of rendering Quark provides,
which is 2000 frames per second, and what that means when a light blinks that
fast. Our eyes cannot detect light changes at 2000 frames per second. If red
and green were to alternate at 2000fps, the light would appear yellow. When
moving the light, especially at a high velocity, the red and green would become
visible. Compound colors will alternate between up to four colors at 2000fps
when the color is being displayed. This means if two colors are picked, the
first is displayed on all odd frames and the second on all even frames.

Morph and Streak colors cover morph. Morph as most are familiar would actually
be "Streak" colors in Quark. Streak colors change from one color to the next
over the span of a single segment. Morph colors change over time. That rate of
change is motion reactive. Both Morph and Streak colors can morph between up to
four colors. Streak colors also have a direction that can allow a "fusion"
morph as in the Aethers where it blends 2->1, 3->2, 1->3.

Rotate and Rainbow colors allow for hue-based changes to colors either over
time or over a segment. Both require a starting and ending hue and a direction
of rotation. Rotate has a motion reactive speed and an option to alternate the
direction of rotation when the end point is reached.

All Compound, Morph, and Rotate colors can be thought of as "always playing" in
that they will continue to change color even when not displayed. Streak and
Rainbow colors make little sense unless used with a pick of 1 as they need to
play over the entire segment for their full effect. Morph and Streak also need
more than one chosen color to be more than a static color that takes up extra
computing cycles.


## Advanced Motion

With all of these pattern and color options, nearly every pattern is possible
with Quark. New patterns are easy to iterate on and some of the color options
will enable patterns and effects never seen before. With Quark I'm also
bringing motion tracking customization to new levels. First, a primer on how
motion tracking in Vectr and the Ubers works.

Every 20ms (40 frames), new accelerometer readings are read. These are values
that map from -8G to +8G for the X, Y, and Z axes. These values are then
squared, added, and then the square root is taken to get the magnitude of
acceleration for that moment in time.  One hundred levels of magnitude are
checked against, and if the magnitude is above that threshold, a trigger value
is increased. If it is not, a falloff value is increased. If the falloff value
for a bin reaches a falloff threshold, the trigger value is zeroed out. If the
trigger value reaches a trigger threshold, then the value is considered active.
The highest active value is the current motion value.

In Quark, the range of magnitude tracked is configurable and the falloff and
trigger thresholds are motion reactive. The interaction can be odd under some
circumstances, so careful tuning is required. The range is an easy one, a low
range of 3 to 4G would make anything above medium-speed moved in gloving max
out the tracked motion value. 5G is the max speed tracked in the Uber Nanos and
Vectr. Experimental orbit-friendly versions of Vectr would track up to 11G. To
achieve an effect requested of me, setting the tracking range to 11 or 12G is
required with the last threshold transition being a jump. The falloff should
be a natural value (6-12), for the first thresholds and very high (50+) for the
last. Trigger doesn't matter and can be as responsive (0 or 1) or sluggish (4+)
as you wish. The top threshold for every other part of the light should reflect
something unique and attention grabbing for this pattern as it will appear only
when a decent amount of tension plus a hard bounce trigger it. The pattern will
continue for a second+ depending on the fallout value.

Each mode also has it's own threshold values that correspond the different bins
of motion to the different values for each attribute in a mode's pattern and
colors. This allows multiple ways to adjust sensitivity - both tolerance of
small bursts of motion and amount of motion are configurable. The number of
threshold levels is configurable, with 1 level being a light with no motion
reactivity.


## One Whole

Patterns, colors, and motion are all highly configurable in Quark. This can be
a double-edged sword - with so many options, it's easy to make something
unusable. With so many options, it can be hard to make anything at all! The UI
will have to be a strong one for Quark to work, but for Quark to succeed it
just has to do what I've already done, be a one-size-fits-all set of functions
that take up less space and are easier to maintain and debug than all the
various pattern functions that make up all my other projects and do it without
being too slow for the controller. So, that out of the way, I've already got
what I want, what else do I need? The answer is everything. Well, I don't need
it, but in the words of my wife, I want it.

The UI's plan is to have both an advanced mode where all of the options are
exposed ala Vectr. I know this is too much for some, so the open nature of
Quark provides one other benefit - creating mode templates is an easily
scriptable task and building a UI to enable those tempplates to edit a light in
real-time is somewhat trivial. To make a tracer, it's easy to expose the option
to chose 9 colors (1 tracer + 8 slots in the second) with only static colors
plus two timing sliders for the tracer and strobe lengths. The timings could be
be linked to a dropdown of common timings for extra flair. This could be done
by anyone who can read instructions and understand a portion of what this guide
talks about. It might be easier to make the template-based UI where UIs are
dynamic based on the template might be easier to implement than a general UI
with all it's complexity (drag-n-drop support, color palettes UIs for 7 types,
visualization of the pattern and colors).

I think I'm going to take a crack at a dynamic UI that reads templates before I
try the open UI concept. It's probably both the smart choice from ease of
implementation and ease of use, but without the full editor no one will know
how to design new templates without quite a bit of iteration. Always more to
do, so now I must do it. Thanks for reading. Please comment. Goodbye.
