#!/usr/bin/env python3

from solid import *
from numpy import sqrt
from numpy import tan, sin
from math import pi

import sys

if len(sys.argv) < 3:
	print("Usage: ./triangle.py scad_filename text_filename")
	sys.exit(1)
scad_filename = sys.argv[1]
text_filename = sys.argv[2]

# Triangle side
side=179
height = sqrt(3)*side/2

led_w = 5
led_h = led_w
thickness = 0.01

# difference between x of corner and x of LED side at the edge of the triangle
eps = led_h / (2*tan(60*2*pi/360));

# Clearance to border, along axis
clearance_y=6
clearance_x=clearance_y*(1/sin(60*2*pi/360) + 1/tan(60*2*pi/360))

# Number of LEDs on the lower row
n = 12
ledcounts = [n-i for i in range(n)]
print("Number of leds: " + str(int(n*(n+1)/2)))

# Inner triangle's dimensions
side2 = side - 2*clearance_x
height2 = side2*tan(60*2*pi/360)/2

led_positions = [] # led centers relative to the triangle's bottom left

def triangle():
	d = difference() (
	polygon(points=[[-side/2,0], [side/2,0], [0,height]]),
	polygon(points=[[(-side/2)+thickness,thickness], [(side/2)-thickness,thickness], [0,height-thickness]])
	)
	return d

def led():
	d = difference() (
	square([led_w,led_h], center=True),
	square([led_w-thickness,led_h-thickness], center=True)
	)
	return d

def row(startpos, nb, avail):
	d = []
	if nb == 1:
		# center
		d.append(translate([0, startpos[1]]) (
		led()
		))
		led_positions.append([side/2, startpos[1]])
	else:
		spacing = led_w + (avail-(nb-1)*led_w) / (nb-1)
		#print("Espacement horizontal:" + str(spacing))
		if spacing <= 0:
			print("Too many LEDs on the row: " + str(nb))
		for i in range(nb):
			d.append(translate([startpos[0] + i*spacing, startpos[1]]) (
			led()
			))
			led_positions.append([startpos[0] + i*spacing + side/2, startpos[1]])
	return union()(*d)

def leds():
	nb_lines = len(ledcounts);
	starty = clearance_y;
	# yspacing is the spacing between centers
	# height so that the last LED doesn't overflow the inner tri.
	h_marg = height2*(1-led_w/side2)
	# Available height between the two extremal centers
	avail_y = height2
	interled_y = (avail_y - (nb_lines-1)*led_h)/(nb_lines-1)
	yspacing = led_h + interled_y
	#assert (interled_y > 0, "Too many rows");
	d = []
	for i in range(nb_lines):
		y = i*(yspacing)
		# Thales' theorem gives that, it's the width of the
		# triangle
		L = side2*(1-y/height2)
		# Available length (i.e length between the two extremal centers
		avail_x = L
		nb = ledcounts[i]
		d.append(row([-avail_x/2, starty+i*yspacing], nb, avail_x))
	return union()(*d)

def face():
	return triangle() + leds()

with open(scad_filename, 'w') as f:
	print(scad_render(face()), file=f)

with open(text_filename, 'w') as f:
	for x, y in led_positions:
		print(str(x) + "," + str(y), file=f)
