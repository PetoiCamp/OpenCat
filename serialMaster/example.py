#!/usr/bin/python3
# -*- coding: UTF-8 -*-

from ardSerial import *


if __name__ == '__main__':
    try:
        flushSeialOutput(300)

        '''
        testSchedule is used to test various serial port commands
        '''
        testSchedule = [
# - 'kbalance' indicates the command to control Bittle to stand normally
# - 2 indicates the postponed time after finishing the command, in seconds
                        ['kbalance', 2],
                        
# - m indicates the command to control the rotation of the joint servo
# - 0 indicates the index number of joint servo
# - -20 indicates the rotation angle (this angle refers to the origin, rather than additive) the unit is degree
# - 1.5 indicates the postponed time after finishing the command, in seconds. It can be a float number.
                        ['m', [1, 0], 2],
                        ['m', [0, -20], 1.5],

# Using this format, multiple joint servo rotation commands can be sent at one time,
# and these joint servo rotation commands are executed SEQUENTIALLY,
# not at the same time.
# The meaning of this example is: the joint servo with index number 0 is first
# rotated to the 45 degree position, and then rotated to the -45 degree position, and so on.
# After these motion commands are completed, the next command will
# be executed after a 2-second delay.
                        ['m', [0, 45, 0, -45, 0, 45, 0, -45], 2],

# Using this format, multiple joint servo rotation commands can be issued at one time,
# and these joint servo rotation commands are executed AT THE SAME TIME.
# The meaning of this example is: the joint servos with index numbers 8, 9 are
# rotated to the -15, -20 degree position at the same time.
# After these motion commands are completed, the next command will
# be executed after a 2-second delay.
                        ['M', [8, -15, 9, -20], 2],
                        
# - d indicates the command to put the robot down and shut down the servos
# - 2 indicates the postponed time after finishing the command, in seconds
                        ['d', 2],

#- c indicates the command to enter calibration mode
#- 2 indicates the postponed time after finishing the command, in seconds
# After these motion commands are completed, the next command will
# be executed after a 2-second delay.
                        ['c', 2],

# - c indicates the command to enter calibration mode
# - 0 indicates the index number of joint servo
# - -9 indicates the rotation angle, the unit is degree
# - 2 indicates the postponed time after finishing the command, in seconds
                        ['c',[0,-9],2],

# - i indicates the command to rotate multiple joint servos at the same time
# - 8, 9, 10, 11, 0 indicate the index numbers of joint servos
# - 50, 50, 50, 50, 0 indicate the rotation angle (this angle refers to the origin,
#                     rather than additive), the unit is degree
# - 3 indicates the postponed time after finishing the command, in seconds
                        ['i',[8,50,9,50,10,50,11,50,0,0],3],

# - l indicates the command to control all joint servos to rotate at the same time
#     (currently the command supports 16 degrees of freedom, that is, 16 servos)
# - 20,0,0,0,0,0,0,0,45,45,45,45,36,36,36,36 indicate the rotation
#               angle of each joint servo corresponding to 0-15 (this angle refers to
#               the origin, rather than additive), the unit is degree
# - 5 indicates the postponed time after finishing the command, in seconds
                        ['l',[20,0,0,0,0,0,0,0,45,45,45,45,36,36,36,36],2],

# - b indicates the command to control the buzzer to beep
# - 10 indicates the music tone
# - 255 indicates the lengths of duration, the value range is 0~255
# - 2 indicates the postponed time after completing the pronunciation, in seconds
                        ['b',[10, 255], 2],

# Using this format, multiple tone commands can be issued at one time,
# and a simple melody can be played.
# The meaning of this example is: play a simple melody, and delay 5 seconds after
# the music melody is played.
# - '14', '14', '21', '21', ‘23', '23', '21' indicate the music tones
# - '90', '90', '90', '90', '90', '90', '180' indicates the lengths of duration
# - 5 indicates the postponed time after the music melody is played, in seconds
                        ['b', [14, 90, 14, 90, 21, 90, 21, 90, 23, 90, 23, 90, 21, 180], 5],
                        ['d', 2],
                       ]

        for task in testSchedule: #execute the tasks in the testSchedule
            token = task[0][0]
            wrapper(task)
            response = ser.main_engine.read_all()
            logger.info(f"Response is: {response.decode('utf-8')}")

        closeSerialBehavior()
        logger.info("finish!")

    except Exception as e:
        logger.info("Exception")
        closeSerialBehavior()
        raise e
