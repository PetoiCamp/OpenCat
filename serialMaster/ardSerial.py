#!/usr/bin/python3
# -*- coding: UTF-8 -*-

import struct
import sys
import time
import logging
from SerialCommunication import *  # module SerialCommunication.py
import platform
import copy
import threading
import os

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


def printH(head, value):
    print(head, end=' ')
    print(value)


def encode(in_str, encoding='utf-8'):
    if isinstance(in_str, bytes):
        return in_str
    else:
        return in_str.encode(encoding)

def serialWriteNumToByte(port, token, var=None):  # Only to be used for c m u b I K L o within Python
    # print("Num Token "); print(token);print(" var ");print(var);print("\n\n");
    logger.debug(f'serialWriteNumToByte, token={token}, var={var}')
    in_str = ""
    if var is None:
        var = []
    if token == 'K':
        period = var[0]
        #        print(encode(in_str))
        if period > 0:
            skillHeader = 4
        else:
            skillHeader = 7
            
        if period > 1:
            frameSize = 8  # gait
        elif period == 1:
            frameSize = 16  # posture
        else:
            frameSize = 20  # behavior
    # divide large angles by 2
        angleRatio = 1
        for row in range(abs(period)):
            for angle in var[skillHeader + row * frameSize:skillHeader + row * frameSize + min(16,frameSize)]:
                if angle > 125 or angle<-125:
                    angleRatio = 2
                    break
            if angleRatio ==2:
                break
        
        if angleRatio == 2:
            var[3] = 2
            for row in range(abs(period)):
                for i in range(skillHeader + row * frameSize,skillHeader + row * frameSize + min(16,frameSize)):
                    var[i] //=2
            printH('rescaled:\n',var)
            
        var = list(map(int, var))

        in_str = token.encode() + struct.pack('b' * skillHeader, *var[0:skillHeader])  # +'~'.encode()
        port.Send_data(in_str)
        time.sleep(0.001)

        for f in range(abs(period)):
            in_str = struct.pack('b' * (frameSize),
                                 *var[skillHeader + f * frameSize:skillHeader + (f + 1) * frameSize])  # + '~'.encode()
            if f == abs(period) - 1:
                in_str = in_str + '~'.encode()
            port.Send_data(in_str)
            time.sleep(0.001)
    else:
        if token == 'L' or token == 'I' or token == 'B' or token == 'C':
#            if token == 'C':
#                packType = 'B'
#            else:
#                packType = 'b'
            var = list(map(int, var))
            
            in_str = token.encode() + struct.pack('b' * len(var), *var) + '~'.encode()

        elif token == 'c' or token == 'm' or token == 'i' or token == 'b' or token == 'u' or token == 't':
            in_str = token + " "
            for element in var:
                in_str = in_str + str(element) + " "
        logger.debug(f"!!!! {in_str}")
        #        print(encode(in_str))
        port.Send_data(encode(in_str))


def serialWriteByte(port, var=None):
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
    port.Send_data(encode(in_str))


def printSerialMessage(port, token, timeout=0):
    if token == 'k' or token == 'K':
        threshold = 4
    else:
        threshold = 1
    #    if token == 'K':
    #        timeout = 1
    startTime = time.time()
    allPrints = ''
    while True:
        time.sleep(0.001)
        #            return 'err'
        if port:
            response = port.main_engine.readline().decode('ISO-8859-1')
            if response != '':
                #                startTime = time.time()
                response = response[:-2]  # delete '\r\n'
                if response.lower() == token.lower():
                    return [response, allPrints]
                else:
#                    print(response, flush=True)
                    allPrints += response + '\n'
        now = time.time()
        if (now - startTime) > threshold:
            print('Elapsed time: ', end='')
            print(threshold, end=' seconds\n', flush=True)
            threshold += 2
        if 0 < timeout < now - startTime:
            return -1


def sendTask(goodPorts, port, task, timeout=0):  # task Structure is [token, var=[], time]
    logger.debug(f"{task}")
    global returnValue
    #    global sync
    #    print(task)
    if port:
        try:
            previousBuffer = port.main_engine.read_all().decode('ISO-8859-1')
            if previousBuffer:
                printH('Previous buffer:', previousBuffer)
            if len(task) == 2:
                #        print('a')
                #        print(task[0])
                serialWriteByte(port, [task[0]])
            elif isinstance(task[1][0], int):
                #        print('b')
                serialWriteNumToByte(port, task[0], task[1])
            else:
                #        print('c') #which case
                serialWriteByte(port, task[1])
            token = task[0][0]
            lastMessage = printSerialMessage(port, token, timeout)
            time.sleep(task[-1])
        #    with lock:
        #        sync += 1
        #        printH('sync',sync)
        #    if initialized:
        #        printH('thread',portDictionary[port])
        except Exception as e:
            #        printH('Fail to send to port',goodPorts[port])
            if port in goodPorts:
                goodPorts.pop(port)
            lastMessage = -1
    else:
        lastMessage = -1
    returnValue = lastMessage
    return lastMessage


def sendTaskParallel(ports, task, timeout=0):
    global returnValue
    #    global sync
    #    sync = 0
    threads = list()
    for p in ports:
        t = threading.Thread(target=sendTask, args=(goodPorts, p, task, timeout))
        threads.append(t)
        t.start()
    for t in threads:
        if t.is_alive():
            t.join()
    return returnValue


def splitTaskForLargeAngles(task):
    token = task[0]
    queue = list()
    if token == 'L' or token == 'I':
        var = task[1]
        indexedList = list()
        if token == 'L':
            for i in range(4):
                for j in range(4):
                    angle = var[4 * j + i]
                    if angle < -125 or angle > 125:
                        indexedList += [4 * j + i, angle]
                        var[4 * j + i] = max(min(angle, 125), -125)
            if len(var):
                queue.append(['L', var, task[-1]])
            if len(indexedList):
                queue[0][-1] = 0
                queue.append(['i', indexedList, task[-1]])
        #                print(queue)
        elif token == 'I':
            if min(var) < -125 or max(var) > 125:
                task[0] = 'i'
            queue.append(task)
    else:
        queue.append(task)
    return queue


def send(port, task, timeout=0):
    if isinstance(port, dict):
        p = list(port.keys())
    elif isinstance(port, list):
        p = port
    queue = splitTaskForLargeAngles(task)
    for task in queue:
        if len(port) > 1:
            returnResult = sendTaskParallel(p, task, timeout)
        elif len(port) == 1:
            returnResult = sendTask(goodPorts, p[0], task, timeout)
        else:
            #            print('no ports')
            return -1
    return returnResult


def keepReadingInput(ports):
    while True and len(ports):
        time.sleep(0.001)
        x = input()  # blocked waiting for the user's input
        if x != "":
            if x == "q" or x == "quit":
                break
            else:
                token = x[0]
                task = x[1:].split()  # white space
                if len(task) <= 1:
                    send(ports, [x, 1])
                else:
                    send(ports, [token, list(map(int, task)), 1])


def closeSerialBehavior(port):
    try:
        port.Close_Engine()
    except Exception as e:
        port.Close_Engine()
        raise e
    logger.info("close the serial port.")


def closeAllSerial(ports):
    send(ports, ['d', 0], 1)
    for p in ports:
        t = threading.Thread(target=closeSerialBehavior, args=(p,))
        t.start()
        t.join()
    ports.clear()


balance = [
    1, 0, 0, 1,
    0, 0, 0, 0, 0, 0, 0, 0, 30, 30, 30, 30, 30, 30, 30, 30]
buttUp = [
    1, 0, 15, 1,
    20, 40, 0, 0, 5, 5, 3, 3, 90, 90, 45, 45, -60, -60, 5, 5]
calib = [
    1, 0, 0, 1,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
dropped = [
    1, 0, -75, 1,
    0, 30, 0, 0, -5, -5, 15, 15, -75, -75, 45, 45, 60, 60, -30, -30]
lifted = [
    1, 0, 75, 1,
    0, -20, 0, 0, 0, 0, 0, 0, 60, 60, 75, 75, 45, 45, 75, 75]
rest = [
    1, 0, 0, 1,
    -30, -80, -45, 0, -3, -3, 3, 3, 70, 70, 70, 70, -55, -55, -55, -55]
sit = [
    1, 0, -30, 1,
    0, 0, -45, 0, -5, -5, 20, 20, 45, 45, 105, 105, 45, 45, -45, -45]
stretch = [
    1, 0, 20, 1,
    0, 30, 0, 0, -5, -5, 0, 0, -75, -75, 30, 30, 60, 60, 0, 0]
zero = [
    1, 0, 0, 1,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]

balanceNybble = [
    1, 0, 0, 1,
    0, 0, 0, 0, 0, 0, 0, 0, 30, 30, -30, -30, 30, 30, -30, -30, ]
buttUpNybble = [
    1, 0, 15, 1,
    20, 40, 0, 0, 5, 5, 3, 3, 90, 90, -45, -45, -60, -60, -5, -5, ]
calibNybble = [
    1, 0, 0, 1,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ]
droppedNybble = [
    1, 0, 75, 1,
    0, 30, 0, 0, -5, -5, 15, 15, -75, -75, -60, -60, 60, 60, 30, 30, ]
liftedNybble = [
    1, 0, -75, 1,
    0, -70, 0, 0, 0, 0, 0, 0, 55, 55, 20, 20, 45, 45, 0, 0, ]
luNybble = [
    1, -30, 15, 1,
    -45, 60, -60, 0, 5, 5, 3, 3, -60, 70, -45, -35, 15, -60, 10, -65, ]
restNybble = [
    1, 0, 0, 1,
    -30, -80, -45, 0, -3, -3, 3, 3, 60, 60, -60, -60, -45, -45, 45, 45, ]
sitNybble = [
    1, 0, -20, 1,
    10, -20, -60, 0, -5, -5, 20, 20, 30, 30, -90, -90, 60, 60, 45, 45, ]
sleepNybble = [
    1, 0, 0, 1,
    -10, -100, 0, 0, -5, -5, 3, 3, 80, 80, -80, -80, -55, -55, 55, 55, ]
strNybble = [
    1, 0, 15, 1,
    10, 70, -30, 0, -5, -5, 0, 0, -75, -75, -45, -45, 60, 60, -45, -45, ]
zeroNybble = [
    1, 0, 0, 1,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ]

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
    "balance": balanceNybble,
    "buttUp": buttUpNybble,
    "calib": calibNybble,
    "dropped": droppedNybble,
    "lifted": liftedNybble,
    "lu": luNybble,
    "rest": restNybble,
    "sit": sitNybble,
    "sleep": sleepNybble,
    "str": strNybble,
    "zero": zeroNybble
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
    'DoF16': postureTableDoF16
}
model = 'Bittle'
postureTable = postureDict[model]


def schedulerToSkill(ports, testSchedule):
    compactSkillData = []
    newSkill = []
    outputStr = ""

    for task in testSchedule:  # execute the tasks in the testSchedule
        print(task)
        token = task[0][0]
        if token == 'k' and task[0][1:] in postureTable:
            currentRow = postureTable[task[0][1:]][-16:]
            skillRow = copy.deepcopy(currentRow)
            compactSkillData.append(skillRow + [8, int(task[1] * 1000 / 500), 0, 0])
            newSkill = newSkill + skillRow + [8, int(task[1] * 1000 / 500), 0, 0]

        elif token == 'i' or token == 'I':
            currentRow = copy.deepcopy(skillRow)
            for e in range(0, len(task[1]), 2):
                #                    print(task[1][e],task[1][e+1])
                currentRow[task[1][e]] = task[1][e + 1]
            skillRow = copy.deepcopy(currentRow)
            compactSkillData.append(skillRow + [8, int(task[2] * 1000 / 500), 0, 0])
            newSkill = newSkill + skillRow + [8, int(task[2] * 1000 / 500), 0, 0]
        elif token == 'L':
            skillRow = copy.deepcopy(task[1][:16])
            compactSkillData.append(skillRow + [8, int(task[2] * 1000 / 500), 0, 0])
            newSkill = newSkill + skillRow + [8, int(task[2] * 1000 / 500), 0, 0]

        elif token == 'm':
            currentRow = copy.deepcopy(skillRow)
            for e in range(0, len(task[1]), 2):
                currentRow[task[1][e]] = task[1][e + 1]
                skillRow = copy.deepcopy(currentRow)
                compactSkillData.append(skillRow + [8, int(task[2] * 1000 / 500), 0, 0])
                newSkill = newSkill + skillRow + [8, int(task[2] * 1000 / 500), 0, 0]
    if len(compactSkillData) > 0:
        print('{')
        print('{:>4},{:>4},{:>4},{:>4},'.format(*[-len(compactSkillData), 0, 0, 1]))
        print('{:>4},{:>4},{:>4},'.format(*[0, 0, 0]))
    else:
        return
    angleRatio = 1
    for row in compactSkillData:
        if min(row) < -125 or max(row) > 125:
            angleRatio = 2
        print(('{:>4},' * 20).format(*row))
    print('};')
    newSkill = list(map(lambda x: x // angleRatio, newSkill))
    newSkill = [-len(compactSkillData), 0, 0, angleRatio, 0, 0, 0] + newSkill
    print(newSkill)
    #    sendTaskParallel(['K', newSkill, 1])
    send(ports, ['K', newSkill, 1])


def testPort(goodPorts, serialObject, p):
    global goodPortCount
    #    global sync
    try:
        time.sleep(3)
        result = serialObject.main_engine.read_all().decode('ISO-8859-1')
        if result != '':
            print('Waiting for the robot to boot up')
            time.sleep(2)
            waitTime = 3
        else:
            waitTime = 2
        result = sendTask(goodPorts, serialObject, ['b', [20, 50], 0], waitTime)
        print(result)
        if result != -1:
            printH('Adding', p)
            goodPorts.update({serialObject: p})
            goodPortCount += 1
        else:
            serialObject.Close_Engine()
            print('* Port ' + p + ' is not connected to a Petoi device!')
    #    sync +=1
    except Exception:
        print('* Port ' + p + ' cannot be opened!')


def checkPortList(goodPorts, allPorts):
    threads = list()
    for p in reversed(allPorts):  # assuming the last one is the most possible port
        # if p == '/dev/ttyAMA0':
        #     continue
        serialObject = Communication(p, 115200, 1)
        t = threading.Thread(target=testPort,
                             args=(goodPorts, serialObject, p.split('/')[-1]))  # remove '/dev/' in the port name
        threads.append(t)
        t.start()
    for t in threads:
        if t.is_alive():
            t.join(8)


def keepCheckingPort(portList, check=True):
    allPorts = Communication.Print_Used_Com()
    while len(portList) > 0:
        currentPorts = Communication.Print_Used_Com()
        time.sleep(0.01)
        if set(currentPorts) - set(allPorts):
            newPort = list(set(currentPorts) - set(allPorts))
            if check:
                time.sleep(0.5)
                checkPortList(portList, newPort)

        elif set(allPorts) - set(currentPorts):
            closedPort = list(set(allPorts) - set(currentPorts))
            inv_dict = {v: k for k, v in portList.items()}
            for p in closedPort:
                if inv_dict.get(p.split('/')[-1], -1) != -1:
                    printH('Removing', p)
                    portList.pop(inv_dict[p.split('/')[-1]])

        allPorts = copy.deepcopy(currentPorts)


def connectPort(PortList):
    global initialized
    allPorts = Communication.Print_Used_Com()
    if platform.uname()[1] == 'raspberrypi':
        allPorts.append('/dev/ttyS0')

    for index in range(len(allPorts)):
        logger.info(f"port[{index}] is {allPorts[index]} ")

    if len(allPorts) > 0:
        goodPortCount = 0
        checkPortList(PortList, allPorts)
    initialized = True
    if len(PortList) == 0:
        print('No port found!')
    else:
        logger.info(f"Connect to serial port:")
        for p in PortList:
            logger.info(f"{PortList[p]}")


goodPorts = {}
initialized = False
goodPortCount = 0
sync = 0
lock = threading.Lock()
returnValue = ''

if __name__ == '__main__':
    try:
        connectPort(goodPorts)
        t = threading.Thread(target=keepCheckingPort, args=(goodPorts,))
        t.start()
        if len(sys.argv) >= 2:
            if len(sys.argv) == 2:
                cmd = sys.argv[1]
                token = cmd[0][0]
            else:
                token = sys.argv[1][0]
            #                sendTaskParallel([sys.argv[1][0], sys.argv[1:], 1])
            send(goodPorts, [sys.argv[1][0], sys.argv[1:], 1])

        print("You can type 'quit' or 'q' to exit.")

        keepReadingInput(goodPorts)

        closeAllSerial(goodPorts)
        logger.info("finish!")
        os._exit(0)

    except Exception as e:
        logger.info("Exception")
        closeAllSerial(goodPorts)
        raise e
