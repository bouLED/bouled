# Facet design files

## Prerequisites

`solidpython` and `openscad` are needed. You may install solidpython using
`pip install pysolid`.

## Running

Run `make` to generate `78leds.scad`, `78leds.txt` and `78leds.dxf` (the `txt`
and `dxf` files are meant to be used during the PCB layout of the triangles
in Xpedition PCB).

The led positions are written by the script in `78leds.txt`, relative to the
bottom-left corner of the triangle.

The layout is also exported in `78leds.dxf` by the script.
