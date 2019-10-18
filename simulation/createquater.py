#Test for the simulation : sending quarternion to output standard

import random
import sys
import threading
import time
from math import cos,sin,radians


def createQuaternion(attente):
    angle        = 0
    rotation     = 0
    while True:
        rotation   = rotation + 5
        quaternion = [cos(radians((angle+rotation)/2)),
                      -1*sin(radians((angle+rotation)/2)),
                      0,
                      0]
        sys.stdout.write("%s"%quaternion[0]+","+"%s"%quaternion[1]
                            +","+"%s"%quaternion[2]
                            +","+"%s"%quaternion[3]+"\n")
        time.sleep(attente)

#send quaternion every 50ms
createQuaternion(0.05)
