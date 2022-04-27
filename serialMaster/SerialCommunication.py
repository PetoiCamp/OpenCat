#!/usr/bin/python3
# -*- coding: UTF-8 -*-

# modified from https://blog.csdn.net/u013541325/article/details/113062191

import binascii
import serial  # need to install pyserial first
import serial.tools.list_ports

# global variables
# whether the serial port is created successfully or not
Ret = False
# list of serial port number
port_list_number = []
# list of serial port names
port_list_name = []

class Communication(object):
    """
    Python serial communication package class
    """
    # initialization
    def __init__(self, com, bps, timeout):
        self.port = com  # serial port number
        self.bps = bps  # baud rate
        self.timeout = timeout  # timeout
        self.main_engine = None  # global serial communication object
        global Ret
        Ret = False
        self.data = None
        self.b_c_text = None

        try:
            # open the serial port and get the serial port object
            self.main_engine = serial.Serial(self.port, self.bps, timeout=self.timeout)
            # determine whether the opening is successful
            if self.main_engine.is_open:
                Ret = True
                # print("Ret = ", Ret)
        except Exception as e:
            print("---Exception---：", e)


    def Print_Name(self):
        """
        Print the basic information of the device
        """
        print(self.main_engine.name)  # device name
        print(self.main_engine.port)  # read or write port
        print(self.main_engine.baudrate)  # baud rate
        print(self.main_engine.bytesize)  # byte size
        print(self.main_engine.parity)  # parity
        print(self.main_engine.stopbits)  # stop bits
        print(self.main_engine.timeout)  # read timeout setting
        print(self.main_engine.writeTimeout)  # write timeout setting
        print(self.main_engine.xonxoff)  # software flow control setting
        print(self.main_engine.rtscts)  # hardware (RTS/CTS) flow control setting
        print(self.main_engine.dsrdtr)  # hardware (DSR/DTR) flow control setting
        print(self.main_engine.interCharTimeout)  # character interval timeout


    def Open_Engine(self):
        """
        open serial port
        """
        global Ret
        # If the serial port is not open, open the serial port
        if not self.main_engine.is_open:
            self.main_engine.open()
            Ret = True


    def Close_Engine(self):
        """
        close serial port
        """
        global Ret
        # print(self.main_engine.is_open)  # check if the serial port is open
        # determine whether to open
        if self.main_engine.is_open:
            self.main_engine.close()  # close serial port
            Ret = False


    @staticmethod
    def Print_Used_Com():
        """
        print the list of available serial ports
        """
        port_list_name.clear()
        port_list_number.clear()

        port_list = list(serial.tools.list_ports.comports())

        if len(port_list) <= 0:
            print("The Serial port can't find!")
        else:
            for each_port in port_list:
                port_list_number.append(each_port[0])
                port_list_name.append(each_port[1])

#        print(port_list_number)
#        print(port_list_name)
        return port_list_number

    # Receive data of specified size
    # Read size bytes from the serial port.
    # If a timeout is set it may return less characters as requested.
    # With no timeout it will block until the requested number of bytes is read.
    def Read_Size(self, size):
        """
        Receive data of specified size
        :param size:
        :return:
        """
        return self.main_engine.read(size=size)

    # Receive a line of data
    # When using readline(), you should pay attention:
    # you should specify a timeout when opening the serial port,
    # otherwise, if the serial port does not receive a new line,
    # it will wait forever.
    # If there is no timeout, readline() will report an exception.
    def Read_Line(self):
        """
        Receive a line of data
        :return:
        """
        return self.main_engine.readline()


    def Send_data(self, data):
        """
        send data
        :param data:
        """
        self.main_engine.write(data)

    # more examples
    # self.main_engine.write(bytes(listData))  # send list data listData = [0x01, 0x02, 0xFD] or listData = [1, 2, 253]
    # self.main_engine.write(chr(0x06).encode("utf-8"))  # send a data in hexadecimal
    # print(self.main_engine.read().hex())  # read a byte in hexadecimal
    # print(self.main_engine.read())  # read a byte
    # print(self.main_engine.read(10).decode("gbk"))  # read 10 bytes
    # print(self.main_engine.readline().decode("gbk"))  # read a line
    # print(self.main_engine.readlines())  # read multiple lines, return the list, must match the timeout (timeout) use
    # print(self.main_engine.in_waiting)  # get the remaining bytes of the input buffer
    # print(self.main_engine.out_waiting)  # Get the remaining bytes of the output buffer
    # print(self.main_engine.readall())# read all characters

    # receive data
    # an integer data occupies two bytes
    # a character occupies one byte
    def Receive_data(self, way):
        """
        receive data
        :param way:
        """
        # Receiving data cyclically, this is an endless loop,
        # which can be implemented by threads
        print("Start receiving data：")
        bWaitRec = True
        while bWaitRec:
            try:
                # byte-by-byte reception
                if self.main_engine.in_waiting:
                    if way == 0:
                        for i in range(self.main_engine.in_waiting):
                            print("receive ascii data：" + str(self.Read_Size(1)))
                            # data1 = self.Read_Size(1).hex()  # convert to hexadecimal
                            # data2 = int(data1, 16)  # convert to decimal
                            # print("received data hexadecimal："+data1+"  received data decimal："+str(data2))
                    if way == 1:
                        # overall reception
                        self.data = self.main_engine.read(self.main_engine.in_waiting).decode("utf-8")  # method 1
                        #self.data = self.main_engine.read_all()  # method 2
                        print("receive ascii data：", self.data)

                        '''
                        Hex Bytes to Bytes，e.g. b'\x01\x06\xaa\x92\x12' convert to b'0106aa9212'
                        '''
                        # self.b_c_text = binascii.hexlify(self.data)
                        # print("Bytes data：", self.b_c_text)

                        # if data is empty, exit the receiving loop
                        if self.data.strip() == '':
                            bWaitRec = False
            except Exception as e:
                print("Exception report：", e)

        print("Data received！")


# if __name__ == '__main__':
#     Communication.Print_Used_Com()
#     port = port_list_number
#     myCom = Communication(port[0], 115200, 0.5)
#     print("Ret = ", Ret)
#     myCom.Open_Engine()
#     myCom.Receive_data(1)
#     myCom.Close_Engine()
