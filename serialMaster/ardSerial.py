#!/usr/bin/python3
# -*- coding: UTF-8 -*-

import struct
import sys
import time
import logging
from SerialCommunication import *    # module SerialCommunication.py
import platform
import copy

FORMAT = '%(asctime)-15s %(name)s - %(levelname)s - %(message)s'
'''
Level: The level determines the minimum priority level of messages to log. 
Messages will be logged in order of increasing severity: 
DEBUG is the least threatening, 
INFO is also not very threatening, 
WARNING needs attention, 
ERROR needs immediate attention, 
and CRITICAL means “drop everything and find out what’s wrong.” 
The default starting point is INFO, 
which means that the logging module will automatically filter out any DEBUG messages.
'''
# logging.basicConfig(level=logging.DEBUG, format=FORMAT)
logging.basicConfig(level=logging.INFO, format=FORMAT)
logger = logging.getLogger(__name__)


def encode(in_str, encoding='utf-8'):
    if isinstance(in_str, bytes):
        return in_str
    else:
        return in_str.encode(encoding)


def serialWriteNumToByte(token, var=None):  # Only to be used for c m u b I K L o within Python
    # print("Num Token "); print(token);print(" var ");print(var);print("\n\n");
    logger.debug(f'serialWriteNumToByte, token={token}, var={var}')

    in_str = ""
    if var is None:
        var = []
    if token =='K':
        var = list(map(int, var))
        period = var[0]
#        print(encode(in_str))
        if period >0:
            skillHeader = 4
        else:
            skillHeader = 7
            
        in_str = token.encode() + struct.pack('b' * skillHeader, *var[0:skillHeader])#+'~'.encode()
        ser.Send_data(in_str)
        time.sleep(0.005)
        if period >1:
            frameSize = 8   #gait
        elif period ==1:
            frameSize = 16  #posture
        else:
            frameSize = 20     #behavior
        for f in range(abs(period)):
            in_str = struct.pack('b' * (frameSize), *var[skillHeader + f * frameSize:skillHeader + (f + 1) * frameSize])# + '~'.encode()
            if f == abs(period)-1:
                in_str = in_str + '~'.encode()
            ser.Send_data(in_str)
            time.sleep(0.005)
    else:
        if token == 'L' or token == 'I' or token == 'B':
            var = list(map(int, var))
            in_str = token.encode() + struct.pack('b' * len(var), *var) + '~'.encode()
            
        elif token == 'c' or token == 'm' or token == 'i' or token == 'b' or token == 'u' or token == 't':
            in_str = token + " "
            for element in var:
                in_str = in_str + str(element) + " "
        logger.debug(f"!!!! {in_str}")
#        print(encode(in_str))
        ser.Send_data(encode(in_str))


def serialWriteByte(var=None):
    logger.debug(f'serial_write_byte, var={var}')
    if var is None:
        var = []
    token = var[0][0]
    # print var
    if (token == 'c' or token == 'm' or token == 'i' or token == 'b' or token == 'u' or token == 't') and len(var) >= 2:
        in_str = ""
        for element in var:
            in_str = in_str + element + " "
    elif token == 'L' or token == 'I':
        if len(var[0]) > 1:
            var.insert(1, var[0][1:])
        var[1:] = list(map(int, var[1:]))
        in_str = token.encode() + struct.pack('b' * (len(var) - 1), *var[1:]) + '~'.encode()
    elif token == 'w' or token == 'k':
        in_str = var[0] + '\n'
    else:
        in_str = token
    logger.debug(f"!!!!!!! {in_str}")
    ser.Send_data(encode(in_str))

def printSerialMessage(token):
    if token == 'k':
        threshold = 4
    else:
        threshold = 2
    startTime = time.time()
    
    while True:
        time.sleep(0.005)
        now = time.time()
        if (now - startTime) > threshold :
            print('Elapsed time: ',end='')
            print(threshold, end=' seconds\n', flush = True)
            threshold += 2
#            return 'err'
        response = ser.main_engine.readline().decode('ISO-8859-1')
        if response != '':
            startTime = time.time()
            if response.lower() == token.lower() +'\r\n':
                return response[:-2]
            else:
                print(response, flush = True)


def wrapper(task):  # task Structure is [token, var=[], time]
    logger.debug(f"{task}")
#    print(task)
    if len(task) == 2:
#        print('a')
        print(task[0])
        serialWriteByte([task[0]])
    elif isinstance(task[1][0], int):
#        print('b')
        serialWriteNumToByte(task[0], task[1])
        
    else:
#        print('c') #which case
        serialWriteByte(task[1])
    token = task[0][0]
    lastMessage = printSerialMessage(token)
    time.sleep(task[-1])
    return lastMessage
            
def keepReadingSerial():
    while True:
        time.sleep(0.005)
#        response = ser.main_engine.readline().decode('ISO-8859-1')
#        if response != '':
#            print(response)
#        time.sleep(0.005)  #won't print until gets new input. why??
        x = input()
        if x != "":
            if x == "q" or x == "quit":
                break
            else:
                token = x[0]
                task = x[1:].split() #white space
                if len(task) <= 1:
                    wrapper([x, 1])
                else:
                    wrapper([token, list(map(int, task)), 1])
#            wrapper(['j', 1])


def closeSerialBehavior():
    try:
        wrapper(['d', 1])
        ser.Close_Engine()
        logger.info("close the serial port.")
    except Exception as e:
        ser.Close_Engine()
        logger.info("close the serial port.")
        raise e


def flushSerialOutput(counterLimit=300):
    counter = 0
    while True:
        time.sleep(0.01)
        counter = counter + 1
        if counter > counterLimit:
            break
        if ser.main_engine.in_waiting > 0:
            x = ser.main_engine.readline()
            if x != "":
                logger.debug(f"{x}")

    if platform.uname()[1] != 'raspberrypi':
        print("If there is no response after you input the serial command in the terminal")
        print("you should close the terminal first")
        print("then change the value of 'bluetoothPortIndex' in the ardSerial.py (line:159)")
        print("to connect to another serial port")
        print("then reopen the terminal and rerun the script")
        print("---Start---")

balance = [
    1, 0, 0, 1,
    0,   0,   0,   0,   0,   0,   0,   0,  30,  30,  30,  30,  30,  30,  30,  30]
buttUp = [
    1, 0, 15, 1,
    20,  40,   0,   0,   5,   5,   3,   3,  90,  90,  45,  45, -60, -60,   5,   5]
calib = [
    1, 0, 0, 1,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0]
dropped = [
    1, 0, -75, 1,
    0,  30,   0,   0,  -5,  -5,  15,  15, -75, -75,  45,  45,  60,  60, -30, -30]
lifted = [
    1, 0, 75, 1,
    0, -20,   0,   0,   0,   0,   0,   0,  60,  60,  75,  75,  45,  45,  75,  75]
rest = [
    1, 0, 0, 1,
    -30, -80, -45,   0,  -3,  -3,   3,   3,  70,  70,  70,  70, -55, -55, -55, -55]
sit = [
    1, 0, -30, 1,
    0,   0, -45,   0,  -5,  -5,  20,  20,  45,  45, 105, 105,  45,  45, -45, -45]
stretch = [
    1, 0, 20, 1,
    0,  30,   0,   0,  -5,  -5,   0,   0, -75, -75,  30,  30,  60,  60,   0,   0]
zero = [
    1, 0, 0, 1,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0]
    
    
balanceNybble = [
    1, 0, 0, 1,
    0,   0,   0,   0,   0,   0,   0,   0,  30,  30, -30, -30,  30,  30, -30, -30,]
buttUpNybble = [
    1, 0, 15, 1,
   20,  40,   0,   0,   5,   5,   3,   3,  90,  90, -45, -45, -60, -60,  -5,  -5,]
calibNybble = [
    1, 0, 0, 1,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,]
droppedNybble = [
    1, 0, 75, 1,
    0,  30,   0,   0,  -5,  -5,  15,  15, -75, -75, -60, -60,  60,  60,  30,  30,]
liftedNybble = [
    1, 0, -75, 1,
    0, -70,   0,   0,   0,   0,   0,   0,  55,  55,  20,  20,  45,  45,   0,   0,]
luNybble = [
    1, -30, 15, 1,
  -45,  60, -60,   0,   5,   5,   3,   3, -60,  70, -45, -35,  15, -60,  10, -65,]
restNybble = [
    1, 0, 0, 1,
  -30, -80, -45,   0,  -3,  -3,   3,   3,  60,  60, -60, -60, -45, -45,  45,  45,]
sitNybble = [
    1, 0, -20, 1,
    10, -20, -60,   0,  -5,  -5,  20,  20,  30,  30, -90, -90,  60,  60,  45,  45,]
sleepNybble = [
    1, 0, 0, 1,
  -10,-100,   0,   0,  -5,  -5,   3,   3,  80,  80, -80, -80, -55, -55,  55,  55,]
strNybble = [
    1, 0, 15, 1,
    10,  70,   -30,   0,  -5,  -5,   0,   0, -75, -75, -45, -45,  60,  60, -45, -45,]
zeroNybble = [
    1, 0, 0, 1,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,]


postureTableBittle = {
    "balance": balance,
    "buttUp": buttUp,
    "calib": calib,
    "dropped": dropped,
    "lifted": lifted,
    "rest": rest,
    "sit": sit,
    "str": stretch,
    "zero": zero
}

postureTableNybble = {
    "balance":balanceNybble,
    "buttUp":buttUpNybble,
    "calib":calibNybble,
    "dropped":droppedNybble,
    "lifted":liftedNybble,
    "lu":luNybble,
    "rest":restNybble,
    "sit":sitNybble,
    "sleep":sleepNybble,
    "str":strNybble,
    "zero":zeroNybble
}
postureTableDoF16 = {
    "balance": balance,
    "buttUp": buttUp,
    "calib": calib,
    "dropped": dropped,
    "lifted": lifted,
    "rest": rest,
    "sit": sit,
    "str": stretch,
    "zero": zero
}

postureDict = {
    'Nybble': postureTableNybble,
    'Bittle': postureTableBittle,
    'DoF16':postureTableDoF16
}
model = 'Bittle'
postureTable = postureDict[model]


def schedulerToSkill(testSchedule):
    compactSkillData = []
    newSkill = []
    outputStr = ""

    for task in testSchedule:  # execute the tasks in the testSchedule
        #            print(task)
        token = task[0][0]
        if token == 'k':
            currentRow = postureTable[task[0][1:]][-16:]
            skillRow = copy.deepcopy(currentRow)
            compactSkillData.append(skillRow + [8, int(task[1]*1000/500), 0, 0])
            newSkill = newSkill + skillRow + [8, int(task[1]*1000/500), 0, 0]

        elif token == 'i' or token == 'I':
            currentRow = copy.deepcopy(skillRow)
            for e in range(0, len(task[1]), 2):
                #                    print(task[1][e],task[1][e+1])
                currentRow[task[1][e]] = task[1][e+1]
            skillRow = copy.deepcopy(currentRow)
            compactSkillData.append(skillRow + [8, int(task[2]*1000/500), 0, 0])
            newSkill = newSkill + skillRow + [8, int(task[2]*1000/500), 0, 0]
        elif token =='L':
            skillRow = copy.deepcopy(task[1][:16])
            compactSkillData.append(skillRow + [8, int(task[2]*1000/500), 0, 0])
            newSkill = newSkill + skillRow + [8, int(task[2]*1000/500), 0, 0]

        elif token == 'm':
            currentRow = copy.deepcopy(skillRow)
            for e in range(0, len(task[1]), 2):
                currentRow[task[1][e]] = task[1][e+1]
                skillRow = copy.deepcopy(currentRow)
                compactSkillData.append(skillRow + [8, int(task[2]*1000/500), 0, 0])
                newSkill = newSkill + skillRow + [8, int(task[2]*1000/500), 0, 0]

    print('{')
    print('{:>4},{:>4},{:>4},{:>4},'.format(*[-len(compactSkillData), 0, 0, 1]))
    print('{:>4},{:>4},{:>4},'.format(*[0, 0, 0]))
    for row in compactSkillData:
        print('{:>4},{:>4},{:>4},{:>4},{:>4},{:>4},{:>4},{:>4},{:>4},{:>4},{:>4},{:>4},{:>4},{:>4},{:>4},{:>4}, {:>4},{:>4},{:>4},{:>4},'.format(*row))
    print('};')

    newSkill = [-len(compactSkillData), 0, 0, 1, 0, 0, 0] + newSkill
    print(newSkill)
    wrapper(['K', newSkill, 1])

# port = '/dev/cu.BittleSPP-3534C8-Port'    # Bluetooth serial port needed when using Mac
# port = '/dev/cu.wchusbserial1430'    # needed when using Mac
# port = '/dev/ttyS0'    # needed when plug in RaspberryPi
# port = 'COM5'    # needed when using Windows

Communication.Print_Used_Com()
port = port_list_number
total = len(port)
index = 0
for index in range(total):
    logger.info(f"port[{index}] is {port[index]} ")

if len(port) > 1:
    """
    If there is no response after you input the serial command in the terminal,
    you should close the terminal first,
    then change the value of "bluetoothPortIndex" in the ardSerial.py (line:159)
    to connect to another serial port,
    then reopen the terminal and rerun the script.
    """
    if platform.uname()[1] == 'raspberrypi':
        serialPort = '/dev/ttyS0'  # needed when plug in RaspberryPi
        ser = Communication(serialPort, 115200, 0.5)
        logger.info(f"Connect to usb serial port: {serialPort}.")
#        serialWriteByte(["d"])
#        time.sleep(0.2)
        wrapper(['d',0])
        response = ser.main_engine.read_all()
        logger.info(f"Response is: {response}")
        if response == b'':
            ser.Close_Engine()
            logger.info(f"close the serial port: {serialPort}.")
            serialPortIndex = 0  # 0 means connect to port[0]; -1 means connect to the last port in the list
            ser = Communication(port[serialPortIndex], 115200, 0.5)
            logger.info(f"Connect to serial port: {port[serialPortIndex]}.")
            print("If there is no response after you input the serial command in the terminal")
            print("you should close the terminal first")
            print("then change the value of 'serialPortIndex' in the ardSerial.py (line:150)")
            print("to connect to another serial port")
            print("then reopen the terminal and rerun the script")
    else:
        bluetoothPortIndex = -1    #0 means connetct to port[0]; -1 means connetct to the last port in the list
        ser = Communication(port[bluetoothPortIndex], 115200, 0.5)
        logger.info(f"Connect to serial port: {port[bluetoothPortIndex]}.")
else:
    if platform.uname()[1] == 'raspberrypi':
        serialPort = '/dev/ttyS0'  # needed when plug in RaspberryPi
        ser = Communication(serialPort, 115200, 0.5)
        logger.info(f"Connect to usb serial port: {serialPort}.")
    else:
        usbPortIndex = 0
        ser = Communication(port[usbPortIndex], 115200, 0.5)
        logger.info(f"Connect to usb serial port: {port[usbPortIndex]}.")


if __name__ == '__main__':
    try:
#        flushSerialOutput(500)
        if len(sys.argv) >= 2:
            if len(sys.argv) == 2:
                cmd = sys.argv[1]
                token = cmd[0][0]
                wrapper([sys.argv[1], 1])
#                time.sleep(0.2)
            else:
                token = sys.argv[1][0]
                wrapper([sys.argv[1][0], sys.argv[1:], 1])

        print("You can type 'quit' or 'q' to exit.")
        
        keepReadingSerial()
        
        
        closeSerialBehavior()
        logger.info("finish!")

    except Exception as e:
        logger.info("Exception")
        closeSerialBehavior()
        raise e
