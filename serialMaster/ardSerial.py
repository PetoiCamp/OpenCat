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
import config
import glob

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
logging.basicConfig(filename='./logfile.log', filemode='a+', level=logging.INFO, format=FORMAT)
logger = logging.getLogger(__name__)


def printH(head, value):
    print(head, end=' ')
    print(value)


if not config.useMindPlus:
    import tkinter as tk
    import tkinter.messagebox
    sys.path.append("../pyUI")
    from translate import *
    language = languageList['English']

    def txt(key):
        global language
        logger.debug(f"config.strLan is: {config.strLan}.")
        language = languageList[config.strLan]
        return language.get(key, textEN[key])

    # printH("txt('lan'):", txt('lan'))

with open("./logfile.log", "w+", encoding="ISO-8859-1") as logfile:
    pass
time.sleep(1)
logger.info("ardSerial date: Jun. 20, 2024")

def encode(in_str, encoding='utf-8'):
    if isinstance(in_str, bytes):
        return in_str
    else:
        return in_str.encode(encoding)

delayBetweenSlice = 0.001

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
                if angle > 125 or angle < -125:
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
        in_str = token.encode() + struct.pack('b' * len(var), *var) + '~'.encode()

    else:
        if token.isupper():# == 'L' or token == 'I' or token == 'B' or token == 'C':
#            if token == 'C':
#                packType = 'B'
#            else:
#                packType = 'b'
#            port.Send_data(token.encode())
            if len(var)>0:
                message = list(map(int, var))
                if token == 'B':
                    for l in range(len(message)//2):
                        message[l*2+1]*= 8  #change 1 to 8 to save time for tests
                        # print(message[l*2],end=",")
                        # print(message[l*2+1],end=",")
                        logger.debug(f"{message[l*2]},{message[l*2+1]}")
            if token == 'W' or token == 'C':
                in_str = struct.pack('B' * len(message), *message)    # B - unsigned char, e.g. 'B'*3 means 'BBB'
            else:
                in_str = struct.pack('b' * len(message), *message)    # b - signed char, e.g. 'b'*3 means 'bbb'
            in_str = token.encode() + in_str + '~'.encode()

        else:#if token == 'c' or token == 'm' or token == 'i' or token == 'b' or token == 'u' or token == 't':
            message = ""
            for element in var:
                message +=  (str(round(element))+" ")
            in_str = token.encode()+encode(message) +'\n'.encode()

    slice = 0
    while len(in_str) > slice:
        if len(in_str) - slice >= 20:
            port.Send_data(in_str[slice:slice+20])
        else:
            port.Send_data(in_str[slice:])
        slice+=20
        time.sleep(delayBetweenSlice)
    logger.debug(f"!!!! {in_str}")
            #print(encode(in_str))
#            port.Send_data(encode(message))


def serialWriteByte(port, var=None):
    logger.debug(f'serial_write_byte, var={var}')
    if var is None:
        var = []
    token = var[0][0]
    # printH("token:",token)
    # print var
    if (token == 'c' or token == 'm' or token == 'i' or token == 'b' or token == 'u' or token == 't') and len(var) >= 2:
        in_str = ""
        for element in var:
            in_str = in_str + element + " "
        in_str += '\n'
    elif token == 'L' or token == 'I':
        if len(var[0]) > 1:
            var.insert(1, var[0][1:])
        var[1:] = list(map(int, var[1:]))
        in_str = token.encode() + struct.pack('b' * (len(var) - 1), *var[1:]) + '~'.encode()
    elif token == 'w' or token == 'k' or token == 'X':
        in_str = var[0] + '\n'
    else:
        in_str = token + '\n'
    logger.debug(f"!!!!!!! {in_str}")
    # printH("in_str:", in_str)
    port.Send_data(encode(in_str))
    time.sleep(0.01)


def printSerialMessage(port, token, timeout=0):
    if token == 'k' or token == 'K':
        threshold = 8
    else:
        threshold = 3
    if 'X' in token:
        token = 'X'
    startTime = time.time()
    allPrints = ''
    while True:
        time.sleep(0.001)
        #            return 'err'
        if port:
            response = port.main_engine.readline().decode('ISO-8859-1')
            if response != '':
                logger.debug(f"response is: {response}")
                responseTrim = response.split('\r')[0]
                logger.debug(f"responseTrim is: {responseTrim}")
                if responseTrim.lower() == token.lower(): 
                    return [response, allPrints]
                elif token == 'p' and responseTrim == 'k':
                    return [response, allPrints]
                else:
                    # print(response, flush=True)
                    allPrints += response
        now = time.time()
        if (now - startTime) > threshold:
            # print('Elapsed time: ', end='')
            # print(threshold, end=' seconds\n', flush=True)
            logger.debug(f"Elapsed time: {threshold} seconds")
            threshold += 2
            if threshold > 5:
                return -1
        if 0 < timeout < now - startTime:
            return -1


def sendTask(PortList, port, task, timeout=0):  # task Structure is [token, var=[], time]
    logger.debug(f"{task}")
    # printH("task:",task)
    global returnValue
    #    global sync
    #    print(task)
    if port:
        try:
            previousBuffer = port.main_engine.read_all().decode('ISO-8859-1')
            if previousBuffer:
                logger.debug(f"Previous buffer: {previousBuffer}")
                pass
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
#            printH("token",token)
            if token == 'I' or token =='L':
                timeout = 1 # in case the UI gets stuck
            lastMessage = printSerialMessage(port, token, timeout)
            time.sleep(task[-1])
        #    with lock:
        #        sync += 1
        #        printH('sync',sync)
        #    if initialized:
        #        printH('thread',portDictionary[port])
        except Exception as e:
            #        printH('Fail to send to port',PortList[port])
            if port in PortList:
                PortList.pop(port)
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
        t.daemon = True
        t.start()
    for t in threads:
        if t.is_alive():
            t.join()
    return returnValue


def splitTaskForLargeAngles(task):
    token = task[0]
    queue = list()
    if len(task)>2 and (token == 'L' or token == 'I'):
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
                queue[0][-1] = 0.01
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
#    printH('*** @@@ open port ',port) #debug
    if isinstance(port, dict):
        p = list(port.keys())
    elif isinstance(port, list):
        p = port
    queue = splitTaskForLargeAngles(task)
    for task in queue:
        # printH("task",task)
        if len(port) > 1:
            returnResult = sendTaskParallel(p, task, timeout)
        elif len(port) == 1:
            returnResult = sendTask(goodPorts, p[0], task, timeout)
        else:
            # print('no ports')
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


def closeAllSerial(ports, clearPorts=True):
    if clearPorts is True:
        send(ports, ['d', 0], 1)

    for p in ports:
        t = threading.Thread(target=closeSerialBehavior, args=(p,))
        t.daemon = True
        t.start()
        t.join()

    if clearPorts is True:
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
droppedNybble = [
    1, 0, 75, 1,
    0, 30, 0, 0, -5, -5, 15, 15, -75, -75, -60, -60, 60, 60, 30, 30, ]
liftedNybble = [
    1, 0, -75, 1,
    0, -70, 0, 0, 0, 0, 0, 0, 55, 55, 20, 20, 45, 45, 0, 0, ]
restNybble = [
    1, 0, 0, 1,
    -30, -80, -45, 0, -3, -3, 3, 3, 60, 60, -60, -60, -45, -45, 45, 45, ]
sitNybble = [
    1, 0, -20, 1,
    10, -20, -60, 0, -5, -5, 20, 20, 30, 30, -90, -90, 60, 60, 45, 45, ]
strNybble = [
    1, 0, 15, 1,
    10, 70, -30, 0, -5, -5, 0, 0, -75, -75, -45, -45, 60, 60, -45, -45, ]
zeroNybble = [
    1, 0, 0, 1,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ]

postureTableBittle = {
    "balance": balance,
    "buttUp": buttUp,
    # "dropped": dropped,
    # "lifted": lifted,
    # 'flat': flat,
    # 'table': table,
    "rest": rest,
    "sit": sit,
    "str": stretch,
    "zero": zero
}

postureTableBittleR = {
    "balance": balance,
    "buttUp": buttUp,
    # "dropped": dropped,
    # "lifted": lifted,
    # 'flat': flat,
    # 'table': table,
    "rest": rest,
    "sit": sit,
    "str": stretch,
    "zero": zero
}

postureTableNybble = {
    "balance": balanceNybble,
    "buttUp": buttUpNybble,
    # "dropped": droppedNybble,
    # "lifted": liftedNybble,
    # 'flat': flatNybble,
    # 'table': tableNybble,
    "rest": restNybble,
    "sit": sitNybble,
    "str": strNybble,
    "zero": zeroNybble
}
postureTableDoF16 = {
    "balance": balance,
    "buttUp": buttUp,
    # "dropped": dropped,
    # "lifted": lifted,
    # 'flat': flat,
    # 'table': table,
    "rest": rest,
    "sit": sit,
    "str": stretch,
    "zero": zero
}

postureDict = {
    'Nybble': postureTableNybble,
    'Bittle': postureTableBittle,
    'BittleR': postureTableBittleR,
    'DoF16': postureTableDoF16
}

skillFullName = {
        'balance': 'balance',
        'buttUp': 'buttUp',
        'dropped': 'dropped',
        'lifted': 'lifted',
        'lnd': 'landing',
        'rest': 'rest',
        'sit': 'sit',
        'up': 'up',
        'str': 'stretch',
        'calib': 'calib',
        'zero': 'zero',
        'ang':'angry',
         'bf': 'backFlip',
         'bx': 'boxing',
         'ck': 'check',
         'cmh': 'comeHere',
         'dg': 'dig',
         'ff': 'frontFlip',
         'fiv': 'highFive',
         'gdb': 'goodboy',
         'hds': 'handStand',
         'hi': 'hi',
         'hg': 'hug',
         'hsk': 'handShake',
         'hu': 'handsUp',
         'jmp': 'jump',
         'chr': 'cheers',
         'kc': 'kick',
         'mw': 'moonWalk',
         'nd': 'nod',
         'pd': 'playDead',
         'pee': 'pee',
         'pu': 'pushUp',
         'pu1': 'pushUpSingleArm',
         'rc': 'recover',
         'rl': 'roll',
         'scrh': 'scratch',
         'snf': 'sniff',
         'tbl': 'table',
         'ts': 'testServo',
         'wh': 'waveHead',
         'zz': 'zz',
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

def getModelAndVersion(result):
    if result != -1:
        parse = result[1].replace('\r','').split('\n')
        for l in range(len(parse)):
            if 'Nybble' in parse[l] or 'Bittle' in parse[l] or 'DoF16' in parse[l]:
                config.model_ = parse[l]
                config.version_ = parse [l+1]
                config.modelList += [config.model_]
                print(config.model_)
                print(config.version_)
                return
    config.model_ = 'Bittle'
    config.version_ = 'Unknown'
    
def deleteDuplicatedUsbSerial(list):
    for item in list:
        if 'modem' in item: # prefer the USB modem device because it can restart the NyBoard
            serialNumber = item[item.index('modem')+5:]
            for name in list:
                if serialNumber in name and 'modem' not in name:    # remove the "wch" device
                    list.remove(name)
        elif 'serial-' in item: # prefer the "serial-" device 
            serialNumber = item[item.index('serial-')+7:]
            for name in list:
                if serialNumber in name and 'wch' in name:    # remove the "wch" device
                    list.remove(name)
        elif 'cu.SLAB_USBtoUART' in item:
            list.remove(item)
    return list
    
def testPort(PortList, serialObject, p):
    global goodPortCount
    #    global sync
    try:
        time.sleep(3)
        waitTime = 0
        result = serialObject.main_engine
        if result != None:
            result = result.read_all().decode('ISO-8859-1')
            if result != '':
                print('Waiting for the robot to boot up')
                time.sleep(2)
                waitTime = 3
            else:
                waitTime = 2
            result = sendTask(PortList, serialObject, ['?', 0], waitTime)
            if result != -1:
                logger.debug(f"Adding in testPort: {p}")
                PortList.update({serialObject: p})
                goodPortCount += 1
                getModelAndVersion(result)
            else:
                serialObject.Close_Engine()
                print('* Port ' + p + ' is not connected to a Petoi device!')
    #    sync +=1
    except Exception as e:
        print('* Port ' + p + ' cannot be opened!')
        raise e

def checkPortList(PortList, allPorts, needTesting=True):
    threads = list()
    global goodPortCount

    for p in reversed(allPorts):  # assuming the last one is the most possible port
        # if p == '/dev/ttyAMA0':
        #     continue
        serialObject = Communication(p, 115200, 1)
        if needTesting is True:
            t = threading.Thread(target=testPort,
                                 args=(PortList, serialObject, p.split('/')[-1]))    # remove '/dev/' in the port name
            threads.append(t)
            t.daemon = True
            t.start()
        else:
            logger.debug(f"Adding in checkPortList: {p}")
            PortList.update({serialObject: p.split('/')[-1]})    # remove '/dev/' in the port name
            goodPortCount += 1
            logger.info(f"Connected to serial port: {p}")
    if needTesting is True:
        for t in threads:
            if t.is_alive():
                t.join(8)


def keepCheckingPort(portList, cond1=None, check=True, updateFunc = lambda:None):
    # portList is a dictionary, the structure is {SerialPort Object(<class 'SerialCommunication.Communication'>): portName(string), ...}
    # allPorts is a string list which delete the duplicated port(Reserve the name of the serial port that contains the usbmodem)
    # portStrList is the serial port string list
    global portStrList
    allPorts = Communication.Print_Used_Com()
    logger.debug(f"allPorts is {allPorts}")
    if cond1 is None:
        cond1 = lambda: len(portList) > 0

    while cond1():
        time.sleep(0.5)
        currentPorts = Communication.Print_Used_Com()    # string list
        # logger.debug(f"currentPorts is {currentPorts}")
        
        if set(currentPorts) - set(allPorts):
            time.sleep(1) #usbmodem is slower in detection
            currentPorts = Communication.Print_Used_Com()
            # newPort = deleteDuplicatedUsbSerial(list(set(currentPorts) - set(allPorts)))
            newPort = list(set(currentPorts) - set(allPorts))
            if check:
                time.sleep(0.5)
                checkPortList(portList, newPort)
            else:
                for p in newPort:
                    logger.debug(f"Adding serial port: {p}")
                    portName = p.split('/')[-1]
                    portStrList.insert(0, portName)  # remove '/dev/' in the port name
                    tk.messagebox.showinfo(title=txt('Info'), message=txt('New port prompt') + portName)
            updateFunc()
        elif set(allPorts) - set(currentPorts):
            time.sleep(1) #usbmodem is slower in detection
            currentPorts = Communication.Print_Used_Com()
            closedPort = list(set(allPorts) - set(currentPorts))
            if check:
                inv_dict = {v: k for k, v in portList.items()}
                for p in closedPort:
                    if inv_dict.get(p.split('/')[-1], -1) != -1:
                        logger.info(f"Removing {p.split('/')[-1]}")
                        portList.pop(inv_dict[p.split('/')[-1]])
            else:
                for p in reversed(closedPort):
                    portName = p.split('/')[-1]
                    if portName in portStrList:
                        logger.info(f"Removing serial port:{portName}")
                        portStrList.remove(portName)
            updateFunc()
        allPorts = copy.deepcopy(currentPorts)

def showSerialPorts(allPorts):
    # currently an issue in pyserial where for newer raspiberry pi os
    # (Kernel version: 6.1, Debian version: 12 (bookworm)) or ubuntus (22.04)
    # it classifies the /dev/ttyS0 port as a platform port and therefore won't be queried
    # https://github.com/pyserial/pyserial/issues/489
    if os.name == 'posix' and sys.platform.lower()[:5] == 'linux':
        extra_ports = glob.glob('/dev/ttyS*')
        for port in extra_ports:
            if port not in allPorts:
                allPorts.append(port)
        for item in allPorts:
            if 'AMA0' in item:
                allPorts.remove(item)
        
    # allPorts = deleteDuplicatedUsbSerial(allPorts)
    for index in range(len(allPorts)):
        logger.debug(f"port[{index}] is {allPorts[index]} ")
    logger.info(f"*** Available serial ports: ***")
    # print(*allPorts, sep = "\n")
    for index in range(len(allPorts)):
        logger.info(f"{allPorts[index]} ")

    if platform.system() != "Windows":
        for p in allPorts:
             if 'cu.usb' in p:
                print('\n* Manually connect to the following port if it fail to connect automatically\n')
                if config.useMindPlus:
                    print(p.replace('/dev/',''),end='\n\n')
                else:
                    print(p, end='\n\n')

                
def connectPort(PortList, needTesting=True, needSendTask=True, needOpenPort=True):
    global initialized
    global goodPortCount
    allPorts = Communication.Print_Used_Com()
    showSerialPorts(allPorts)

    if len(allPorts) > 0:
        goodPortCount = 0
        if needOpenPort is True:
            checkPortList(PortList, allPorts, needTesting)
    initialized = True
    if needOpenPort is True:
        if len(PortList) == 0:
            print('No port found! Please make sure the serial port can be recognized by the computer first.')
            if not config.useMindPlus:
                print('Replug mode')
                replug(PortList, needSendTask, needOpenPort)
        else:
            logger.info(f"Connect to serial port list:")
            for p in PortList:
                logger.debug(f"datatype of p : {type(p)}")
                logger.info(f"{PortList[p]}")
                portStrList.append(PortList[p])
    else:
        if len(allPorts) == 0 or len(allPorts) > 1:
            print('Replug mode')
            replug(PortList, needSendTask, needOpenPort)
        else:   # len(allPorts) == 1
            portName = allPorts[0].split('/')[-1]
            portStrList.insert(0, portName)    # remove '/dev/' in the port name

                                
def replug(PortList, needSendTask=True, needOpenPort=True):
    global timePassed
    print('Please disconnect and reconnect the device from the COMPUTER side')
    
    window = tk.Tk()
    window.geometry('+800+500')
    def on_closing():
        window.destroy()
        os._exit(0)
    window.protocol('WM_DELETE_WINDOW', on_closing)
    window.title(txt('Replug mode'))

    thres = 10 # time out for the manual plug and unplug
    print('Counting down to manual mode:')
    
    def bCallback():
        labelC.destroy()
        buttonC.destroy()
        
        labelT['text']= txt('Counting down to manual mode: ')
        labelT.grid(row=0,column=0)
        
        label.grid(row=1,column=0)
        label['text']="{} s".format(thres)
        countdown(time.time(),copy.deepcopy(Communication.Print_Used_Com()))
        
    labelC = tk.Label(window, font='sans 14 bold', justify='left')
    labelC['text'] = txt('Replug prompt')
    labelC.grid(row=0, column=0)
    buttonC = tk.Button(window, text=txt('Confirm'), command=bCallback)
    buttonC.grid(row=1, column=0, pady=10)
    labelT = tk.Label(window, font='sans 14 bold')
    label = tk.Label(window, font='sans 14 bold')
    def countdown(start,ap):
        global goodPortCount
        global timePassed
        curPorts = copy.deepcopy(Communication.Print_Used_Com())

        if len(curPorts) != len(ap):
            time.sleep(0.5)    # USB modem serial takes longer time to get ready
            curPorts = copy.deepcopy(Communication.Print_Used_Com())
            print(ap)
            print('---')
            print(curPorts)
            if len(curPorts) < len(ap):
                ap = curPorts
                start = time.time()
                timePassed = 0
            else:
                dif = list(set(curPorts)-set(ap))
                # dif = deleteDuplicatedUsbSerial(dif)
                print("diff:",end=" ")
                print(dif)
                
                success = False
                for p in dif:
                    try:
                        portName = p.split('/')[-1]
                        if needOpenPort is True:
                            logger.info(f"Connected to serial port: {p}")
                            serialObject = Communication(p, 115200, 1)
                            PortList.update({serialObject: portName})
                        portStrList.insert(0, portName)  # remove '/dev/' in the port name
                        goodPortCount += 1
                        tk.messagebox.showinfo(title=txt('Info'), message=txt('New port prompt') + portName)

                        if (needOpenPort is True) and (needSendTask is True):
                            time.sleep(2)
                            result = sendTask(PortList, serialObject, ['?', 0])
                            getModelAndVersion(result)
                        
                        success = True
                    except Exception as e:
                        raise e
                        print("Cannot open {}".format(p))
                for p in ap:
                    # logger.debug(f"datatype of p : {type(p)}")
                    # logger.info(f"{PortList[p]}")
                    portStrList.append(p)

                if success:
                    window.destroy()
                else:
                    labelT.destroy()
                    label.destroy()
                    manualSelect(PortList, window, needSendTask, needOpenPort)
                return

        if time.time() - start > thres:
            labelT.destroy()
            label.destroy()
            manualSelect(PortList, window, needSendTask, needOpenPort)
            return
        elif (time.time() - start) % 1 < 0.1:
            print(thres - round(time.time() - start) // 1)
            label['text'] = "{} s".format((thres - round(time.time() - start) // 1))
        window.after(100, lambda: countdown(start, ap))

    window.focus_force()  # new window gets focus
    window.mainloop()
    
def selectList(PortList,ls,win, needSendTask=True, needOpenPort=True):
    
    global goodPortCount
    for i in ls.curselection():
        p = ls.get(i)#.split('/')[-1]
        try:
            print(p)
            print(p.split('/')[-1])
            if needOpenPort is True:
                serialObject = Communication(p, 115200, 1)
                PortList.update({serialObject: p.split('/')[-1]})
            portStrList.append(p.split('/')[-1])
            goodPortCount += 1
            logger.info(f"Connected to serial port: {p}")
            
            if (needOpenPort is True) and (needSendTask is True):
                time.sleep(2)
                result = sendTask(PortList, serialObject, ['?', 0])
                getModelAndVersion(result)
            win.withdraw()

        except Exception as e:
            tk.messagebox.showwarning(title=txt('Warning'), message=txt('* Port ') + p + txt(' cannot be opened'))
            print("Cannot open {}".format(p))
            raise e
    win.destroy()

def manualSelect(PortList, window, needSendTask=True, needOpenPort=True):
    # allPorts = deleteDuplicatedUsbSerial(Communication.Print_Used_Com())
    allPorts = Communication.Print_Used_Com()
    window.title(txt('Manual mode'))
    l1 = tk.Label(window, font = 'sans 14 bold')
    l1['text'] = txt('Manual mode')
    l1.grid(row=0,column = 0)
    l2 = tk.Label(window, font='sans 14 bold')
    l2["text"]=txt('Please select the port from the list')
    l2.grid(row=1,column=0)
    ls = tk.Listbox(window,selectmode="multiple")
    ls.grid(row=2,column=0)
    def refreshBox(ls):
        # allPorts = deleteDuplicatedUsbSerial(Communication.Print_Used_Com())
        allPorts = Communication.Print_Used_Com()
        ls.delete(0,tk.END)
        for p in allPorts:
            ls.insert(tk.END,p)
    for p in allPorts:
        ls.insert(tk.END,p)
    bu = tk.Button(window, text=txt('OK'), command=lambda:selectList(PortList, ls, window, needSendTask, needOpenPort))
    bu.grid(row=2, column=1)
    bu2 = tk.Button(window, text=txt('Refresh'), command=lambda:refreshBox(ls))
    bu2.grid(row=1, column=1)
    tk.messagebox.showwarning(title=txt('Warning'), message=txt('Manual mode'))
    window.mainloop()

#if need to open serial port, use objects goodPorts
goodPorts = {}      # goodPorts is a dictionary, the structure is {SerialPort Object(<class 'SerialCommunication.Communication'>): portName(string), ...}

portStrList = []    # portStrList is the serial port string list
initialized = False
goodPortCount = 0
sync = 0
lock = threading.Lock()
returnValue = ''
timePassed = 0

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
        printH('Model list',config.modelList)
        print("You can type 'quit' or 'q' to exit.")

        keepReadingInput(goodPorts)

        closeAllSerial(goodPorts)
        logger.info("finish!")
        os._exit(0)

    except Exception as e:
        logger.info("Exception")
        closeAllSerial(goodPorts)
        raise e
