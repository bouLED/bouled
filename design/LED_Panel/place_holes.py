#!/usr/bin/python3
from math import sqrt, tan, sin, pi

# all values in mm

side = 179
height = side*sqrt(3)/2
margin = 12
t = 0.58

holes = []

holes.append([margin/tan(pi/6), margin])
holes.append([side - margin/tan(pi/6), margin])
holes.append([side/2, height - margin/sin(pi/6)])

for i in range(3):
    holes.append([t*holes[i][0] + (1-t)*holes[(i+1)%3][0], t*holes[i][1] + (1-t)*holes[(i+1)%3][1]])

print(holes)

