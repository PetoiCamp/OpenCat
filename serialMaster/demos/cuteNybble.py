#!/usr/bin/python3
# -*- coding: UTF-8 -*-

import sys
sys.path.append("..")
from ardSerial import *
import math
import numpy as np

if __name__ == '__main__':
    try:
        flushSerialOutput(300)

        serialWriteByte(["ksit"])
        time.sleep(2)
        serialWriteByte(['i', '2', '-90', '10', '-80', '11', '-80', '14', '-10', '15', '-10'])
        time.sleep(1)
        serialWriteByte(['i', '8', '-10', '9', '-10', '10', '-60', '11', '-60'])
        time.sleep(0.5)
        while True:
            for a in np.arange(0, 5 * math.pi, 0.4):
                # print(a)
                tilt = math.cos(2*a)
                serialWriteByte(['I', '0', str(int(30 * math.sin(a))), '1', str(int(15 * tilt - 60)), '12', str(int(20 * tilt + 50)), '13', str(int(-20 * tilt + 50))])
                time.sleep(0.13)

            time.sleep(1)
            x = input()
            if x != "":
                # print(x)
                if x == "q" or x == "quit":
                    break
                elif x == "co":
                    continue

        closeSerialBehavior()
        logger.info("finish!")

    except Exception as e:
        logger.info("Exception")
        closeSerialBehavior()
        raise e
