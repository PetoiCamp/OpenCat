#!/usr/bin/python3
# -*- coding: UTF-8 -*-

import sys
sys.path.append("..")
from ardSerial import *

if __name__ == '__main__':
    try:
#        flushSeialOutput(300)
        time.sleep(3);
        wrapper(['g',1])
        time.sleep(1);
        wrapper(['kbalance', 2])
        time.sleep(1);
        token = 'i'
        while True:
            for i in range(-10,10):
                print (['I',[12,i,13,i,14,i,15,i],0])
                wrapper(['I',[12,i,13,i,14,i,15,i],0])
                printSerialMessage(token)
                time.sleep(0.02)
            for i in range(10,-10,-1):
                print (['I',[12,i,13,i,14,i,15,i],0])
                wrapper(['I',[12,i,13,i,14,i,15,i],0])
                printSerialMessage(token)
                time.sleep(0.02)
            
        closeSerialBehavior()
        logger.info("finish!")

    except Exception as e:
        logger.info("Exception")
        closeSerialBehavior()
        raise e
