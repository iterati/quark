# Introducing Quark

"A quark is an elementary particle and a fundamental constituent of matter."
- Wikipedia definition of Quark

Since I became interested in making things blink, my mind has always looked for the funamentals
of blinking things. How many ways can you blink a light? Quite a lot it turns out. Being a
developer, I can make a light blink however I want. Making a light that can easily be told to
blink however one wants, however, requires more programming skill than most people would want.
I built Vectr to bridge that gap somewhat. The firmware is very flexible (and complex) and the
UI exposes all of that flexibility (and complexity) in what can at best be described as a
utilitarian way.

Even with the flexibility and complexity of Vectr, there's still much ground uncovered.  I've
spent the time since building new firmwares (the Ubers from LEDGloves.com - check them out and
I'm sure you'll find something in there that you never thought a glove would do) and in doing
so I've found a million ideas on how to blink a light and have tried over and over to break
them all down and find a fundamental way of describing them.

See, the more you break down how to blink the more you can build a UI that lets you build
anything. Vectr's utilitiarian UI has born dozens upon dozens of modes not possible elsewhere
in spite of it's steep curve and small userbase and limited customization. Sure, there are a
number of base patterns, but they are preset and you cannot blend between them. To allow for
blending between various patterns, patterns needed to be broken down more and color-based
effects had to mostly be removed from patterns. As such, Quark, which is what I'm calling my
new project, tackles both problems by being even more complex than before. However, as a
developer, my philosophy about complexity is that complexity is part of the nature of any
system large enough to be worth modeling. Complication is something that humans bring into
complexity. By breaking things down to their fundamental elements, the complex nature of
systems can be revealed and through careful reconstruction of these complex pieces into a
working model, the end result can be powerful, flexible, usable, and, most importantly,
expressive.

## On Patterns

Vectr had up to 10 different base patterns in different working flavors during active
development. These patterns were very similar in code except for how certain events would
trigger and behave. Morph was essentially Strobe but the color morphed. Tracer and Double/
Multi were similar but for how colors were chosen/alternate. Many options were not possible
due to only timings being motion reactive - tracers with multiple tracer strobes couldn't
increase/decrease the number of strobes based on motion. Tracers couldn't morph due to morph
being a mode rather than some other trait of colors. All of these issues needed to be untangled
before finding the fundamentals as sought. The first I figured out was morphing, but I'm saving
that one for the Colors segment.

Oddly enough, the second wasn't even mentioned above. I solved the issue of Strobe, Tracer,
and Edge being similar, and it happened to be the biggest shift mentally for me. To understand,
you need a map. To read the map, you're going to need examples.


```
R.G.B.R.G.B.    3 Color Strobe
R.G.R.B.R.G.    3 Color Tracer
BGRGB.BGRGB.    3 Color Edge

R - Red
G - Green
B - Blue
. - Blank
```

See those lovely examples? Simple, yeah, but they do the job. It shows how different the three
patterns are, but it'll be easier to see how I break them apart in a moment.

```
Strobe segments:
R. or G or .B.

Tracer segments:
R.
G. or B.

Edge segments:
BG
R
GB
.
```

So there are different segments in these patterns. The segments have different palettes. Some
are just empty space. Hmm. We can work with this. So if a pattern is 4 segments big, segments
can be combined to build complex patterns. How complex? Depends on how complex each segment can
be. Segments are pretty complicated in my world. I'll talk about them a little more next time.
It's time to talk color.


## On Color

As mentioned, blend was the first thing about patterns I figured out. But it wasn't blending as
in morphing as you're accustom to. It was based on most of my personal modes which, I have to
be real, use a LOT of color wheel blends. True color wheel blends require using HSV color rather
than RGB. RGB is native to the LED. It has red, green, and blue diodes. So I wrote my own HSV
library that, while not entirely accurate and a little limited, provides 24 main colors with
4 levels of saturation (how much white is in them/how pastel they are) and 4 levels of
brightness. Oh, plus white (with 4 levels of brightness) and blank. It's also really fast on 8
bit hardware as it has to be to handle all the rest of my crazy ideas. I love my fucking color
wheel blends, OK?

So colors are now defined by their place in the color wheel (1 is red, 9 is green, 17 is blue),
how white they are (is calling colors white politically correct?) and how dim they are (I know
this one is borderline). How else are they defined? I did some thinking and came up with 5 ways
to show colors. Then because I'm crazy I came up with a 5th and 6th one. You can put any of
these color types into any color slot in a Quark mode.

* Basic - Color doesn't change at all. Pick red and it's red.
* Compound - Every frame (0.5ms) it switches to the next color. You can have up to 4 colors.
This will look interesting on orbits.
* Reactive - Kinetics did it first, I did it better, EL stole the Kinetics, and then Atoms
upped the ante (9 colors!). This is only 4 color flux but you can have up to 8 of them per
segment.
* Morph - Here's where traditional morphing went. Instead of it being over a certain number of
strobes, it's over time so your lights might never go out of sync. Or they might if you decide
to make the blend speed motion reactive. You can choose up to 4 colors for your morph and you
can have up to 8 sets of morphing colors. I'm not even sure what that's going to look like yet.
* Streak - Like morph, but the morph takes place acros a segment.
* Blend - My mojo - Blends from one color to another on the color wheel. Can either blend one
way or to-from. Blend speed is motion reactive.
* Rainbow - Like blend, but the blend takes place across a segment.

So those are all the ways to color that I can think of, and they are they ways to use colors in
Quark. Another advance is that each segment has it's own palette. As shown in the examples,
Tracer is really two segments with two palettes:

```
Segment 1:
pattern: X.
pallete: A

Segment 2:
pattern: X.
palette: B C
```

And Edge could be 4 segments with four palettes:

```
Segment 1:
pattern: XY
palette: C B

Segment 2:
pattern: X
palette: A

Segment 3:
pattern: XY
palette: B C

Segment 4:
pattern: .
palette: none
```

Strobe's simply:

```
Segment 1:
pattern: X.
palette: A B C
```

## Just the Start

With just these simple parts, it's easy to visualize quite a lot of possibilities. Of course,
like Vectr, you will be able to vary the timings of each part of each pattern by motion and to
whatever length between 0 and 125ms in 0.5ms increments. 389 colors to chose from (blending
makes for millions of possible colors displayed) with 5 totally different ways to behave will
round out the package making for this to be a studio for designing modes that far exceed what
was possible in Vectr, and Vectr showed there was quite a lot more possible than even the
Spectrums would provide. My next article will focus on how the patterns inside segments are
defined and how they interact with your color palette.
