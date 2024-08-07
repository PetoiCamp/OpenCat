#!/usr/bin/python3
# -*- coding: UTF-8 -*-

# !/usr/bin/python3

# Jason Wong
# Petoi LLC
# May.1st, 2022

from commonVar import *
# from subprocess import check_call
import subprocess
import threading
from tkinter import ttk
from tkinter import filedialog
import pathlib
import webbrowser

regularW = 14
language = languageList['English']
NyBoard_version_list = ['NyBoard_V1_0', 'NyBoard_V1_1', 'NyBoard_V1_2']
BiBoard_version_list = ['BiBoard_V0_1', 'BiBoard_V0_2', 'BiBoard_V1_0']
# Hunter_Board_list = ['BiBoard_V1_0']

def txt(key):
    return language.get(key, textEN[key])
    
class Uploader:
    def __init__(self,model,lan):
        connectPort(goodPorts, needTesting=False, needSendTask=False, needOpenPort=False)
        # closeAllSerial(goodPorts, clearPorts=False)
        self.configName = model
        self.win = Tk()
        self.OSname = self.win.call('tk', 'windowingsystem')
        self.shellOption = True
        self.win.geometry('+260+100')
        if self.OSname == 'aqua':
            self.backgroundColor ='gray'
        else:
            self.backgroundColor = None

        if self.OSname == 'win32':
            self.shellOption = False
        self.win.resizable(False, False)
        self.bParaUpload = True
        self.bFacReset = False
        self.bIMUerror = False
        Grid.rowconfigure(self.win, 0, weight=1)
        Grid.columnconfigure(self.win, 0, weight=1)
        self.strProduct = StringVar()
        global language
        language = lan
        # self.BittleNyBoardModes = list(map(lambda x: txt(x),['Standard', 'Mind+', 'RandomMind', 'Voice', 'Camera','Ultrasonic', 'RandomMind_Ultrasonic', 'PIR', 'Touch', 'Light', 'Gesture', 'InfraredDistance']))
        # self.NybbleNyBoardModes = list(map(lambda x: txt(x),['Standard', 'Mind+', 'RandomMind', 'Voice', 'Camera','Ultrasonic', 'RandomMind_Ultrasonic', 'PIR', 'Touch', 'Light', 'Gesture', 'InfraredDistance']))
        # for NyBoard, the mode is the same between Bittle and Nybble now
        self.NyBoardModes = list(map(lambda x: txt(x),
                                           ['Standard', 'Mind+', 'RandomMind', 'Voice', 'Camera', 'Ultrasonic',
                                            'RandomMind_Ultrasonic', 'PIR', 'Touch', 'Light', 'Gesture',
                                            'InfraredDistance']))
        # self.BittleBiBoardModes = list(map(lambda x: txt(x), ['Standard', 'Camera','Ultrasonic','PIR','Touch','Light','Gesture']))
        # self.NybbleBiBoardModes = list(map(lambda x: txt(x), ['Standard']))
        # for BiBoard, the mode is the same between Bittle and Nybble now
        self.BiBoardModes = list(map(lambda x: txt(x), ['Standard']))
        self.inv_txt = {v: k for k, v in language.items()}
        self.initWidgets()
        if self.strProduct.get() == 'Bittle X' or self.strProduct.get() == 'Bittle R':
            board_version_list = BiBoard_version_list
        # elif self.strProduct.get() == 'Hunter':
        #     board_version_list = Hunter_Board_list
        else:
            board_version_list = NyBoard_version_list + BiBoard_version_list
        self.cbBoardVersion['values'] = board_version_list
        self.updateMode()
        self.setActiveOption()

        self.win.protocol('WM_DELETE_WINDOW', self.on_closing)
        self.win.update()

        self.keepChecking = True
        t = threading.Thread(target=keepCheckingPort,
                             args=(goodPorts, lambda: self.keepChecking, False, self.updatePortlist))
        t.daemon = True
        t.start()

        self.win.focus_force()    # force the main interface to get focus
        self.win.mainloop()


    def buildMenu(self):
        self.menuBar = Menu(self.win)
        self.win.configure(menu=self.menuBar)
        
        if self.OSname == 'win32':
            self.win.iconbitmap(resourcePath + 'Petoi.ico')
        
        self.helpMenu = Menu(self.menuBar, tearoff=0)
        self.helpMenu.add_command(label=txt('labAbout'), command=self.about)
        self.menuBar.add_cascade(label=txt('labHelp'), menu=self.helpMenu)

    def initWidgets(self):
        self.win.title(txt('uploaderTitle'))
        self.buildMenu()
        self.strFileDir = StringVar()
        self.strPort = StringVar()
        self.strStatus = StringVar()
        self.strSoftwareVersion = StringVar()
        self.strBoardVersion = StringVar()
        
        self.intMode = IntVar()
        self.strMode = StringVar()

        lines = []
        try:
            with open(defaultConfPath, "r", encoding="utf-8") as f:
                lines = f.readlines()
                # f.close()
            lines = [line.split('\n')[0] for line in lines]    # remove the '\n' at the end of each line
            self.defaultLan = lines[0]
            self.configName = lines[1]
            strDefaultPath = lines[2]
            strSwVersion = lines[3]
            strBdVersion = lines[4]
            mode = lines[5]
            if len(lines) >= 8:
                strCreator = lines[6]
                strLocation = lines[7]
                self.configuration = [self.defaultLan, self.configName, strDefaultPath, strSwVersion, strBdVersion,
                                      mode, strCreator, strLocation]
            else:
                self.configuration = [self.defaultLan, self.configName, strDefaultPath, strSwVersion, strBdVersion, mode]

                
        except Exception as e:
            print ('Create configuration file')
            self.defaultLan = 'English'
            strDefaultPath = releasePath[:-1]
            strSwVersion = '2.0'
            strBdVersion = NyBoard_version_list[-1]
            mode = 'Standard'
            self.configuration = [self.defaultLan, self.configName, strDefaultPath, strSwVersion, strBdVersion, mode]
            
        num = len(lines)
        logger.debug(f"len(lines): {num}")
        self.lastSetting = [self.configName,strDefaultPath,strSwVersion,strBdVersion,mode]
        self.currentSetting = []
        
        logger.info(f"The firmware file folder is {strDefaultPath}")
        self.strFileDir.set(strDefaultPath)

        fmFileDir = ttk.Frame(self.win)
        fmFileDir.grid(row=0, columnspan=3, ipadx=2, padx=2, sticky=W + E + N + S)

        self.labFileDir = Label(fmFileDir, text=txt('labFileDir'), font=('Arial', 16))
        self.labFileDir.grid(row=0, column=0, ipadx=2, padx=2, sticky=W)

        self.btnFileDir = Button(fmFileDir, text=txt('btnFileDir'), font=('Arial', 12), foreground='blue',
                                        background=self.backgroundColor, command=self.open_dir)  # bind open_dir function
        self.btnFileDir.grid(row=0, column=1, ipadx=5, padx=5, pady=5, sticky=E)

        self.entry = Entry(fmFileDir, textvariable=self.strFileDir, font=('Arial', 16), foreground='green', background='white')
        self.entry.grid(row=1, columnspan=2, ipadx=5, padx=5, sticky=E + W)
        
        fmFileDir.columnconfigure(0, weight=8)  # set column width
        fmFileDir.columnconfigure(1, weight=1)  # set column width
        fmFileDir.rowconfigure(1, weight=1)

        fmProduct = ttk.Frame(self.win)
        fmProduct.grid(row=1, column=0, ipadx=2, padx=2, sticky=W)
        self.labProduct = ttk.Label(fmProduct, text=txt('labProduct'), font=('Arial', 16))
        self.labProduct.grid(row=0, column=0, ipadx=5, padx=5, sticky=W)

        cbProduct = ttk.Combobox(fmProduct, textvariable=self.strProduct, foreground='blue', font=12)
        # list of product
        cbProductList = ['Nybble', 'Bittle', 'Bittle X', 'Bittle R']
        # set default value of Combobox
        cbProduct.set(self.lastSetting[0])
        # set list for Combobox
        cbProduct['values'] = cbProductList
        cbProduct.grid(row=1, ipadx=5, padx=5, sticky=W)
        cbProduct.bind("<<ComboboxSelected>>", self.chooseProduct)

        fmSoftwareVersion = ttk.Frame(self.win)
        fmSoftwareVersion.grid(row=1, column=1, ipadx=2, padx=2, sticky=W)
        self.labSoftwareVersion = ttk.Label(fmSoftwareVersion, text=txt('labSoftwareVersion'), font=('Arial', 16))
        self.labSoftwareVersion.grid(row=0, ipadx=5, padx=5, sticky=W)
        self.cbSoftwareVersion = ttk.Combobox(fmSoftwareVersion, textvariable=self.strSoftwareVersion, foreground='blue', font=12)
        self.cbSoftwareVersion.bind("<<ComboboxSelected>>",self.chooseSoftwareVersion)

        # list of software_version
        software_version_list = ['1.0', '2.0']
        # set default value of Combobox
        self.cbSoftwareVersion.set(self.lastSetting[2])
        
        # set list for Combobox
        self.cbSoftwareVersion['values'] = software_version_list
        self.cbSoftwareVersion.grid(row=1, ipadx=5, padx=5, sticky=W)

        fmBoardVersion = ttk.Frame(self.win)
        fmBoardVersion.grid(row=1, column=2, ipadx=2, padx=2, sticky=W)
        self.labBoardVersion = ttk.Label(fmBoardVersion, text=txt('labBoardVersion'), font=('Arial', 16))
        self.labBoardVersion.grid(row=0, ipadx=5, padx=5, sticky=W)
        
        self.cbBoardVersion = ttk.Combobox(fmBoardVersion, textvariable=self.strBoardVersion, foreground='blue', font=12)
        self.cbBoardVersion.bind("<<ComboboxSelected>>", self.chooseBoardVersion)
        # list of board_version
        board_version_list = NyBoard_version_list + BiBoard_version_list
        # set default value of Combobox
        self.cbBoardVersion.set(self.lastSetting[3])
        # set list for Combobox
        if self.strProduct.get() == 'Bittle X':
            if self.lastSetting[3] in NyBoard_version_list:
                self.cbBoardVersion.set(BiBoard_version_list[0])
            board_version_list = BiBoard_version_list
        elif self.strProduct.get() == 'Bittle R':
            if self.lastSetting[3] in NyBoard_version_list:
                self.cbBoardVersion.set(BiBoard_version_list[2])
            board_version_list = BiBoard_version_list
        else:
            board_version_list = NyBoard_version_list + BiBoard_version_list
        self.cbBoardVersion['values'] = board_version_list
        self.cbBoardVersion.grid(row=1, ipadx=5, padx=5, sticky=W)

        fmMode = ttk.Frame(self.win)
        fmMode.grid(row=2, column=0, ipadx=2, padx=2, pady=6,sticky=W)
        self.labMode = ttk.Label(fmMode, text=txt('labMode'), font=('Arial', 16))
        self.labMode.grid(row=0, column=0, ipadx=5, padx=5, sticky=W)

        if self.strProduct.get() == 'Bittle' or self.strProduct.get() == 'Nybble':
            if 'NyBoard' in self.strBoardVersion.get():
                cbModeList = self.NyBoardModes
            else:
                cbModeList = self.BiBoardModes
        elif self.strProduct.get() == 'Bittle X' or self.strProduct.get() == 'Bittle R':
            cbModeList = self.BiBoardModes

        self.cbMode = ttk.Combobox(fmMode, textvariable=self.strMode, foreground='blue', font=12)
        # set default value of Combobox
        self.cbMode.set(txt(self.lastSetting[4]))
        # set list for Combobox
        self.cbMode['values'] = cbModeList
        self.cbMode.grid(row=1, ipadx=5, padx=5, sticky=W)

        fmSerial = ttk.Frame(self.win)    # relief=GROOVE
        fmSerial.grid(row=2, column=1, ipadx=2, padx=2, pady=6, sticky=W)
        self.labPort = ttk.Label(fmSerial, text=txt('labPort'), font=('Arial', 16))
        self.labPort.grid(row=0, ipadx=5, padx=5, sticky=W)
        self.cbPort = ttk.Combobox(fmSerial, textvariable=self.strPort, foreground='blue', font=12)    # width=38,
        # list of serial port number
        port_number_list = []
        if len(portStrList) == 0:
            port_number_list = [' ']
            print("Cannot find the serial port!")
        else:
            logger.info(f"portStrList is {portStrList}")
            for p in portStrList:
                portName = p
                logger.debug(f"portName is {portName}")
                port_number_list.append(portName)
            logger.debug(f"port_number_list is {port_number_list}")
            self.cbPort.set(port_number_list[0])
        # set list for Combobox
        self.cbPort['values'] = port_number_list
        self.cbPort.grid(row=1, ipadx=5, padx=5, sticky=W)

        fmFacReset = ttk.Frame(self.win)    # relief=GROOVE
        fmFacReset.grid(row=2, column=2, ipadx=2, padx=2, pady=6, sticky=W + E)
        self.btnFacReset = Button(fmFacReset, text=txt('btnFacReset'), font=('Arial', 16, 'bold'), fg='red',
                                  relief='groove', command=self.factoryReset)
        self.btnFacReset.grid(row=0, ipadx=5, ipady=5, padx=9, pady=8, sticky=W + E + N + S)
        tip(self.btnFacReset, txt('tipFacReset'))
        fmFacReset.columnconfigure(0, weight=1)
        fmFacReset.rowconfigure(0, weight=1)

        fmUpload = ttk.Frame(self.win)
        fmUpload.grid(row=3, columnspan=3, ipadx=2, padx=2, pady=8, sticky=W + E + N + S)
        self.btnUpgrade = Button(fmUpload, text=txt('btnUpgrade'), font=('Arial', 16, 'bold'), foreground='blue',
                                background=self.backgroundColor, relief='groove', command=self.upgrade)
        self.btnUpgrade.grid(row=0, column=0, ipadx=5, padx=5, pady=5, sticky=W + E)
        tip(self.btnUpgrade, txt('tipUpgrade'))
        self.btnUpdateMode = Button(fmUpload, text=txt('btnUpdateMode'), font=('Arial', 16, 'bold'), foreground='blue',
                                       background=self.backgroundColor, relief='groove', command=self.uploadeModeOnly)
        self.btnUpdateMode.grid(row=0, column=1, ipadx=5, padx=5, pady=5, sticky=W + E)
        tip(self.btnUpdateMode, txt('tipUpdateMode'))
        fmUpload.columnconfigure(0, weight=1)
        fmUpload.columnconfigure(1, weight=1)
        fmUpload.rowconfigure(0, weight=1)

        fmStatus = ttk.Frame(self.win)
        fmStatus.grid(row=4, columnspan=3, ipadx=2, padx=2, pady=5, sticky=W + E + N + S)
        self.statusBar = ttk.Label(fmStatus, textvariable=self.strStatus, font=('Arial', 16), relief=SUNKEN)
        self.statusBar.grid(row=0, ipadx=5, padx=5, sticky=W + E + N + S)
        fmStatus.columnconfigure(0, weight=1)

    def uploadeModeOnly(self):
        self.bParaUpload = False
        self.bFacReset = False
        self.autoupload()

    def factoryReset(self):
        self.bParaUpload = True
        self.bFacReset = True
        self.autoupload()

    def upgrade(self):
        self.bParaUpload = True
        self.bFacReset = False
        self.autoupload()

    def updatePortlist(self):
        port_number_list = []
        if len(portStrList) == 0:
            port_number_list = [' ']
            print("Cannot find the serial port!")
        else:
            logger.info(f"portStrList is {portStrList}")
            for p in portStrList:
                portName = p
                logger.debug(f"{portName}")
                port_number_list.append(portName)
            logger.debug(f"port_number_list is {port_number_list}")
        self.cbPort.set(port_number_list[0])
        # set list for Combobox
        self.cbPort['values'] = port_number_list

    def about(self):
        self.msgbox = messagebox.showinfo(txt('titleVersion'), txt('msgVersion'))
        self.force_focus()

    def setActiveMode(self):
        if self.strSoftwareVersion.get() == '1.0':
            stt = DISABLED
            self.strMode.set(txt('Standard'))
            board_version_list = NyBoard_version_list
            self.strBoardVersion.set(board_version_list[-1])
        else:
            stt = NORMAL
            board_version_list = NyBoard_version_list + BiBoard_version_list
        # set list for Combobox
        self.cbBoardVersion['values'] = board_version_list
        self.cbMode.config(state = stt)
    
    def chooseSoftwareVersion(self, event):
        self.setActiveMode()

    def setActiveOption(self):
        if self.cbBoardVersion.get() in BiBoard_version_list:
            stt = DISABLED
            self.strSoftwareVersion.set('2.0')
        else:
            stt = NORMAL

        self.cbSoftwareVersion.config(state=stt)

    def chooseBoardVersion(self, event):
        self.setActiveOption()
        self.updateMode()

    def chooseProduct(self, event):
        if self.strProduct.get() == 'Bittle X' or self.strProduct.get() == 'Bittle R':
            self.strBoardVersion.set(BiBoard_version_list[0])
            board_version_list = BiBoard_version_list
        # elif self.strProduct.get() == 'Bittle R':
        #     self.cbBoardVersion.set(Hunter_Board_list[0])
        #     board_version_list = Hunter_Board_list
        else:
            board_version_list = NyBoard_version_list + BiBoard_version_list
        self.cbBoardVersion['values'] = board_version_list
        self.updateMode()
        self.setActiveOption()

    def updateMode(self):
        if self.strProduct.get() == 'Bittle' or self.strProduct.get() == 'Nybble':
            if 'NyBoard' in self.strBoardVersion.get():
                modeList = self.NyBoardModes
            else:
                modeList = self.BiBoardModes
        elif self.strProduct.get() == 'Bittle X'or self.strProduct.get() == 'Bittle R':
            modeList = self.BiBoardModes

        self.cbMode['values'] = modeList

        if self.strMode.get() not in modeList:
            messagebox.showwarning(txt('Warning'),txt('msgMode'))
            # printH("modeList[0]:", modeList[0])
            self.strMode.set(modeList[0])
            self.force_focus()  # force the main interface to get focus

    def formalize(self, strdir=' '):
        sep = "/"
        listDir = strdir.split("/")
        if (strdir == str(pathlib.Path().resolve())):
            strdir = sep.join(listDir) + sep + "release"
        else:
            if (listDir[-1].find("release") == -1) and len(listDir) >= 2:
                while listDir[-1].find("release") == -1 and len(listDir) >= 2:
                    listDir = listDir[:-1]
                if listDir[-1] != "release":
                    strdir = " "
                else:
                    strdir = sep.join(listDir)
        self.strFileDir.set(strdir)


    def open_dir(self):
        # call askdirectory to open file director
        logger.debug(f"{self.strFileDir.get()}")
        if (self.strFileDir.get()).find(releasePath[:-1]) != -1:
            initDir = releasePath[:-1]
        else:
            initDir = self.strFileDir
        dirpath = filedialog.askdirectory(title=txt('titleFileDir'), initialdir=initDir)

        if dirpath:
            self.formalize(dirpath)
        self.force_focus()

    def encode(self, in_str, encoding='utf-8'):
        if isinstance(in_str, bytes):
            return in_str
        else:
            return in_str.encode(encoding)

    def WriteInstinctPrompts(self, port):
        serObj = Communication(port, 115200, 0.5)
        logger.info(f"Connect to usb serial port: {port}.")
        strSoftwareVersion = self.strSoftwareVersion.get()
        promptJointCalib = {
            'message':txt('reset joints?'),
            'operating':txt('reseting joints'),
            'result':txt('joints reset'),
        }
        promptInstinct = {
            'message':txt('update instincts?'),
            'operating':txt('updating instincts'),
            'result':txt('instincts updated')
        }
        promptIMU = {
            'message':txt('calibrate IMU?'),
            'operating':txt('calibrating IMU'),
            'result':txt('IMU calibrated')
        }
        if strSoftwareVersion == '1.0':
            promptList = [promptJointCalib,promptInstinct,promptIMU]
        elif strSoftwareVersion == '2.0':
            promptList = [promptJointCalib,promptIMU]

        strBoardVersion = self.strBoardVersion.get()
        
        progress = 0
        bCount = False
        bResetMode = False
        retMsg = False
        self.bIMUerror = False
        prompStr = ""
        counterIMU = 0
        while True:
            time.sleep(0.01)
            if serObj.main_engine.in_waiting > 0:
                x = str(serObj.main_engine.readline())
                prompStr = x[2:-1]
                logger.debug(f"new line:{x}")
                if x != "":
                    print(prompStr)
                    logger.info(prompStr)
                    questionMark = "Y/n"
                    if self.bFacReset and strBoardVersion in BiBoard_version_list:    # for BiBoard Factory reset
                        newBoardMark = "Set up the new board"
                        if prompStr.find(newBoardMark) != -1:
                            bResetMode = True

                        if not bResetMode and (prompStr.find("Ready!") != -1):
                            time.sleep(1)
                            serObj.Send_data(self.encode("!"))
                            continue

                        if bResetMode:
                            if prompStr.find(questionMark) != -1:
                                if progress > 0 and retMsg:
                                    self.strStatus.set(promptList[progress-1]['result'])
                                    self.statusBar.update()
                                if prompStr.find("joint") != -1:
                                    prompt = promptJointCalib
                                elif prompStr.find("Instinct") != -1:
                                    prompt = promptInstinct
                                elif prompStr.find("Calibrate") != -1:
                                    prompt = promptIMU
                                elif prompStr.find("assurance") != -1:
                                    serObj.Send_data(self.encode("n"))
                                    continue
                                retMsg = messagebox.askyesno(txt('Warning'), prompt['message'])
                                if retMsg:
                                    self.strStatus.set(prompt['operating'])
                                    self.statusBar.update()
                                    serObj.Send_data(self.encode("Y"))
                                else:
                                    serObj.Send_data(self.encode("n"))
                                progress += 1
                            if prompStr.find("Ready!") != -1:
                                break
                    else:
                        if prompStr.find(questionMark) != -1:
                            if self.bParaUpload and strBoardVersion in NyBoard_version_list:    # for NyBoard upgrade firmware
                                if progress > 0 and retMsg:
                                    self.strStatus.set(promptList[progress-1]['result'])
                                    self.statusBar.update()
                                if prompStr.find("joint") != -1:
                                    prompt = promptJointCalib
                                elif prompStr.find("Instinct") != -1:
                                    prompt = promptInstinct
                                elif prompStr.find("Calibrate") != -1:
                                    prompt = promptIMU
                                elif prompStr.find("assurance") != -1:
                                    serObj.Send_data(self.encode("n"))
                                    continue
                                retMsg = messagebox.askyesno(txt('Warning'), prompt['message'])
                                if retMsg:
                                    self.strStatus.set(prompt['operating'])
                                    self.statusBar.update()
                                    serObj.Send_data(self.encode("Y"))
                                else:
                                    serObj.Send_data(self.encode("n"))
                                progress += 1
                            else:    # for BiBoard upgrade firmware
                                if prompStr.find("joint") != -1:
                                    prompt = promptJointCalib
                                    serObj.Send_data(self.encode("n"))
                                elif prompStr.find("Instinct") != -1:
                                    prompt = promptInstinct
                                    serObj.Send_data(self.encode("n"))
                                elif prompStr.find("Calibrate") != -1:
                                    prompt = promptIMU
                                    serObj.Send_data(self.encode("n"))
                                elif prompStr.find("assurance") != -1:
                                    serObj.Send_data(self.encode("n"))
                                    continue
                        elif prompStr.find(questionMark) == -1 and self.bParaUpload:
                            if prompStr[:3] == "IMU":
                                if progress > 0 and retMsg:
                                    self.strStatus.set(promptList[progress - 1]['result'])
                                    self.statusBar.update()
                                counterIMU += 1
                                if counterIMU == 3:
                                    self.bIMUerror = True
                                    self.strStatus.set(txt('caliIMUerrorStatus'))
                                    self.statusBar.update()
                                    break

                        if prompStr.find("sent to mpu.setXAccelOffset") != -1 or prompStr.find("Ready!") != -1:
                            if strBoardVersion in NyBoard_version_list:
                                if retMsg:
                                    self.strStatus.set(promptList[progress - 1]['result'])
                                    self.statusBar.update()
                                if strSoftwareVersion == '2.0':
                                    continue
                                else:
                                    break
                            else:
                                break
                        elif prompStr.find("Calibrated:") != -1:
                            self.strStatus.set(txt('9685 Calibrated'))
                            self.statusBar.update()
                            break
            else:
                if self.bFacReset:    # for NyBoard Factory reset
                    if strBoardVersion in NyBoard_version_list:
                        if prompStr.find("Optional: Connect PWM 3") != -1 and (not bCount):
                            bCount = True
                            start = time.time()
                            logger.info(f"start timer")
                        if bCount and (time.time() - start > 5):
                            break
                else:    # for NyBoard upgrade firmware
                    if (strBoardVersion in NyBoard_version_list) and (prompStr.find("Optional: Connect PWM 3") != -1):
                        break

        serObj.Close_Engine()
        logger.info("close the serial port.")
        self.force_focus()

        if self.bIMUerror and strBoardVersion in NyBoard_version_list:
            messagebox.showwarning(txt('Warning'), message=txt('caliIMUerrorMessage'))
            return

        if not self.bFacReset and strBoardVersion in NyBoard_version_list:
            messagebox.showinfo(title=None, message=txt('parameterFinish'))


    def saveConfigToFile(self,filename):
        if len(self.configuration) == 6:
            self.configuration = [self.defaultLan, self.lastSetting[0], self.lastSetting[1], self.lastSetting[2],
                                  self.lastSetting[3], self.lastSetting[4]]
        else:
            self.configuration = [self.defaultLan, self.lastSetting[0], self.lastSetting[1], self.lastSetting[2],
                                  self.lastSetting[3], self.lastSetting[4], self.configuration[6],self.configuration[7]]

        with open(filename, "w", encoding="utf-8") as f:
            lines = '\n'.join(self.configuration)+'\n'
            f.writelines(lines)
            # f.close()


    def showMessage(self,sta):
        self.strStatus.set(sta)
        self.statusBar.update()
        messagebox.showinfo('Petoi Desktop App', txt('checkLogfile'))

        if self.OSname == 'aqua':    # for macOS
            folder_path = "file:///Applications/Petoi Desktop App.app/Contents/Resources"
            # folder_path = "file:////./"  # Replace with the actual folder path
        else:    # for Windows or Linux
            path = os.getcwd()
            folder_path = "file://" + path  # Replace with the actual folder path
            # os.startfile(path)
        print(folder_path)
        # Open the folder in the default file browser
        webbrowser.open_new_tab(folder_path)


    def autoupload(self):
        file = open('./logfile.log', 'r+')
        lines = file.readlines()
        # Read the first three lines
        first_three_lines = lines[:3]
        file.close()

        for line in lines:
            line = line.strip()  # remove the line break from each line
            logger.debug(f"{line}")
            if (".ino.hex" in line) or \
                    (".ino.bin" in line):
                with open("./logfile.log", "w+", encoding="utf-8") as logfile:
                    for line in first_three_lines:
                        logfile.write(line)
                break
        logger.info(f"lastSetting: {self.lastSetting}.")
        strProd = self.strProduct.get()
        strDefaultPath = self.strFileDir.get()
        strSoftwareVersion = self.strSoftwareVersion.get()
        strBoardVersion = self.strBoardVersion.get()
        strMode = self.inv_txt[self.strMode.get()]
        self.currentSetting = [strProd, strDefaultPath, strSoftwareVersion, strBoardVersion, strMode]
        logger.info(f"currentSetting: {self.currentSetting}.")

        if self.strFileDir.get() == '' or self.strFileDir.get() == ' ':
            messagebox.showwarning(txt('Warning'), txt('msgFileDir'))
            self.force_focus()  # force the main interface to get focus
            return False

        # NyBoard_V1_X software version are all the same
        if "NyBoard_V1" in strBoardVersion:
            pathBoardVersion = "NyBoard_V1"
        else:
            pathBoardVersion = strBoardVersion

        if strProd == "Bittle X":
            strProdPath = "Bittle"
        elif strProd == "Bittle R":
            strProdPath = "BittleR"
        else:
            strProdPath = strProd
        path = self.strFileDir.get() + '/' + strSoftwareVersion + '/' + strProdPath + '/' + pathBoardVersion + '/'

        if self.OSname == 'x11' or self.OSname == 'aqua':
            port = '/dev/' + self.strPort.get()
        else:
            port = self.strPort.get()
        logger.info(f"{self.strPort.get()}")
        if port == ' ' or port == '':
            messagebox.showwarning(txt('Warning'), txt('msgPort'))
            self.force_focus()
            return False

        if strBoardVersion in NyBoard_version_list:
            if self.bFacReset:
                fnWriteI = path + 'WriteInstinctAutoInit.ino.hex'
                fnOpenCat = path + 'OpenCatStandard.ino.hex'
                self.currentSetting[4] = 'Standard'
            else:
                fnWriteI = path + 'WriteInstinct.ino.hex'
                fnOpenCat = path + 'OpenCat' + strMode + '.ino.hex'
            filename = [fnWriteI, fnOpenCat]
            logger.info(f"{filename}")
            uploadStage = ['Parameters', 'Main function']
            for s in range(len(uploadStage)):
                # if s == 0 and self.bParaUploaded and self.currentSetting[:4] == self.lastSetting[:4]:
                # for NyBoard uplod mode only
                if s == 0 and (not self.bParaUpload):
                    continue               # no need upload configuration firmware
                # if calibrate IMU failed
                elif s == 1 and self.bIMUerror:
                    continue               # no need upload main function firmware

                self.strStatus.set(txt('Uploading') + txt(uploadStage[s]) + '...' )
                self.win.update()
                # self.inProgress = True
                # status = txt('Uploading') + txt(uploadStage[s]) + '.'
                # t = threading.Thread(target=self.progressiveDots, args=(status,))
                # t.start()
                if self.OSname == 'win32':
                    avrdudePath = resourcePath + 'avrdudeWin/'
                elif self.OSname == 'x11':     # Linux
                    avrdudePath = '/usr/bin/'
                    path = pathlib.Path(avrdudePath + 'avrdude')
                    if not path.exists():
                        messagebox.showwarning(txt('Warning'), txt('msgNoneAvrdude'))
                        self.force_focus()  # force the main interface to get focus
                        return False
                    # avrdudeconfPath = '/etc/avrdude/'      # Fedora / CentOS
                    avrdudeconfPath = '/etc/'            # Debian / Ubuntu
                else:
                    avrdudePath = resourcePath + 'avrdudeMac/'

                try:
                    # for NyBoard factory reset or upgrade firmware
                    if s == 0 and self.bIMUerror:    # alread upload configuration firmware,but calibrate IMU failed
                        pass                         # no need upload configuration firmware again
                    else:
                        if self.OSname == 'x11':     # Linuxself.OSname == 'x11':     # Linux
                            # check_call(avrdudePath + 'avrdude -C' + avrdudeconfPath + 'avrdude.conf -v -V -patmega328p -carduino -P%s -b115200 -D -Uflash:w:%s:i' % \
                            #         (port, filename[s]), shell=self.shellOption)
                            cmd = avrdudePath + 'avrdude -C' + avrdudeconfPath + 'avrdude.conf -v -V -patmega328p -carduino -P' + port + ' -b115200 -D -Uflash:w:' + \
                                  filename[s] + ':i'
                        else:
                            # check_call(avrdudePath + 'avrdude -C' + avrdudePath + 'avrdude.conf -v -V -patmega328p -carduino -P%s -b115200 -D -Uflash:w:%s:i > ./avrdude_log.txt 2> ./avrdude_errors.txt' % \
                            #         (port, filename[s]), shell=self.shellOption)
                            cmd = avrdudePath + 'avrdude -C' + avrdudePath + 'avrdude.conf -v -V -patmega328p -carduino -P' + port + ' -b115200 -D -Uflash:w:' + \
                                  filename[s] + ':i'

                        # Run the program and capture output
                        process = subprocess.Popen(cmd,shell = self.shellOption, stdout = subprocess.PIPE, stderr = subprocess.STDOUT)
                        output, error = process.communicate()  # Wait for the program to finish
                        # printH("error:", error)
                        # printH("output:", output)

                        # Check for errors (optional)
                        if error:
                            logger.info(f"Error running program: {error}")
                        else:
                            # Write captured output to a file
                            with open("./logfile.log", "a+", encoding="utf-8") as logfile:
                                logfile.write(output.decode())  # Decode bytes to string
                                # time.sleep(5)
                                # lines = logfile.readlines()
                            # print(lines)
                            file = open('./logfile.log', 'r+')
                            lines = file.readlines()
                            file.close()

                            for line in lines:
                                line = line.strip()  # remove the line break from each line
                                logger.debug(f"{line}")
                                if ("programmer is not responding" in line) or \
                                    ("can\'t open device" in line) or \
                                    ("attempt" in line) or \
                                    ("error" in line) or ("Errno" in line):
                                    status = txt(uploadStage[s]) + txt('failed to upload')
                                    self.showMessage(status)
                                    return False

                # self.inProgress = False
                except:
                    status = txt(uploadStage[s]) + txt('failed to upload')
                    self.strStatus.set(status)
                    self.statusBar.update()
                    messagebox.showwarning(txt('Warning'), txt('Replug prompt'))
                    return False
                else:
                    status = txt(uploadStage[s]) + txt('is successully uploaded')
                
                self.strStatus.set(status)
                self.statusBar.update()

                if s == 0:
                    self.WriteInstinctPrompts(port)
                else:
                    pass
        elif strBoardVersion in BiBoard_version_list:
            modeName = "Standard"
            # fnBootLoader = path + 'OpenCatEsp32Standard.ino.bootloader.bin'
            fnBootLoader = path + 'OpenCatEsp32' + modeName + '.ino.bootloader.bin'
            # fnPartitions = path + 'OpenCatEsp32Standard.ino.partitions.bin'
            fnPartitions = path + 'OpenCatEsp32' + modeName + '.ino.partitions.bin'
            # fnMainFunc = path + 'OpenCatEsp32Standard.ino.bin '
            fnMainFunc = path + 'OpenCatEsp32' + modeName + '.ino.bin '
            fnBootApp = path + 'boot_app0.bin'

            filename = [fnBootLoader, fnPartitions, fnBootApp, fnMainFunc]
            logger.info(f"{filename}")
            self.strStatus.set(txt('Uploading') + txt('Main function') + ', ' + txt('Time consuming') + '...' )
            self.win.update()
            if self.OSname == 'win32':   # Windows
                esptoolPath = resourcePath + 'esptoolWin/'
            elif self.OSname == 'x11':  # Linux
                esptoolPath = '/usr/bin/'
                path = pathlib.Path(esptoolPath + 'esptool')
                if not path.exists():
                    messagebox.showwarning(txt('Warning'), txt('msgNoneEsptool'))
                    self.force_focus()  # force the main interface to get focus
                    return False
            else:    # Mac
                esptoolPath = resourcePath + 'esptoolMac/'
            # print()
            try:
                # check_call(esptoolPath + 'esptool --chip esp32 --port %s --baud 921600 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 80m --flash_size 16MB 0x1000 %s 0x8000 %s 0xe000 %s 0x10000 %s' % \
                # (port, filename[0], filename[1], filename[2], filename[3]), shell=self.shellOption)
                # subprocess.check_call(esptoolPath + 'esptool --chip esp32 --port %s --baud 921600 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 80m --flash_size 4MB 0x1000 %s 0x8000 %s 0xe000 %s 0x10000 %s' % \
                #     (port, filename[0], filename[1], filename[2], filename[3]), shell=self.shellOption)
                cmd = esptoolPath + 'esptool --chip esp32 --port ' + port + ' --baud 921600 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 80m --flash_size 4MB 0x1000 ' \
                      + filename[0] + \
                      ' 0x8000 ' + filename[1] + \
                      ' 0xe000 ' + filename[2] + \
                      ' 0x10000 ' + filename[3]
                # Run the program and capture output
                process = subprocess.Popen(cmd, shell=self.shellOption, stdout=subprocess.PIPE,
                                           stderr=subprocess.STDOUT)
                output, error = process.communicate()  # Wait for the program to finish
                # printH("error:", error)
                # printH("output:", output)

                # Check for errors (optional)
                if error:
                    logger.info(f"Error running program: {error}")
                else:
                    # Write captured output to a file
                    with open("./logfile.log", "a+", encoding="utf-8") as logfile:
                        logfile.write(output.decode())  # Decode bytes to string
                        # time.sleep(5)
                        # lines = logfile.readlines()
                    # print(lines)
                    file = open('./logfile.log', 'r+')
                    lines = file.readlines()
                    file.close()

                    for line in lines:
                        line = line.strip()  # remove the line break from each line
                        logger.debug(f"{line}")
                        if ("Traceback" in line) or \
                            ("Failed to connect to ESP32" in line) or \
                            ("error" in line) or ("Errno" in line):
                            status = txt('Main function') + txt('failed to upload')
                            self.showMessage(status)
                            return False

            except:
                status = txt('Main function') + txt('failed to upload')
                self.strStatus.set(status)
                self.statusBar.update()
                return False
            else:
                status = txt('Main function') + txt('is successully uploaded')
                
            self.strStatus.set(status)
            self.statusBar.update()
            self.WriteInstinctPrompts(port)

        self.lastSetting = self.currentSetting
        if self.bFacReset:
            self.strMode.set(txt('Standard'))
        self.saveConfigToFile(defaultConfPath)
            
        # for there is no calibrate IMU error
        if not self.bIMUerror:
            print('Finish!')
            messagebox.showinfo(title=None, message=txt('msgFinish'))
        self.force_focus()  # force the main interface to get focus
        return True
        
    def force_focus(self):
        self.win.after(1, lambda: self.win.focus_force())
        
    def on_closing(self):
        if messagebox.askokcancel(txt('Quit'), txt('Do you want to quit?')):
            self.saveConfigToFile(defaultConfPath)
            logger.info(f"{self.configuration}")
            self.win.destroy()

if __name__ == '__main__':
    model = 'Bittle'
    Uploader = Uploader(model, language)
