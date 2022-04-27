#!/usr/bin/python3
# -*- coding: UTF-8 -*-
# autopep8 -i

# A complex skill to make Bittle climb up a steel ceiling with meganets on its feet.
# Demo: https://youtu.be/ravFaiJzrpo
import sys
sys.path.append("..")
from ardSerial import *

if __name__ == '__main__':
    try:
        time.sleep(3)

        '''
        testSchedule is used to test various serial port commands
        '''
        testSchedule = [
            # - 'kbalance' indicates the command to control Bittle to stand normally
            # - 2 indicates the postponed time after finishing the command, in seconds
            ['g', 0.5],
            ['kbalance', 1],
            ['ksit', 2],
            ['I', [11, 75, 15, -30, 8, -90, 12, 45, 9, 20, 13, 90], 0.5],
            ['I', [8, -100, 12, 30], 0.5],
            ['m', [8, -128, 12, 85], 1],
            ['m', [9, -100, 13, 45], 0.5],
            ['I', [9, -128, 13, 85, 10, 70, 14, -20, 15, -20], 1],
            ['I', [8, -80, 9, -80, 12, 10, 13, 10, 10, 127, 11, 127, 14, -80, 15, -80], 0.5],
            ['I', [10, -15, 11, -15, 14, 70, 15, 70], 1.5],
            ['I', [8, -70, 9, -70, 12, -10, 13, -10, 10, 30, 11, 30], 0.5],
            ['I', [8, 50, 9, 50, 12, -50, 13, -50, 10, 127, 11, 127, 14, 90, 15, 90], 1],
            ['m', [11, -20, 15, 40], 1],
            ['m', [15, 90], 1],
            ['m', [14, 0, 10, -20], 1],
            ['I', [14, 90, 10, -20], 1],
            ['kbalance', 2],
            ['i', [0, -80, 1, 20], 2],
            ['d', 1],
        ]
        serialObject = connectPort()
        if serialObject!=-1:
            for task in testSchedule:  # execute the tasks in the testSchedule
                print(task)
                sendTask(task)

            schedulerToSkill(testSchedule)
            closeSerialBehavior()
            logger.info("finish!")

    except Exception as e:
        logger.info("Exception")
        closeSerialBehavior()
        raise e
