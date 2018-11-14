# Strobe

* up to 8 colors
* bunch of preset timings (strobe, strobie, dops, hyperstrobe)
* pick up to 4 levels of preset timings + jump option

1:
    palette mode
    pick 0/drop 1 (make redundant)

strobe          strobe 1
blank           blank and gap 1


# Candy Strobe

* up to 8 colors
* bunch of preset timings (strobe, strobie, dops, hyperstrobe)

1:
    palette mode
    pick 0/drop 1

strobe          strobe 1
blank           blank and gap 1
group size      pick 1 (2 or 3)
strobe count    strobe count 1 (min of 2 or 3)



# Tracer

* one color is tracer, up to 8 colors not tracer
* up to 4 levels + jump

1: tracer
    palette mode
    pick 1/drop 1

2: color
    palette mode
    drop 1

tracer strobe   strobe 1
tracer blank    blank 1
tracer count    strobe count 1
color strobe    strobe 2
color blank     blank 2
color count     strobe count 1
gap             gap 1 & 2
one-color       pick 2 (either pick 0 or pick 1)


# Halo

* up to 8 colors
* up to 4 levels + jump

1: front
    shift mode
    direction c alt
    effect to
    repeat count 2

2: center
    palette mode
    pick 1/drop 1
    repeat count 2

3: rear
    shift mode
    direction c alt
    effect from
    repeat count 2


center strobe   strobe 2
center blank    blank 2
center count    strobe count 2
edge strobe     strobe 1 & 3
edge blank      blank 1 & 3
edge count      strobe count 1 & 3
edge gap        gap 1 & 2
gap             gap 3
delta           shift delta 1 & 3


# Waveform

Hyperstrobe at low, dops wave at high

* up to 8 colors

1: wave strobe
    palette mode
    pick 0/drop 1
    change mode over alt

2: blank strobe
    one blank color in palette mode


hyper strobe    strobe and strobe end 1 low
hyper blank     strobe and strobe end 2 low
wave strobe     strobe and strobe end 1 high
wave blank max  strobe end 1 high (strobe 2 high is 0)
