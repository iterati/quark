# Modes

Let's quickly review the history of programmable LEDs for gloves and orbits (note: this might be
slightly inaccurate and isn't in chronological order. Instead it's in order of how powerful I
consider the firmware to be):

* One pattern and pick colors on a light-wide basis. (First lights)
* Preset patterns and colors on a light-wide basis. (Second gen)
* Pick a pattern and colors on light-wide basis. (Matrix/IMAX)
* Preset patterns and different colors per mode. (Oracles)
* Preset patterns and different colors with brightness per mode (Chromas).
* Pick a pattern and different colors with brightness per mode. (CTRLs/Aethers)
* Pick between preset motion reactive patterns and different colors per mode. (Kinetics)
* Pick up to two patterns, two sets of colors, and a motion option per mode. (Elements)
* Pick between motion reactive and non-motion reactive patterns, colors, and motion options for
non-motion reactive patterns. (Ubers)
* Pick up to three patterns, three sets of colors, and a sensitivity per mode. (Atoms)
* Edit pattern timings, color RGB values, and select a motion option per mode. (Spectras)
* Edit pattern timings, color RGB values, and motion across three levels per mode. (Vectr)

I'd put Primer somewhere between the Elements and the Spectras and along side the Atoms. As I began
developing light firmwares after being burned by the V1 Elements, the location of Primer makes
perfect sense. I strongly believe that the Spectras were a reaction to many of the things I built
in Vectr and that Vectr handily beats the Spectras in capabilities even though it was the
precursor.

As the lights grew more and more complex, the toolboxes grew. We started with just basic colors and
no options to edit patterns. Matrixes allowed us to alter the preset patterns and IMAXes allowed
chosing patterns, so patterns slowly became a tool in the box. Next was brightness levels, then
motion reactivity settings and sensitivities. Primer opened the door for custom color values. Vectr
added customizable patterns and timings and was followed by the Spectras. Vectr also added
customization for motion reactivity b to the toolbox with the thresholds function (too bad the "4
slider" UI was too confusing and I had to figure out a capable and less complicated alternative).


## Quark Modes

If we're going to add the future to the history list, here would be Quark's entry:

* Edit pattern timings and behaviors, colors and their behaviors, and motion thresholds and options
across four levels per mode. (Quark)

Including all the color options available in Quark, our toolbox is looking quite full:

* Pattern timings
* Pattern behavior
* Color values (limited by HBD color space)
* Color behavior
* Motion thresholds
* Motion behavior

Our modes are large, though. Very large. FL's Atoms use about 16 bytes per mode. EL's Elements are
slightly larger. Vectr's last public release with 16 colors used 192 bytes. I'm assuming the
Spectras use quite a bit with custom colors though no where near 192. Quark modes each reserve 768
bytes of memory, though a dozen or so are not used at this time. So not only do we have a lot of
tools, we have a large canvas to work with as well. Don't worry about filling up the canvas,
either, as simple sometimes looks as amazing as something all filled in.

Modes are comprised of motion options that define how the accelerometer works, four segments worth
of pattern timings and behaviors, and the segments' color palettes. While modes with single
segments may only take up around a quarter of the space available, they still pack many times more
expressiveness than a Vectr mode previously. While not all Vectr modes are possible with a single
segment in Quark, not all single-segment Quark patterns are possible in Vectr and that discounts
the options presented by the myriad color types. Many of the best modes will be simple. In the
same way, not every possible pattern is possible in Quark. While Quark allows for nearly every
pattern I have thought up to date, it's trivial to come up with one not possible by adding an edge
case not supported to an existing pattern. However, there are also many ways to present the same
pattern, so when trying to do something new be open to different ways to accomplish similar goals.
I will show some examples in future articles geared at this very problem.


## Interpolation

Many reading will know Vectr as a firmware and app that was my second major OSM project and my most
successful. At the heart of Vectr lies *vectr*, the motion tracking and interpolation engine that
powered the effects that made Vectr's effects possible. I have used *vectr* in other projects that
I have developed including the Ubers (it's why they're so damned good) and will continue using it
in Quark. It's fast, responsive, and simple. Trifecta. As mentioned in the motion article, each
mode can have it's own set of trigger and falloff values as well as threshold levels.

Every 20ms the active threshold level is determined which feeds the interpolation engine.
Interpolation is a fancy way of saying "smoothly increment from a starting value to an ending
value", and the particular type of interpolation used in *vectr* is linear interpolation, meaning
the increment amount is the same as you go. No curves here. Vector in physics means having a
position as well as momentum. In programming it (simplified) refers to a linear structure of data.
The two meanings are where the name of Vectr originates. That plus the horrible trend of removing
vowels from words in both music and startups which I ironically adopted because it's better to
embrace than hate and also I'm corny af. TI(U?)L folks.

Anyway, active threshold level, that's where the threshold values come in. Beyond idle (0), *vectr*
can track up to 128 different levels of motion. Since it's hard to tell the difference between 128
values as a human being, the thresholds will be mapped to values along those 128 levels and there
will be less choice. This will make it easier to find values that are meaningful while still
allowing for many steps between levels in turn making the blends smoother as you speed up and slow
down. That said, each motion-reactive value - timings, quantities, reactive colors, etc. - is
calculated on the fly based on the thresholds of the mode and level values of that value. Each
level transition (1 to 2, 2 to 3, and 3 to 4) can also be marked as "switch" transitions where the
value switches when the threshold is reached rather than blending.

Here's a simplified example of interpolation:

```
Example:

thresholds are 1, 3, 5, 7
switches are off, off, on (1-3 blend, 3-5 blend, 7 switch)
level values are 2, 6, 12, 20

active = 0  value = 2       < 2
active = 1  value = 2       = 2
active = 2  value = 4       between 2 and 6, not switching
active = 3  value = 6       = 6
active = 4  value = 9       between 6 and 12, not switching
active = 5  value = 12      = 12
active = 6  value = 12      between 12 and 20, but switching
active = 7  value = 20      = 20
active = 8  value = 20      > 20
```


## Reactivity

So modes define one part of the interpolation equation that is updated every 20ms (40 frames). The
rendering function will then interpolate new values it needs using that equation. This is a fancy
way of me saying that most values are updated immediately, but some values are updated only when
a new value needs to be generated. Halo deltas and reactive colors react immediately to changes in
motion. Strobe and blank lengths react when the current piece finishes. Long strobes and blanks
reduce responsiveness. If aiming for maximum responsiveness, use multiple pieces if possible.

Remember my baby, Darkside? Here she is. Not new, but improved once again:

```
Darkside of the Mode (Quark Edition 1.0)
1 segment
4 thresholds

                    level 1     level 2     level 3     level 4
strobe start:       3.0ms       60.0ms      7.5ms       1.5ms
blank start:        0.0ms       0.0ms       0.0ms       13.5ms
gap:                117.0ms     60.0ms      60.0ms      0.0ms
number of strobes:  1           1           8           8


Darkside of the Mode (Quark Edition 2.0)
2 segments
4 thresholds, switch from 2->3

Palettes:
Segment 1:
    1: Rainbow from red to red counterclockwise

Segment 2:
    1: Blank

Patterns:       level 1     level 2     level 3     level 4
Segment 1:
    strobe:     0.5ms       10.0ms      7.5ms       0.5ms
    blank:      0.0ms       0.0ms       0.0ms       7.0ms
    #strobes:   6           6           8           16

Segment 2:
    blank:      19.5ms      10.0ms      7.5ms       0.0ms
    #strobes:   6           6           8           1
```

The 2.0 Darkside is even more responsive to motion than the original. Each piece will have a
chance to update during the next loop due to no piece being longer than the 20ms cycle time. The
mode does not look as straight forward as in the past, but it's now better. The original would be
easily build and discovered through use of Quark with a sufficient UI, from there, it's easy to,
as we call it in software development, refactor the design into something that's more reactive to
motion. All it took was some simple math to break up the strobe pieces into smaller parts and to
use the blanks of the second segment as a replacement for a single segment mode with a gap. The
first segment will still only need one Rainbow color. The second segment needs no colors. Some
might wonder what happens if you expand upon that. What else is possible? Welcome to Quark.


## Possibilities

Primer made me realize how much there was to making patterns to blink a single light. It had 64 and
people wanted more. Vectr helped me realize the possibilities of allowing anyone to mess with
pattern timings and behavior, but also to understand that encompassing all patterns into one
framework was not in any way a simple task. I had to work on dozens of ideas that wouldn't fit my
previous molds to really find the building blocks. There are some not found that won't be in Quark.
There are many that have which cannot be included either due to my being unable to wrangle their
complexity into mine without a large increase in complication both to the underlying code and the
eventual UI. Oh no, only more possible modes than particles in the universe. Whatever shall we do?

Sarcastic self-deprecation aside, Quark really does allow for quite an endless variation of
patterns and color options that I find it preferable to design a mode as a Quark mode than to drop
into code and build it. For someone who would previously build his Vectr modes without using the
UI, that's saying something. Granted, I'm still just writing things in a text editor, but when the
UI is released, I'll likely never write a one-off mode again. The next article will break down some
of the modes I've built in the past for Vectr and the Ubers, and I'll explain how to build it as a
Quark mode possibly improving it in the process.
