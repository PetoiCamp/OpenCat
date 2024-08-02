#!/usr/bin/python3
# -*- coding: UTF-8 -*-

import sys
sys.path.append("..")
from ardSerial import *
# the following skill arrays are identical to those in InstinctBittle.h
skillNames =["pickUpL","dropDownL","huntL","showOffL","putAwayL","throwAwayL","shootL","bdF","bk","bkArmF","bkArmLF","bkF","bkL","crArmF","crArmL","crF","crL","gpF","gpL","hlw","jpF","lftF","lftL","phF","phL","trArmF","trArmL","trF","trL","vtArmF","vtF","vtL","wkArmF","wkArmL","wkF","wkL","balance","buttUp","calib","dropped","lifted","lnd","rest","sit","str","up","zeroN","ang","bf","bx","chr","ck","clap","cmh","dg","ff","fiv","gdb","hds","hg","hi","hsk","hu","jmp","kc","knock","lpov","mw","nd","pd","pee","pu","pu1","rc","rl","scrh","snf","tbl","ts","wh","zz"]


model = 'Bittle'
postureTable = postureDict[model]

E_RGB_ALL = 0
E_RGB_RIGHT = 1
E_RGB_LEFT = 2

E_EFFECT_BREATHING = 0
E_EFFECT_ROTATE = 1
E_EFFECT_FLASH = 2
E_EFFECT_NONE = 3


if __name__ == '__main__':
    try:
        '''
        testSchedule is used to test various serial port commands
        '''
        goodPorts = {}
        connectPort(goodPorts)
        t=threading.Thread(target = keepCheckingPort, args = (goodPorts,))
        t.start()
        parallel = False
#        if len(goodPorts)>0:
#        time.sleep(2);
        for task in skillNames:  # execute the tasks in the testSchedule
            print(['k'+task,1])
            send(goodPorts, ['k'+task,1])
        send(goodPorts, ['kbalance',0.5])

# the following test checks the serial pass through mode
#        send(goodPorts,['kup',1])
#        send(goodPorts,['w',[0,2300],0.1])
#        send(goodPorts,['w',[0,600],0.1])
#        while 1:
#            send(goodPorts,['W',[100,9,1,100,8,1],0.1]) # digital write. 'd' = 100.
#            send(goodPorts,['W',[100,9,1,100,8,0],0.1])
#            send(goodPorts,['W',[100,9,0,100,8,1],0.1])
#            send(goodPorts,['W',[100,9,0,100,8,0],0.1])
#            for i in range(8):
#                send(goodPorts,['W',[97,6,i*30,97,9,255-i*30,97,10,255-i*30],0.1]) # analog write. 'a' = 97.
#            for i in range(8):
#                send(goodPorts,['W',[97,6,255-i*30,97,9,i*30,97,10,i*30],0.1])
#            send(goodPorts,['R',[97,16,97,17],0.1])
#            time.sleep(1)
#
#        schedulerToSkill(goodPorts, testSchedule) # compile the motion related instructions to a skill and send it to the robot.
        closeAllSerial(goodPorts)
        logger.info("finish!")
        os._exit(0)

    except Exception as e:
        logger.info("Exception")
        closeAllSerial(goodPorts)
        os._exit(0)
        raise e
