#!/usr/bin/python3
# -*- coding: UTF-8 -*-

import sys
sys.path.append("..")
from ardSerial import *

if __name__ == '__main__':
    try:
        goodPorts = {}
        connectPort(goodPorts)
        if len(goodPorts)>0:
            time.sleep(3);
            send(goodPorts,['g',1])
            time.sleep(1);
            send(goodPorts,['ksit', 1])
            send(goodPorts,['I',[12, 90, 13, 90, 14, -40, 15, -40], 1])
            time.sleep(1);
            while True:
                for i in range(1,20,1):
    #                print (['I',[12,i,13,i,14,i,15,i],0])
    #                sendTask(['I',[12,i,13,i,14,i,15,i],0])
                    print (['I',[8,i-50,9,i-50],0])
                    send(goodPorts,['I',[8,i-50,9,i-50],0.5])
                time.sleep(1)
                for i in range(18,-1,-1):
    #                print (['I',[12,i,13,i,14,i,15,i],0])
    #                sendTask(['I',[12,i,13,i,14,i,15,i],0])
                    print (['I',[8,i-50,9,i-50],0])
                    send(goodPorts,['I',[8,i-50,9,i-50],0.5])
                time.sleep(1)
                
            closeAllSerial(goodPorts)
            logger.info("finish!")

    except Exception as e:
        logger.info("Exception")
        closeAllSerial(goodPorts)
        raise e
