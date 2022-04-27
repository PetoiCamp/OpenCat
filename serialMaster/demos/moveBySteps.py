#!/usr/bin/python3
# -*- coding: UTF-8 -*-

import sys
sys.path.append("..")
from ardSerial import *

if __name__ == '__main__':
    try:
        serialObject = connectPort()
        if serialObject!=-1:
            time.sleep(3);
            sendTask(['g',1])
            time.sleep(1);
            sendTask(['ksit', 1])
            sendTask(['I',[12, 90, 13, 90, 14, -40, 15, -40], 1])
            time.sleep(1);
            while True:
                for i in range(1,20,1):
    #                print (['I',[12,i,13,i,14,i,15,i],0])
    #                sendTask(['I',[12,i,13,i,14,i,15,i],0])
                    print (['I',[8,i-50,9,i-50],0])
                    sendTask(['I',[8,i-50,9,i-50],0.5])
                time.sleep(1)
                for i in range(18,-1,-1):
    #                print (['I',[12,i,13,i,14,i,15,i],0])
    #                sendTask(['I',[12,i,13,i,14,i,15,i],0])
                    print (['I',[8,i-50,9,i-50],0])
                    sendTask(['I',[8,i-50,9,i-50],0.5])
                time.sleep(1)
                
            closeSerialBehavior()
            logger.info("finish!")

    except Exception as e:
        logger.info("Exception")
        closeSerialBehavior()
        raise e
