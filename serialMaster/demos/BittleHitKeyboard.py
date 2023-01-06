#!/usr/bin/python3
# -*- coding: UTF-8 -*-

import sys
sys.path.append("..")
from ardSerial import *

if __name__ == '__main__':
    try:
        goodPorts = {}
        connectPort(goodPorts)
#   0,   0, -45,   0,  -5,  -5,  20,  20,  45,  45, 105, 105,  45,  45, -45, -45,
#   0,   0, -45,   0,  -5,  -5,  20,  20,  25,   3,  98,  98,  54,  54, -36, -36,
#   0,   0,  45,   0,  -5,  -5,  20,  20,   3,  25,  98,  98,  54,  54, -36, -36,
        arm = [25,3]
        skill = []
        rows = 15
        for i in range(rows):
#                print (['I',[12,i,13,i,14,i,15,i],0])
#                sendTask(['I',[12,i,13,i,14,i,15,i],0])
            skill += [ rows-i*2,   0, 0,   0,  -5,  -5,  20,  20,  arm[i%2],   arm[(i+1)%2],  98,  98,  54,  54, -51, -51, 24,0,0,0]
            
#                for i in range(18,-1,-1):
#    #                print (['I',[12,i,13,i,14,i,15,i],0])
#    #                sendTask(['I',[12,i,13,i,14,i,15,i],0])
#                    print (['I',[8,i-50,9,i-50],0])
#                    send(goodPorts,['I',[8,i-50,9,i-50],0.5])
#                time.sleep(1)
        skill = [-rows,0,0,1,0,0,0,]+skill
        print(skill)
        if len(goodPorts)>0:
            time.sleep(3);
            send(goodPorts,['g',1])
            time.sleep(1);
            send(goodPorts,['ksit', 1])
            send(goodPorts,['L',[ 0,   0, -45,   0,  -5,  -5,  20,  20,  45,  45, 105, 105,  45,  45, -45, -45], 1])
            for r in range(5):
                send(goodPorts,['K',skill,0])
            closeAllSerial(goodPorts)
            logger.info("finish!")

    except Exception as e:
        logger.info("Exception")
        closeAllSerial(goodPorts)
        raise e
