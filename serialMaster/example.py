#!/usr/bin/python
from ardSerial import *

import serial
import struct
import sys
import time
import math
import numpy as np


schedule = [['m',[1,45],1],\
            ['d',5],\
            ['m',[1,-30],1],\
            ['d',5],\

            ['d',5],\

            ['d',5],\

            ['d',5],\

            ['d',5],\
            ['b','10','255',1],\
            ['ktr',10]]

for task in schedule:
    wrapper(task)
for i in range(1000):
    #serialWriteByte(["i","0",str(int(15*math.sin(2*math.pi*i/100))),"1",str(int(15*math.sin(4*math.pi*i/100))),"2",str(int(15*math.sin(2*math.pi*i/100)))])
    #serialWriteNumToByte("i",[0,int(15*math.sin(2*math.pi*i/100)),1,int(15*math.sin(4*math.pi*i/100)),2,int(15*math.sin(2*math.pi*i/100))])
    #serialWriteByte(["m","1","10"])
    #ser.write("m1 10\n")
    
#    wrapper('i',[0,30*math.sin(4*math.pi*i/500),1,30*math.sin(4*math.pi*i/500+math.pi/2),2,30*math.cos(4*math.pi*i/500)])                             #serialWriteNumToByte(['c'])
    if i%250==0:
        print("should stop")
        time.sleep(1)
        print("should start")
    time.sleep(0.012)

