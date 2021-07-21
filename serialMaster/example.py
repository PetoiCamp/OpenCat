#!/usr/bin/python
from ardSerial import *
import serial
import struct
import sys
import time
import math
import numpy as np

try:
    print("phase 1\n")
    schedule = [['m', [1, 45], 1],
                ['d', 5],
                ['m', [1, -30], 1],
                ['d', 5],
                ['d', 5],
                ['d', 5],
                ['d', 5],
                ['d', 5],
                ['b', '10', '255', 1],
                ['ktr', 10]]

    for task in schedule:
        wrapper(task)

    print("phase 2\n")
    for i in range(1000):  # take a long time
        # serialWriteByte(["i",
        #                  "0",
        #                  str(int(15 * math.sin(2 * math.pi * i / 100))),
        #                  "1",
        #                  str(int(15 * math.sin(4 * math.pi * i / 100))),
        #                  "2",
        #                  str(int(15 * math.sin(2 * math.pi * i / 100)))])
        #
        # serialWriteNumToByte("i", [0,
        #                            int(15 * math.sin(2 * math.pi * i / 100)),
        #                            1,
        #                            int(15 * math.sin(4 * math.pi * i / 100)),
        #                            2,
        #                            int(15 * math.sin(2 * math.pi * i / 100))])
        #
        # serialWriteByte(["m", "1", "10"])
        #
        # ser.write("m1 10\n".encode())
        #
        # wrapper(['i', [0,
        #                30 * math.sin(4 * math.pi * i / 500),
        #                1,
        #                30 * math.sin(4 * math.pi * i / 500 + math.pi / 2),
        #                2,
        #                30 * math.cos(4 * math.pi * i / 500)],
        #          1])

        # serialWriteNumToByte(['c'])

        if i % 250 == 0:
            print("should stop")
            time.sleep(1)
            print("should start")
        time.sleep(0.012)

except Exception as e:
    wrapper(['d', 0])
    close_serial(ser)
    print("exit")
    raise e
