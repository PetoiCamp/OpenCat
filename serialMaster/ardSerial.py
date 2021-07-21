#!/usr/bin/python
import serial
import struct
import sys
import time
import math
import numpy as np
import logging

FORMAT = '%(asctime)-15s %(name)s - %(levelname)s - %(message)s'
logging.basicConfig(level=logging.DEBUG, format=FORMAT)
logger = logging.getLogger(__name__)


def encode(in_str, encoding='utf-8'):
    if isinstance(in_str, bytes):
        return in_str
    else:
        return in_str.encode(encoding)


def get_serial(port='/dev/cu.wchusbserial1430'):
    return serial.Serial(
        port=port,
        baudrate=115200,
        parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_ONE,
        bytesize=serial.EIGHTBITS,
        timeout=1
    )


def open_serial(ser):
    t = 2
    if ser.is_open:
        ser.close()
    logger.info(f'serial port is opening, please wait for {t} secs')
    ser.open()
    time.sleep(t)


def close_serial(ser):
    if ser.is_open:
        ser.close()
    logger.info('closing serial port')
    ser.close()
    time.sleep(2)


def wrapper(task):  # task Structure is [token, var=[], time]
    logger.debug(f"{task}")
    if len(task) == 2:
        serialWriteByte([task[0]])
    elif isinstance(task[1][0], int):
        serialWriteNumToByte(task[0], task[1])
    else:
        serialWriteByte(task[1])
    time.sleep(task[-1])


def serialWriteNumToByte(token, var=None):  # Only to be used for c m u b i l o within Python
    # print("Num Token "); print(token);print(" var ");print(var);print("\n\n");
    logger.debug(f'serialWriteNumToByte, token={token}, var={var}')

    in_str = ""
    if var is None:
        var = []
    if token == 'l' or token == 'i':
        var = list(map(int, var))
        in_str = token.encode() + struct.pack('b' * len(var), *var) + '~'.encode()
    elif token == 'c' or token == 'm' or token == 'u' or token == 'b':
        in_str = token + str(var[0]) + " " + str(var[1]) + '\n'
    logger.debug(f"!!!! {in_str}")
    ser.write(encode(in_str))


def serialWriteByte(var=None):
    logger.debug(f'serial_write_byte, var={var}')
    if var is None:
        var = []
    token = var[0][0]
    # print var
    if (token == 'c' or token == 'm' or token == 'b' or token == 'u') and len(var) >= 2:
        in_str = ""
        for element in var:
            in_str = in_str + element + " "
    elif token == 'l' or token == 'i':
        if len(var[0]) > 1:
            var.insert(1, var[0][1:])
        var[1:] = list(map(int, var[1:]))
        in_str = token.encode() + struct.pack('b' * (len(var) - 1), *var[1:]) + '~'.encode()
    elif token == 'w' or token == 'k':
        in_str = var[0] + '\n'
    else:
        in_str = token
    logger.debug(f"!!!!!!! {in_str}")
    ser.write(encode(in_str))


# port = '/dev/cu.BittleSPP-3534C8-Port'
# port = '/dev/ttyS0',  # needed on RaspberryPi
port = '/dev/cu.wchusbserial1430'  # needed when using Mac USB

ser = get_serial(port)
open_serial(ser)

if __name__ == '__main__':
    counter = 0
    try:
        if len(sys.argv) >= 2:
            if len(sys.argv) == 2:
                wrapper([sys.argv[1], 0])
            else:
                logger.debug(f"{[sys.argv[1][0], sys.argv[1:], 0]}")
                wrapper([sys.argv[1][0], sys.argv[1:], 0])
        else:
            while True:
                for a in np.arange(0, 2 * math.pi, 0.2):
                    print(a)
                    serialWriteByte(["ksit"])
                    time.sleep(0.04)

        while True:
            time.sleep(0.01)
            counter = counter + 1
            if counter > 1000:
                break
            # logger.debug(f"number of chars:{str(ser.in_waiting)}")
            if ser.in_waiting > 0:
                x = ser.readline()
                if x != "":
                    print(x),

    except Exception as e:
        logger.info("exit")
        wrapper(["d"])
        close_serial(ser)
        print("exit")
        raise e

