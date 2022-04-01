#!/usr/bin/python3
# -*- coding: UTF-8 -*-

import sys
sys.path.append("..")
from ardSerial import *
# the following skill arrays are identical to those in InstinctBittle.h
sit = [
1, 0, -30, 1,
    0,   0, -45,   0,  -5,  -5,  20,  20,  45,  45, 105, 105,  45,  45, -45, -45]
ck = [
    -2, 0, 5, 1,
    0, 1, 2,
    45,   0,   0,   0,   0,   0,   0,   0,  45,  35,  38,  50, -30, -10,   0, -20,     6, 1, 0, 0,
   -45,   0,   0,   0,   0,   0,   0,   0,  35,  45,  50,  38, -10, -30, -20,   0,     6, 1, 0, 0,
]
pu = [
-8, 0, -15, 1,
 6, 7, 3,
    0,   0,   0,   0,   0,   0,   0,   0,  30,  30,  30,  30,  30,  30,  30,  30,     5, 0, 0, 0,
   15,   0,   0,   0,   0,   0,   0,   0,  30,  35,  40,  29,  50,  15,  15,  15,     5, 0, 0, 0,
   30,   0,   0,   0,   0,   0,   0,   0,  27,  35,  40,  60,  50,  15,  20,  45,     5, 0, 0, 0,
   15,   0,   0,   0,   0,   0,   0,   0,  45,  35,  40,  60,  25,  20,  20,  60,     5, 0, 0, 0,
    0,   0,   0,   0,   0,   0,   0,   0,  50,  35,  75,  60,  20,  30,  20,  60,     6, 0, 0, 0,
  -15,   0,   0,   0,   0,   0,   0,   0,  60,  60,  70,  70,  15,  15,  60,  60,     6, 0, 0, 0,
    0,   0,   0,   0,   0,   0,   0,   0,  30,  30,  95,  95,  60,  60,  60,  60,     6, 1, 0, 0,
   30,   0,   0,   0,   0,   0,   0,   0,  75,  70,  80,  80, -50, -50,  60,  60,     8, 0, 0, 0,
   ]
pee = [
-4, 0, 10, 1,
 2, 3, 3,
   30,  20,   0,   0,  15, -10,  60, -10,  40,  40,  90,  45,  10,  60,  70,  45,     6, 0, 0, 0,
   45,  20,   0,   0,  15, -10,  60, -10,  60,  53, 115,  60, -30,  40,  50,  21,     2,10, 0, 0,
   30,  20,   0,   0,  15, -10,  60, -10,  40,  40,  90,  45,  10,  50,  70,  45,    16, 0, 0, 0,
   30,  20,   0,   0,  15, -10,  60, -10,  40,  40, 103,  45,  10,  50,  80,  45,    16, 0, 0, 0,]
   
vt = [
21, 0, 0, 1,
  57,  43,  60,  47, -18,   7, -17,   7,
  50,  43,  53,  47,  -5,   7,  -5,   7,
  43,  43,  47,  47,   7,   7,   7,   7,
  43,  43,  47,  47,   7,   7,   7,   7,
  43,  43,  47,  47,   7,   7,   7,   7,
  43,  47,  47,  50,   7,   0,   7,   0,
  43,  54,  47,  58,   7, -13,   7, -13,
  43,  60,  47,  65,   7, -25,   7, -25,
  43,  66,  47,  71,   7, -35,   7, -35,
  43,  63,  47,  67,   7, -30,   7, -29,
  43,  57,  47,  60,   7, -18,   7, -17,
  43,  50,  47,  53,   7,  -5,   7,  -5,
  43,  43,  47,  47,   7,   7,   7,   7,
  43,  43,  47,  47,   7,   7,   7,   7,
  43,  43,  47,  47,   7,   7,   7,   7,
  43,  43,  47,  47,   7,   7,   7,   7,
  47,  43,  50,  47,   0,   7,   0,   7,
  54,  43,  58,  47, -13,   7, -13,   7,
  60,  43,  65,  47, -25,   7, -25,   7,
  66,  43,  71,  47, -35,   7, -35,   7,
  63,  43,  67,  47, -30,   7, -29,   7,
]

wkF = [
    54, 0, 0, 1,# 54 is the number of frames. it should not be larger than 127
   9,  49,  53,  45,  24,  20,  -2,  15,
   8,  50,  41,  46,  28,  21,  -1,  15,
  10,  51,  26,  47,  26,  22,   6,  16,
  12,  52,  23,  48,  24,  24,   9,  17,
  14,  52,  20,  49,  22,  26,  12,  18,
  16,  53,  17,  51,  21,  27,  17,  18,
  18,  53,  14,  52,  20,  29,  22,  19,
  21,  54,  11,  54,  18,  30,  27,  19,
  22,  54,  11,  54,  18,  32,  29,  20,
  25,  54,  13,  55,  16,  34,  27,  21,
  26,  54,  16,  56,  16,  37,  24,  23,
  28,  54,  18,  56,  15,  39,  23,  24,
  30,  52,  20,  57,  14,  45,  22,  26,
  32,  54,  22,  57,  14,  44,  21,  28,
  33,  58,  24,  57,  15,  36,  20,  30,
  34,  61,  26,  57,  15,  31,  19,  32,
  36,  64,  28,  57,  14,  24,  18,  35,
  38,  66,  29,  57,  14,  20,  18,  38,
  39,  67,  31,  57,  14,  16,  17,  40,
  41,  64,  32,  56,  14,   5,  17,  43,
  42,  55,  35,  57,  14,  -1,  16,  44,
  44,  44,  37,  62,  15,  -3,  14,  35,
  45,  30,  39,  66,  15,   1,  14,  29,
  46,  21,  40,  68,  15,   5,  14,  23,
  47,  19,  42,  70,  16,   9,  14,  19,
  48,  16,  43,  70,  17,  12,  15,  17,
  49,  12,  44,  67,  18,  17,  15,   5,
  49,   9,  46,  59,  20,  24,  15,  -2,
  50,   8,  47,  47,  21,  28,  16,  -2,
  51,  10,  48,  34,  22,  26,  16,   1,
  52,  12,  49,  24,  24,  24,  17,   6,
  52,  14,  50,  21,  26,  22,  18,  10,
  53,  16,  51,  19,  27,  21,  19,  12,
  53,  18,  52,  15,  29,  20,  20,  19,
  54,  21,  54,  12,  30,  18,  19,  24,
  54,  22,  55,  12,  32,  18,  20,  27,
  54,  25,  55,  11,  34,  16,  22,  29,
  54,  26,  56,  14,  37,  16,  24,  26,
  54,  28,  56,  17,  39,  15,  25,  24,
  52,  30,  57,  18,  45,  14,  27,  23,
  54,  32,  57,  21,  44,  14,  29,  21,
  58,  33,  57,  23,  36,  15,  31,  20,
  61,  34,  57,  24,  31,  15,  33,  20,
  64,  36,  57,  26,  24,  14,  36,  19,
  66,  38,  57,  28,  20,  14,  39,  18,
  67,  39,  56,  30,  16,  14,  42,  17,
  64,  41,  56,  32,   5,  14,  45,  17,
  55,  42,  59,  33,  -1,  14,  41,  17,
  44,  44,  64,  35,  -3,  15,  33,  16,
  30,  45,  67,  38,   1,  15,  27,  14,
  21,  46,  68,  39,   5,  15,  22,  14,
  19,  47,  70,  41,   9,  16,  18,  14,
  16,  48,  69,  42,  12,  17,  11,  14,
  12,  49,  63,  44,  17,  18,   1,  15]



if __name__ == '__main__':
    try:
        flushSerialOutput(300)

        '''
        testSchedule is used to test various serial port commands
        '''
        testSchedule = [
            
            # - 'kbalance' indicates the command to control Bittle to stand normally
            # - 1 indicates the postponed time after finishing the command, in seconds
            # - the skill data is stored locally on the robot
            ['kbalance', 1],

            # - m indicates the command to control the rotation of the joint servo
            # - 0 indicates the index number of joint servo
            # - -50 indicates the rotation angle (this angle refers to the origin, rather than additive) the unit is degree
            # - 0.5 indicates the postponed time after finishing the command, in seconds. It can be a float number.
            ['m', [0, -50], 0.5],

            # Using this format, multiple joint servo rotation commands can be sent at one time,
            # and these joint servo rotation commands are executed SEQUENTIALLY,
            # not at the same time.
            # The meaning of this example is: the joint servo with index number 0 is first
            # rotated to the 45 degree position, and then rotated to the -45 degree position, and so on.
            # After these motion commands are completed, the next command will
            # be executed after a 2-second delay.
            ['m', [8, -5, 8, 10, 8, -5, 8, 10, 8, -5, 8, 10], 1],

            # Using this format, multiple joint servo rotation commands can be issued at one time,
            # and these joint servo rotation commands are executed AT THE SAME TIME.
            # The meaning of this example is: the joint servos with index numbers 8, 9 are
            # rotated to the -15, -20 degree position at the same time.
            # After these motion commands are completed, the next command will
            # be executed after a 2-second delay.
            ['i', [8, -15, 9, -20], 1],

            # - d indicates the command to put the robot down and shut down the servos
            # - 2 indicates the postponed time after finishing the command, in seconds
            ['d', 1],

            # - c indicates the command to enter calibration mode
            # - 2 indicates the postponed time after finishing the command, in seconds
            # After these motion commands are completed, the next command will
            # be executed after a 2-second delay.
            ['c', 1],

            # - c indicates the command to enter calibration mode
            # - 0 indicates the index number of joint servo
            # - -9 indicates the rotation angle, the unit is degree
            # - 2 indicates the postponed time after finishing the command, in seconds
            ['c', [0, -9], 1],
            ['a', 0],

            # - I indicates the command to rotate multiple joint servos at the same time
            # - 8, 9, 10, 11, 0 indicate the index numbers of joint servos
            # - 50, 50, 50, 50, 0 indicate the rotation angle (this angle refers to the origin,
            #                     rather than additive), the unit is degree
            # - 3 indicates the postponed time after finishing the command, in seconds
            ['I', [8, -20, 0, 0, 9, 50, 10, -20, 11, 50, 14, 80], 1],
            ['I', [8, 20, 0, 40, 9, -20, 10, 50, 11, -20], 1],

            # - L indicates the command to control all joint servos to rotate at the same time
            #     (currently the command supports 16 degrees of freedom, that is, 16 servos)
            # - 20,0,0,0,0,0,0,0,45,45,45,45,36,36,36,36 indicate the rotation
            #               angle of each joint servo corresponding to 0-15 (this angle refers to
            #               the origin, rather than additive), the unit is degree
            # - 5 indicates the postponed time after finishing the command, in seconds
            ['L', [20, 0, 0, 0, 0, 0, 0, 0, 25, 0, 45, 45, 80, 80, 36, 36], 1],

            # - b indicates the command to control the buzzer to beep, encrypted in ASCII
            # - 15 indicates the music tone
            # - 90 indicates the lengths of duration, the value range is 0~255
            # - 2 indicates the postponed time after completing the pronunciation, in seconds
            ['b',[15, 90, 12, 90, 15, 90, 12, 90, 15, 90, 12, 90, 8, 180, 10, 90, 13, 90, 12, 90, 10, 90, 15, 180,],1],

            # Using this format, multiple tone commands can be encrypted in binary
            # the music melody is played.
            # - '20', '22', '24', '15', '20' indicate the music tones
            # - '4', '4', '4', '4', '4' indicates 3 second/lengths of duration
            # - 2 indicates the postponed time after the music melody is played, in seconds
            ['B',[20, 4, 22, 4, 24, 4, 15, 4, 20, 4, 22, 8, 24, 1, 22, 4, 20, 4, 22, 4, 27, 4, 27, 4, 27, 4, 27, 2, 20, 4, 19, 4, 20, 4, 20, 4, 20, 4, 20, 4, 20, 2, 19, 4, 20, 4, 19, 4, 20, 4, 19, 4, 17, 4, 15, 2, 15, 4, 15, 4, 17, 4, 17, 4, 17, 4, 17, 4, 17, 2, 15, 4, 12, 4, 15, 4, 12, 4, 15, 4, 22, 4, 20, 2, -1, 4, 15, 4, 24, 4, 24, 4, 24, 4, 25, 4, 27, 4, 20, 4, 20, 4, 24, 4, 22, 1, 22, 1, -1, 2, 15, 4, 15, 4, 15, 2, 15, 4, 15, 4, 17, 4, 15, 4, 12, 4, 15, 4, -1, 4, 7, 4, 8, 4, 8, 4, 12, 2, 12, 4, 13, 4, 12, 4, 8, 4, 8, 4, 10, 4, 12, 1, -1, 4, 12, 4, 10, 4, 8, 4, 8, 4, 8, 2, 7, 4, -1, 4, 8, 4, 8, 4, 8, 4, 8, 4, 3, 4, 3, 4, 12, 2, 8, 4, 8, 4, 3, 4, 13, 2, 13, 4, 13, 4, 13, 4, 5, 4, 8, 4, 8, 4, 10, 1, 10, 1], 1],

            # - 'K' indicates the skill data to send to Bittle in realtime
            # they are sent to the robot on the go and executed locally on the robot
            # no overhead of communication and waiting for both sides.
            ['kvt', 2],
            ['K',sit, 1],
            ['kck', 1],
            # 'T' calls the last temp skill data of 'K' received from the serial port
            ['T', 2],
            ['K', vt, 2],
            ['ksit', 1],
            ['T', 2],
            ['K', ck, 1],  # compare it with the previous 'kck' command
            ['d', 0]
        ]
        
        time.sleep(2);
        for task in testSchedule:  # execute the tasks in the testSchedule
            print(task)
            wrapper(task)
        
        schedulerToSkill(testSchedule[:11]) # compile the motion related instructions to a skill and send it to the robot. the last skill sent over in this way can be recalled by the 'T' token even after the robot reboots. 

        closeSerialBehavior()
        logger.info("finish!")

    except Exception as e:
        wrapper(['d',1])
        logger.info("Exception")
        closeSerialBehavior()
        raise e
