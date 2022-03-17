#!/usr/bin/python3
# -*- coding: UTF-8 -*-

import sys
sys.path.append("..")
from ardSerial import *

if __name__ == '__main__':
    try:
        flushSeialOutput(300)

       
        time.sleep(3);
        wrapper(['z',1])
        wrapper(['g',1])
        time.sleep(1);
        wrapper(['kbalance', 2])
        time.sleep(1);
        token = 'i'
        while True:
            for i in range(-10,10):
                print (['i',[12,i,13,i,14,i,15,i],0])
                wrapper(['i',[12,i,13,i,14,i,15,i],0])
                while True:
                    response = ser.main_engine.readline().decode('ISO-8859-1')
                    if response != '':
                        print(response)
                    if response == token+'\r\n':
                        break
                    time.sleep(0.01)
            logger.info(f"Response is:\n {response}")
            for i in range(10,-10,-1):
                print (['i',[12,i,13,i,14,i,15,i],0])
                wrapper(['i',[12,i,13,i,14,i,15,i],0])
                while True:
                    response = ser.main_engine.readline().decode('ISO-8859-1')
                    if response != '':
                        print(response)
                    if response == token+'\r\n':
                        break
                    time.sleep(0.01)
            
        closeSerialBehavior()
        logger.info("finish!")

    except Exception as e:
        logger.info("Exception")
        closeSerialBehavior()
        raise e
