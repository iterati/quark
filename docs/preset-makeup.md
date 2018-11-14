mode:
nums is defined
numt is defined, slider?
  - determines what the global NUMT value is
falloff and trigger defined, range or dropdown
thresh values and jump are defined, special widget

each segment:
numc is defined, slider?
types of colors available can be limited
change mode is defined
color mode is defined
    palette or reverse:
    pick is defined, dropdown, checkbox
    drop is defined, checkbox

    shift:
    rotation is defined, dropdown
    effect direction is defined, dropdown
    delta is defined, slider

dim mode is defined, dropdown
strobe/end, blank/end, gap, and counts either defined or part of "Meta Widgets"

meta widgets:
    label
    description
    default value
    widget + params (slider, dropdown, checkbox, or custom)
    targets (updates 1->1)
    on-change (updates not 1->1)
    listens-to (changes in this global value update me somehow)

    slider:
        min/max
        display (function to alter display value if not 1->1)

    dropdown:
        options (display name/value combos)
        default

    checkbox:
        <none>


