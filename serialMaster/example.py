#!/usr/bin/python3
# -*- coding: UTF-8 -*-

from ardSerial import *


if __name__ == '__main__':
    try:
        flushSeialOutput(300)
        print("If there is no response in the terminal")
        print("you should close the terminal first")
        print("then change the value of 'bluetoothPortIndex' in the ardSerial.py (line:129)")
        print("to connect to another blue tooth serial port")
        print("then reopen the terminal and rerun the script")
        print("---Start---")

        '''
        testSchedule is used to test various serial port commands
        '''
        testSchedule = [
# - 'kbalance' indicates the command to control Bittle to stand normally
# - 2 indicates the postponed time after the command execution is completed, in seconds
                        ['kbalance', 2],

# - m indicates the command to control the rotation of the joint servo
# - 0 indicates the index number of joint servo
# - -10 indicates the rotation angle value (this value represents relative to the reference 0
#       which is determined after calibration), the unit is degree
# - 2 indicates the postponed time after the command execution is completed, in seconds
                        ['m', [0, -10], 2],

# - d indicates the command to put the robot down and shut down the servos
# - 2 indicates the postponed time after the command execution is completed, in seconds
                        ['d', 2],

# Using this format, multiple joint servo rotation commands can be issued at one time,
# but note that these joint servo rotation commands are executed sequentially,
# not at the same time.
# The meaning of this example is: the joint servo with index number 0 is first
# rotated to the 45 degree position, and then rotated to the -45 degree position.
# After the execution of these motion commands is completed, the next command will
# be executed after a delay of 2 seconds.
                        ['m', ['m', '0', '45', '0', '-45'], 2],

                        ['d', 2],

# Using this format, multiple joint servo rotation commands can be issued at one time,
# and these joint servo rotation commands are executed at the same time,
# The meaning of this example is: the joint servos with index numbers 8 to 15 are
# rotated to the 30 degree position at the same time.
# After the execution of these motion commands is completed, the next command will
# be executed after a delay of 2 seconds.
                        ['M', ['M', '8', '30', '9', '30', '10', '30', '11', '30', '12', '30', '13', '30', '14', '30', '15', '30'], 2],

                        ['d', 2],

#- c indicates the command to enter calibration mode
#- 3 indicates the postponed time after the command execution is completed, in seconds
#    if there are other commands later, the subsequent commands will
#    be executed after the delay time expires.
                        ['c', 3],

                        ['d', 2],

# - c indicates the command to enter calibration mode
# - 0 indicates the index number of joint servo
# - -5 indicates the rotation angle value, the unit is degree
# - 2 indicates the postponed time after the command execution is completed, in seconds
                        ['c',[0,-5],2],

                        ['d', 2],

# - i indicates the command to control multiple joint servos to rotate at the same time
# - 8, 9, 10, 11, 0 indicate the index numbers of joint servos
# - 50, 50, 50, 50, 0 indicate the rotation angle values (the value represents relative
#   to the reference 0 which is determined after calibration), the unit is degree
# - 3 indicates the postponed time after the command execution is completed, in seconds
                        ['i',[8,50,9,50,10,50,11,50,0,0],3],

                        ['d', 2],

# - l indicates the command to control all joint servos to rotate at the same time
#     (currently the command supports 16 degrees of freedom, that is, 16 servos)
# - 20,0,0,0,0,0,0,0,45,45,45,45,36,36,36,36 indicate corresponding to the rotation
#   angle value of each joint servo with serial numbers 0-15 (the value represents
#   relative to the reference 0 which is determined after calibration), the unit is degree
# - 5 indicates the postponed time after the command execution is completed, in seconds
                        ['l',[20,0,0,0,0,0,0,0,45,45,45,45,36,36,36,36],5],

                        ['d', 2],

# - b indicates the command to control the buzzer to sound
# - 10 indicates the music tone
# - 255 indicates the length of pronunciation, the value range is 0~255
# - 2 indicates the postponed time after completing the pronunciation, in seconds
                        ['b',[10, 255], 2],

# Using this format, multiple tone pronunciation commands can be issued at one time,
# and a simple melody can be played.
# The meaning of this example is: play a simple melody, and delay 5 seconds after
# the music melody is played.
# - '14', '14', '21', '21', ‘23', '23', '21' indicate the music tones
# - '90', '90', '90', '90', '90', '90', '180' indicates the lengths of pronunciation
# - 5 indicates the postponed time after the music melody is played, in seconds
                        ['b', ['b', '14', '90', '14', '90', '21', '90', '21', '90', '23', '90', '23', '90', '21', '180'], 5]
                       ]

        for task in testSchedule:
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
