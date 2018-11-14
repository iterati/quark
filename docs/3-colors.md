# Colors

Previously I explained in-depth how segments were defined and how to combine them to form some of
the more advanced modes in other lights and showed how most advanced modes are fairly easy to
recreate in Quark. However, due to patterns having very little to do with color, morphs are not
possible in this framework. Morphs having to be part of the pattern has always rubbed me the wrong
way. The most basic reason is that for a tracer to have a morph, a pattern would have to be crafted
specifically to allow morphing tracers. There's no amount of tweaking small parts of Vectr to make
the morphing tracer work, you actually have to build a pattern specifically around that idea. If
then I wanted a morphing edge, I'd be in the same boat. I built Vectr so I'd have to drop down to
code less when designing modes, not more. So for the longest time, morphing was a problem.

Eventually I had the breakthrough about having different type of colors - colors that reacted
differently over time and motion. Instead of how the color behaves being a part of the pattern,
the color's behavior is self-contained and can be used in any context. Make a morphing pattern and
you have a morphing version of every pattern. Make a morphng color, and every pattern now supports
morphing.

I'm not sure if these 7 categories of colors are the final 7 or not. While work on Vectr helped
guide pattern segments to a level of clarity and utility that allows for almost anything, my work
on color has just begun.


## Basic Colors and HBD

The first type of color is the simplest and the one familiar - the basic, static color. This is the
type of color provided by every current light. Even still, due to changes necessary to enable the
other forms of color, Quark's basic colors are still an improvement on the past.

Vectr used RGB color at the core. Every color was a set of 3 values from 0 to 255. This allowed for
full control over the hardware. The LEDs can show that much range so Vectr allowed it. However, no
one used the full range. To start with, due to the nature of RGB LEDs, brightness is difficult to
maintain across multiple colors. With full control over the brightness of each color channel, the
user had to do his or her best to maintain the brightness levels between different colors and there
was no help from the UI to make that any easier. In my usage of Vectr and my OSMs, I was able to
find a color palette that I considered balanced across the color wheel using some equations and
approximations, and I've used that as the basis for my color system.

As mentioned in the intro, the colors are based on HSV (hue, saturation, and value) in a format I
prefer to dub HWD (hue, whiteness, dimness) due to the way I store and use the values. There are
24 possible hues to chose from. The diagram below gives a good idea of 12 of the wheel's hues.
There are colors between each pair that make up the full 24.

```
                red
        pink            orange

    magenta                 yellow


purple                          lemon


    blue                    green

        sky             seafoam
                cyan
```

Whiteness is the measure of how much white is in the color. There are 4 levels of whiteness plus
white - no white (full color saturation), less white, medium white, more white, actually white.
24 hues * 4 levels of white = 96 color palette. And we haven't gotten to dimness yet. Dimness is
like whiteness in that there are 4 levels plus a blank level.

That's quite a few colors to chose from. Basic colors, in spite of being old hat, still have a new
spin in Quark. HSV for colors has a number of advantages, such as allowing color wheel
manipulations, but the biggest advantage is how much better it is for picking colors as a human.
Not only is picking a hue, then a level of whiteness, then a level of dimness a better workflow for
color selection, the algorithm does the work of leveling the brightness between all 96 hue and
whiteness combinations.


## Compound

Compound colors are an idea more geared to orbits than gloves. The concept of compound colors is
that each frame of a strobe is a different color. Pretty simple concept, but pretty impressive
results. With up to 4 colors in sequence, the possibilities are significant. I'm not sure if this
will be one of the more widely used color types, but it's a nice option to have. There's nothing I
can point to which would demonstrate compound colors.


## Reactive

Reactive colors are based on how color blending worked in Vectr. Pick up to 4 colors and, depending
on speed the color displayed is a blend of these colors. To see reactive colors in action, fire up
Vectr to see a 3 color version.


## Morph

Morph colors are time-based color blends between up to 4 chosen colors. This is the morph other
lights offer but timed rather than tied to a pattern. What this means is that the morph doesn't
start and stop based on the pattern displayed but changes over time whether the color is being
displayed or not. The timer is always ticking. The speed of the morph is motion reactive. While
this is similar to morphs in other lights, the time-based nature of it makes it quite unique.


## Streak

Streaks colors are the closest to classical morphing patterns. Again, you may chose up to 4
colors to morph between and the colors will morph from one to the over a single segment. To make
streaks motion reactive you should have the pattern lengthen or shorten as there is nothing built
into streak colors to make them motion reactive. Streak is on display in the Ubers as the Prism
motion mode.


## Blend

Blends are color wheel rotations that take place over time. While Morphs are RGB blends and take
place in that color space, Blends take place in the HSV realm and as such have additional data
associated with how to be displayed. At the core, blends take two colors - a starting point and a
destination. The blend will start at the first color and rotate to the second. The direction of
the rotation can be chosen as well as if the blend jumps from the end back to the start or if the
blend reverses directions from end to start when it reaches the destination. As with Morph, this is
a time-based color with the speed of blend being motion reactive.


## Rainbow

Finally, Rainbow colors are to Blends as Streaks are to Morphs - a blend across a segment rather
than over time. These colors only exist for one reason - Darkside of the Mode. Darkside is my
signature mode and I've been working on it since I got a pair of Element V1s. The basic gist of the
mode is that at low speed you see a rainbow streak and at high speed you see a rainbow of strobes.
The elements used a Blink-e/Strobie combo, Primer was similar but had a shorter Blink-e strobe
length, Vectr had a middle level so that the Blink-e portion grew longer as you went faster to
accentuate movement, and finally, a personal firmware removed colors and started just using hue and
time to calculate the color so that the streak was always a perfectly balanced rainbow.

Rainbow colors allow you to chose a starting color, destination color, and the direction of blend.
The blend takes place across a single segment. Darkside's never been so easy:

```
Darkside of the Mode (Quark Edition)
1 segment
4 thresholds

                    level 1     level 2     level 3     level 4
strobe start:       3.0ms       60.0ms      7.5ms       0.5ms
blank start:        0.0ms       0.0ms       0.0ms       7.0ms
gap:                117.0ms     60.0ms      60.0ms      0.0ms
number of strobes:  1           1           8           16
```

That pattern plus a palette with a single rainbow color is all it takes to recreate one of the
modes I am most fond of. In other firmwares it never quite did what I conceptualized until I took
the time to build code specifically for it. While Rainbow colors could be thought of in that light,
they could easily prove useful in other contexts. The best part about finding the funamentals is
that it enables taking an idea born in one context and applying it in another, and these colors
will be fun applying to other concepts.


## Structure

With so many options, it's intersting how simple a color still is:

* Color type
* Hue, whiteness, brightness values for 4 colors
* Speed for Morph and Blend colors
* Direction of Blend and Rainbow colors

The behavior is all in the type, which is another reason I consider colors at an earlier stage than
patterns. Patterns were all about type in Vectr and now they're generalized and broken into smaller
and more fundamental parts.

Each pattern segment has an palette of up to eight colors. Since every color can have up to four
constituent colors, a Quark mode can contain up to 128 colors. In fact, colors take up nearly 80%
of a Quark mode's space.

With patterns and colors explained, it's possible to understand how many endless possibilities
exist for modes in Quark. The missing element is, in my view, the most important - motion. Motion
is at the heart of every firmware I build now, and motion is at the heart of Quark. Without motion,
all the expressiveness of the colors and patterns would fall flat for me. The next article will
focus on motion and putting all of the parts together.
