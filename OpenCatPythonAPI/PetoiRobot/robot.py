#  -*- coding: UTF-8 -*-

# MindPlus
# Python
from .ardSerial import *
from .SerialCommunication import *
import struct
import time
import re
import platform
import os

intoCameraMode = False

if platform.system() == "Windows":    # for Windows
    seperation = '\\'
    homeDri = os.getenv('HOMEDRIVE') 
    homePath = os.getenv('HomePath') 
    configDir = homeDri + homePath
else:  # for Linux & macOS
    seperation = '/'
    home = os.getenv('HOME') 
    configDir = home 
configDir = configDir + seperation +'.config' + seperation +'Petoi'

modelName = 'Bittle'

# use to print debug information
def printH(head, value):
    print(head, end=' ')
    print(value)

printH("Mind+ date: ", "Feb 25, 2025")


def makeDirectory(path):
    # delete spaces in the path string
    path = path.strip()
    # delete the '\' at the end of path string
    path = path.rstrip("\\").rstrip("/")
    # path = path.rstrip("/")
	
    # check whether the path exists
    isExists = os.path.exists(path)
    
    if not isExists:
        # printH("model name:", path.split(seperation)[-1])
        if path.split(seperation)[-1] == "BittleX+Arm":
            path = seperation.join(path.split(seperation)[:-1]) + seperation + "BittleR"
            # printH("path:", path)
            if not os.path.exists(path):
                # Create the directory if it does not exist
                path = seperation.join(path.split(seperation)[:-1]) + seperation + "BittleX+Arm"
                os.makedirs(path)
                print(path + ' creat successfully')
            else:
                # Change the directory name "BittleR" to "BittleX+Arm"
                os.rename(path,seperation.join(path.split(seperation)[:-1]) + seperation + "BittleX+Arm")
                # print("Rename successfully!")
        else:
            # Create the directory if it does not exist
            os.makedirs(path)
            print(path + ' creat successfully')
        return True
    else:
        # If the directory exists, it will not be created and prompt that the directory already exists.
        print(path + ' already exists')
        return False

makeDirectory(configDir)

def file_name(file_dir):
    # printH("file_dir:",file_dir)
    File_Name=[]
    for files in os.listdir(file_dir):
        # printH("files:",files)
        if os.path.splitext(files)[1] == '.md':
            File_Name.append(files.split('.')[0])
            # printH("File_Name:",files.split('.')[0])
    return File_Name


BittleData = '# Token\n\
K\n\
\n\
# Data\n\
{\n\
  -5,  0,   0, 1,\n\
   1,  2,   3,\n\
   0,-20, -60,   0,   0,   0,   0,   0,  35,  30, 120, 105,  75,  60, -40, -30,     4, 2, 0, 0,\n\
  35, -5, -60,   0,   0,   0,   0,   0, -75,  30, 125,  95,  40,  75, -45, -30,    10, 0, 0, 0,\n\
  40,  0, -35,   0,   0,   0,   0,   0, -60,  30, 125,  95,  60,  75, -45, -30,    10, 0, 0, 0,\n\
   0,  0, -45,   0,  -5,  -5,  20,  20,  45,  45, 105, 105,  45,  45, -45, -45,     8, 0, 0, 0,\n\
   0,  0,   0,   0,   0,   0,   0,   0,  30,  30,  30,  30,  30,  30,  30,  30,     5, 0, 0, 0,\n\
};'

NybbleData = '# Token\n\
K\n\
\n\
# Data\n\
{\n\
  -5,   0,   0,   1,\n\
   2,   3,   3,\n\
   0, -20, -65,   0,   0,   0,   0,   0,  30,  30, -90, -90,  60,  60,  45,  45,   8,   1,   0,   0,\n\
   0, -20, -65,   0,   0,   0,   0,   0,  30,  41, -90, -72,  60,  46,  45,   5,   8,   2,   0,   0,\n\
  35, -15, -65,   0,  -3,  -3,   3,   3, -75,  41, -85, -72,  40,  65,  60,   0,   8,   0,   0,   0,\n\
  40, -10, -55,   0,  -3,  -3,   3,   3, -60,  41, -80, -79,  60,  65,  60,   0,   4,   0,   0,   0,\n\
   0,   0,   0,   0,   0,   0,   0,   0,  30,  41, -30, -30,  30,  30, -30, -30,  12,   0,   0,   0,\n\
};'

BittleRData = '# Token\n\
K\n\
\n\
# Data\n\
{\n\
  -6, 0, 0, 1,\n\
   1, 2, 3,\n\
   0, -20,   0,   0,   0,   0,   0,   0,  35,  30, 120, 105,  75,  60, -40, -30,	 8, 2, 0, 0,\n\
  -5,  -5,   0,   0,   0,   0,   0,   0, -99,  30, 125,  95,  40,  75, -45, -30,	10, 0, 0, 0,\n\
   0,  -5,   0,   0,   0,   0,   0,   0, -90,  30, 125,  95,  62,  75, -45, -30,	10, 0, 0, 0,\n\
 -15,  -5,   0,   0,   0,   0,   0,   0, -90,  30, 125,  95,  62,  75, -45, -30,	 8, 0, 0, 0,\n\
   0,  -5,   0,   0,  -5,  -5,  20,  20,  45,  45, 105, 105,  45,  45, -45, -45,	 8, 0, 0, 0,\n\
   0,   0,   0,   0,   0,   0,   0,   0,  30,  30,  30,  30,  30,  30,  30,  30,	 8, 0, 0, 0,\n\
};'

modelDict = {'Bittle': BittleData, 'Nybble': NybbleData, 'BittleX+Arm': BittleRData,}  # 'BittleR': BittleRData, 

def creatSkillFile():
    for key in modelDict:
        modelDir = configDir + seperation + 'SkillLibrary' + seperation + key
        makeDirectory(modelDir)
        filePath = modelDir + seperation + 'skillFileName.md'
        if not os.path.exists(filePath):
            try:
                with open(filePath, 'w+', encoding="utf-8") as f:
                    f.write(modelDict[key])
                    time.sleep(0.1)
            except Exception as e:
                return False, 'save failed:{}'.format(e)

creatSkillFile()


'''
def getPortList():
    if isinstance(goodPorts, dict):
        p = list(goodPorts.keys())
    elif isinstance(goodPorts, list):
        p = goodPorts
    return p
'''


# deactivate the gyro
def deacGyro():
    boardVer = config.version_
    # printH("boardVer:", boardVer)
    if boardVer[0] == 'N':
        res = send(goodPorts, ['G', 0])
        # printH("gyro status:",res )
        logger.debug(f'gyro status:{res}')
        if res != -1 and res[0][0] == 'G':
            res = send(goodPorts, ['G', 0])
            # printH("gyro status:",res )
            logger.debug(f'gyro status:{res}')
    else:
        res = send(goodPorts, ['gb', 0])
        if res != -1 and res[0][0] == 'g':
            logger.debug(f'gyro is deactived successfully.')


# get the current angle list of all joints
def getAngleList():
    token = 'j'
    task = [token, 0]
    # in_str = token + '\n'
    # com.Send_data(encode(in_str))
    # rawData = checkResponse(token)
    # p = getPortList()
    # rawData = sendTask(goodPorts, p[0], task)
    rawData = send(goodPorts, task)

    logger.debug(f'rawData={rawData}')
    p = re.compile(r'^(.*),',re.MULTILINE)
    for one in p.findall(rawData[1]):
        angle = re.sub('\t','',one)
    strAngleList = angle.split(',')
    logger.debug(f'strAngleList={strAngleList}')
    angleList = list(map(lambda x:int(x),strAngleList)) #angle value have to be integer
    logger.debug(f'angleList={angleList}')
    return angleList


# get the current angle value of a joint 
def getAngle(index):
    token = 'j' 
    task = [token, [index], 0]
    rawData = send(goodPorts, task)
    if rawData!=-1:
        logger.debug(f'rawData={rawData}')
        # result = rawData[1][:-2]
        result = rawData[1].replace('\r','').replace('\n','')    # delete '\r\n'
        # printH("result is: ",result)
        if  "=" in result:
            try:
                value = int(result[1:])
                # printH("value is: ",value)
                return value
            except Exception as e:
                print('* No angle value got!')
                raise e


def getCurAng(index):
    currentVal = getAngle(index)
    return currentVal


# creat an absolut value list
def absValList(num1, num2):
    return [(int(num1), num2)]


# rotate angle from relative value to absolute value
# creat a offset value list
def relativeValList(index, symbol, angle):
    # newList = []
    # relative = int(symbol) * angle
    # if int(symbol) == 1:
    #     relative = '+' + str(angle)
    # else:
    #     relative = '-' + str(angle)
    # logger.debug(f'relative={relative}')
    # newList.append(index)
    # newList.append(relative)
    # curAngle = getCurAng(index)
    # absAngle = curAngle + int(symbol) * angle
    # absAngle = min(125,max(-125,absAngle))
    # logger.debug(f'absAngle={absAngle}')
    # newList.append(index)
    # newList.append(absAngle)
    # return newList
    # printH("type:", isinstance(symbol, str))
    return [(int(index), int(symbol), angle)]


# rotate the joints sequentially or simultaneously
def rotateJoints(token, var, delayTime):
    # currentAngleList = getAngleList()
    newList = []
    # delay = delayTime
    for iA in var:
        if isinstance(iA, int):
            newList += [iA]
        elif isinstance(iA, tuple):
            if len(iA)==2:
                newList += [iA[0], iA[1]]
                # currentAngleList[iA[0]] = iA[1]
            elif len(iA)==3:
                currentAngleList = getAngleList()
                currentAngleList[iA[0]] += iA[1]*iA[2]
                currentAngleList[iA[0]] = min(125,max(-125,currentAngleList[iA[0]]))
                newList += [iA[0], currentAngleList[iA[0]]]
            # printH("iA[0]:", iA[0])
            # if iA[0] == 2 and modelName == 'BittleR':
            #     delay = 0.1
            #     printH("delay:", delay)

    sendLongCmd(token, newList, delayTime)


# play tones
def play(token, var, delayTime):
    newList = []

    for iA in var:
        if isinstance(iA, int):
            newList += [iA]
        elif isinstance(iA, tuple):
            newList += [iA[0], iA[1]]
    sendLongCmd(token, newList, delayTime)


'''
# combine the list
def combineList(list1, list2):
    cList = list1
    cList.extend(list2)
    return cList
'''

# encode the character to bytes
def encode(in_str, encoding='utf-8'):
    if isinstance(in_str, bytes):
        return in_str
    else:
        return in_str.encode(encoding)

def printSkillFileName():
    global modelName
    config.model_ = config.model_.replace(' ','')
    if config.model_ == "BittleR":
        modelName = 'BittleX+Arm'
    elif 'Bittle' in config.model_ and config.model_!= 'BittleX+Arm':
        modelName = 'Bittle'
    elif 'Nybble' in config.model_:
        modelName = 'Nybble'
    else:
        modelName = config.model_
    printH("modelName:", modelName)
    skillDir = configDir + seperation + 'SkillLibrary' + seperation + modelName
    skill_file_name=file_name(skillDir)
    print("*** The skill names you can call are as follows: ***")
    for skillName in skill_file_name:
        printH("* ",skillName)
    print("******************************")

# open the serial port 
def openPort(port):
    # com = Communication(port,115200,timeout=0.002)
    allPorts = Communication.Print_Used_Com()
    showSerialPorts(allPorts)
    if platform.system() != "Windows" and '/dev' not in port:
        port = '/dev/' + port
    serialObject = Communication(port, 115200, 1)
    testPort(goodPorts, serialObject, port.split('/')[-1])
    t = 1
    print('Time delay after open port: ', str(t))
    time.sleep(t)
    printSkillFileName()
    deacGyro()


# auto connect serial ports
def autoConnect():
    connectPort(goodPorts)
    logger.debug(f'goodPorts: {goodPorts}')
    printSkillFileName()
    deacGyro()
    

'''
# check if there is a response
def checkResponse(tk, timeout=0):
    token = tk
    if token == 'k' or token == 'K':
        threshold = 4
    else:
        threshold = 1
    startTime = time.time()
    allPrints = ''
    while True:
        response = com.Read_Line()
        # printH('a',response)
        response=response.decode('ISO-8859-1')
        # printH('b',response)
        if response != '':
            # printH('c',response)
            response = response[:-2]   # delete '\r\n'
            if response.lower() == token.lower():
                # printH('response: ', response)
                # printH('allPrints: ', allPrints)
                return [response, allPrints]
            else:
                # print(response, flush=True)
                allPrints += response + '\n'
        now = time.time()
        if (now - startTime) > threshold:
            print('Elapsed time: ', end='')
            print(threshold, end=' seconds\n', flush=True)
            threshold += 2
            if threshold > 10:
                return -1
        if 0 < timeout < now - startTime:
            return -1
        time.sleep(0.001)
'''


# send a short skill string
def sendSkillStr(skillStr, delayTime):
    # in_str = skillStr + '\n'
    # com.write(bytes(skillStr, 'utf-8'))
    # com.Send_data(encode(in_str))
    # checkResponse('k')
    # time.sleep(delayTime)
    logger.debug(f'skillStr={skillStr}')
    send(goodPorts, [skillStr,delayTime])

# perform a skill exported from the Skill Composer
# the file directory is: "/$HOME/.config/Petoi/SkillLibrary/{model}/xxx.md" for Linux and macOS
# the file directory is: "%HOMEDRIVE%\%HomePath%\.config\Petoi\SkillLibrary\{model}\xxx.md" for Windows
def loadSkill(fileName, delayTime):
    global modelName
    # get the path of the exported skill file
    if ".md" in fileName:
        skillFilePath = configDir + seperation + 'SkillLibrary' + seperation + modelName + seperation + fileName
    else:
        skillFilePath = configDir + seperation + 'SkillLibrary' + seperation + modelName + seperation + fileName +'.md'

    logger.debug(f'skillFilePath:{skillFilePath}')

    # open the skill file
    with open(skillFilePath,"r",encoding='utf-8') as f:
        line = f.readline()  # get the whole line content
        while line:
            #key words
            if ("# Token") in line:
                # get the token
                line = next(f) #get the next line
                token = line.replace("\n","")
                logger.debug(f'token:{token}')
            if ("# Data") in line:
                # get the skill data
                lines = f.readlines()  # get the rest lines of the file
                logger.debug(f'lines:{lines}')
            line = f.readline()
    skillDataString = ''.join((str(x) for x in lines))
    logger.debug(f'skillDataString:{skillDataString}')
    skillDataString = ''.join(skillDataString.split()).split('{')[1].split('}')[0].split(',')
    logger.debug(f'skillDataString:{skillDataString}')
    if skillDataString[-1] == '':
        skillDataString = skillDataString[:-1]
    cmdList = list(map(int, skillDataString))
    logger.debug(f'cmdList:{cmdList}')

    send(goodPorts, [token, cmdList, delayTime])


# send a command string
def sendCmdStr(cmdStr, delayTime):
    logger.debug(f'serialCmd={cmdStr}')
    if cmdStr != '':
        token = cmdStr[0]
        logger.debug(f'cmdList={token}')
        cmdList = cmdStr[1:].replace(',',' ').split()
        logger.debug(f'cmdList={cmdList}')

        if len(cmdList) <= 1:
            send(goodPorts, [cmdStr, delayTime])
        else:
            cmdList = list(map(lambda x:int(x),cmdList))
            send(goodPorts, [token, cmdList, delayTime])


# if token == 'c' or token == 'm' or token == 'i' or token == 'b' or token == 'u' or token == 't'
# if the length of list var is > 22, send 22 elements each time, untill all the elements are sent over
def sendLongCmd(token, var, delayTime):
    # message = token
    # count = 0
    var = list(map(int, var))
            
    # in_str = token.encode() + struct.pack('b' * len(var), *var) + '~'.encode()
    # com.Send_data(encode(in_str))
    # for element in var:
    #     message += str(element) + " "
    #     count +=1
    #     if count == 22 or count == len(var):
    #         com.Send_data(encode(message))
    #         message = ""
    #         count = 0
    # checkResponse(token)
    # time.sleep(delayTime)
    send(goodPorts,[token, var, delayTime])


# get value from a request
def getValue(task, dataType ="int"):
    rawData = send(goodPorts, task)
    if rawData!=-1:
        logger.debug(f'rawData={rawData}')
        # result = rawData[1][:-2]
        result = rawData[1].replace('\r','').replace('\n','')    # delete '\r\n'
        # printH("result is: ",result)
        if "=" in result:
            index = result.find("=") + 1
            try:
                if dataType == "float":
                    value = float(result[index:])
                elif dataType == "int":
                    value = int(result[index:])
                elif dataType == "tuple":
                    tmpList = result[index:].split('\t')
                    sizeStr = tmpList[2].split(' ')
                    width = int(sizeStr[-1])
                    # printH("width is: ",width)
                    height = int(tmpList[3])
                    # printH("height is: ",height)
                    tupWidthHeight = (width, height)
                    if width != 0:
                        value = tuple(map(float, tmpList[:2])) + tupWidthHeight
                    else:
                        value = (-255, -255, 0, 0)
                        print('* No value got!')
                # printH("value is: ",value)
            except Exception as e:
                print('* Got value error!')
                raise e
        else:
            value = (-255, -255, 0, 0)
            print('* No value got!')
        return value
    else:
        value = (-255, -255, 0, 0)
        print('* No value got!')
        return value


# get analog value of a pin
def readAnalogValue(pin):
    token = 'R'
    task = [token, [97, pin], 0]
    return getValue(task)
    

# get digital value of a pin
def readDigitalValue(pin):
    token = 'R'
    task = [token, [100, pin], 0]

    # p = getPortList()
    # rawData = sendTask(goodPorts, p[0], task)
    return getValue(task)


# get distance value(cm) from ultrasonic sensor
def readUltrasonicDistance(triggerPin, echoPin):
    token = 'XU'
    task = [token, [int(triggerPin), int(echoPin)], 0]
    return getValue(task, dataType ="float")

# get the coordinates of the identified target from camera module
def readCameraCoordinate():
    global intoCameraMode
    # Check if the camera task isactivated.
    if intoCameraMode == False:
        res = send(goodPorts, ['XCr', 0])
        if res != -1 :
            # printH("intoCameraMode is:",intoCameraMode)
            logger.debug(f'res={res}')
            # p = re.compile(r'^(.*),',re.MULTILINE)
            p = re.compile(r'^(?=.*[01])(?=.*,).+$', flags=re.MULTILINE)
            if res[1] != '':
                logger.debug(f'res[1]={res[1]}')
                for one in p.findall(res[1]):
                    val = re.sub('\t','',one)
                val = val.replace('\r','').replace('\n','')    # delete '\r\n'
                strFlagList = val.split(',')[:-1]
                flagList = list(map(lambda x:int(x),strFlagList))    # flag value have to be integer
                logger.debug(f'flagList={flagList}')
                if flagList[9] == 1:
                    task = ['XCp', 0]
                    intoCameraMode = True
                    return getValue(task, dataType ="tuple")
                else:
                    tup = (0,0)
                    print("No target detected!")
                    return tup
            else:
                task = ['XCp', 0]
                intoCameraMode = True
                return getValue(task, dataType ="tuple")
            
    else:
        # printH("intoCameraMode is:",intoCameraMode)
        task = ['XCp', 0]
        return getValue(task, dataType ="tuple")


# set analog value of a pin
def writeAnalogValue(pin, val):
    token = 'W'
    task = [token, [97, pin, val], 0]

    rawData = send(goodPorts, task)


# set digital value of a pin
def writeDigitalValue(pin, val):
    token = 'W'
    task = [token, [100, pin, val], 0]

    rawData = send(goodPorts, task)


# # initialize a list
# def initList(var):
#     angList = var.split(",")
#     str_to_int = list(map(int, angList))
#     return str_to_int

# close the serial port
def closePort():
        """
        close serial port
        """
        # global com
        # com.Close_Engine()  # close serial port
        closeAllSerial(goodPorts)

