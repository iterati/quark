# Motion

I won't use non-motion reactive gloves anymore. I've spent well over a thousand hours building
motion reactive firmwares and playing with motion reactive gloves. Motion feels right for me when I
flow and removing it feels like removing part of my ability to express things when I flow. Motion
is very hard to get right, though, and thresholds can be very personal. Vectr users will often
tweak their thresholds over weeks to get their gloves feeling like **their** gloves and the results
make for the most expressive motion reactivity available. The biggest failing of Vectr's thresholds
was due to some early decisions I made about how to handle motion and the "4 slider" threshold
system which, for all it's abilities, is somewhat of a failure in usability. The goal was to have
the ability to either blend between levels or jump between them as in the Elements, the Atoms, and
in Primer. I also wanted the ability to have some form of "dead zone" in the middle so blends could
occur in the low end and high end with the middle being static for a range of speeds. The original
solution worked, but the function of the 4 sliders was difficult for many to grasp and often they
would remain untouched removing one of the more powerful features of the firmware.

The 4 slider system worked in this way. The light tracked motion over a range from 0 (no motion) to
64 (max level tracked by the accelerometer before losing accuracy). The range was from 1G at 0 to
5G at 64 for gloves and 1G to 11G for orbits. With 4 sliders in 2 sets of 2, three levels could be
controlled. The first two sliders controlled when the blend from the first level to the second
would start and end. If the values were equal, then there was no blend and the pattern would switch
from the first to the second the moment the threshold was reached. The second set of sliders worked
for the second and third level. Maxing out either set of sliders would disable the level for those
thresholds allowing you to easily create a 1-, 2-, or 3-level mode. Having the second and third
sliders equal would create a smooth blend while having distance between the two values would create
a deadzone for that range.


## Thresholds

Quark recreates this without the use of any double sliders, instead using threshold values that are
individual and checkboxes to mark if the pattern is to blend from level to level. Quark also
expands to four levels because why not? It makes deadzoning possible in the new system by having
the second and third levels match at different threshold values. The UI will be much more straight-
forward with the single slider and checkbox over the multi-slider, and I won't have to build the UI
component myself from scratch. All wonderful things.


## Triggered

The accel core of Quark is still the same as Vectr. Vectr, at the heart, is velocity tracking and
interpolation engine. The patterns, colors, and UI are important from a "pattern and mode"
perspective, but I built Vectr to blend patterns and colors based on motion and that was where the
firmware started and where the most serious effort went. Lessons learned in Lumino and Primer were
honed into the motion tracking algorithm. While not perfect, it does an admirable job of tracking
arbitrary motion with accelerometers and determining how much motion is occuring. The algorithm is
fast and allows me to determine the level of motion within 4ms of reading the accelerometer values
without taking up more than 10% of my processing cycles for each frame.

The algorithm to determine the level of motion uses two "magic" values that define how quickly the
light reacts to a higher level of motion (the **trigger** value) and how quickly the light resets
to a lower level of motion (the **falloff** value). Every accelerometer cycle (every 20ms), each
threshold is processed from lowest to highest. If the threshold has not been reached, the falloff
is incremented. Should the falloff for a threshold meet or exceed the falloff value, the trigger
value is reset to 0. If the force detected is above the threshold's value, the trigger is
incremented. Should the trigger meet or exceed the trigger value, the threshold is considered
active. The highest active threshold is the value used for all motion reaction calculations.

Increasing the trigger value reduces the sensitivity of detecting increases in motion while
lowering the value increases the sensitivity. Increasing the falloff value reduces the sensitivity
of detection for reductions in motion while decreasing the value increases the sensitivity.


## Laid Out

The motion values of a Quark mode are core to how the pattern and colors operate. The values are:

* Falloff for motion tracking
* Trigger for motion tracking
* Four threshold levels
* Three threshold blend/switch options

The motion options take up the least room in a Quark mode but have the highest impact. Read the
next article to see how how it all comes together.
