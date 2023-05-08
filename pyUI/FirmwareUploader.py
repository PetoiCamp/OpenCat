#!/usr/bin/python3
# -*- coding: UTF-8 -*-

# !/usr/bin/python3

# Jason Wong
# Petoi LLC
# May.1st, 2022

from commonVar import *
import logging
from subprocess import check_call
import threading
from tkinter import ttk
from tkinter import filedialog
import pathlib

FORMAT = '%(asctime)-15s %(name)s - %(levelname)s - %(message)s'
'''
Level: The level determines the minimum priority level of messages to log.
Messages will be logged in order of increasing severity:
DEBUG is the least threatening,
INFO is also not very threatening,
WARNING needs attention,
ERROR needs immediate attention,
and CRITICAL means “drop everything and find out what’s wrong.”
The default starting point is INFO,
which means that the logging module will automatically filter out any DEBUG messages.
'''
#logging.basicConfig(level=logging.DEBUG, format=FORMAT)    # the level defined in ardSerial.py
logging.basicConfig(level=logging.INFO, format=FORMAT)
logger = logging.getLogger(__name__)

regularW = 12
language = languageList['English']
NyBoard_version_list = ['NyBoard_V1_0', 'NyBoard_V1_1', 'NyBoard_V1_2']
BiBoard_version_list = ['BiBoard_V0']

def txt(key):
    return language.get(key, textEN[key])
    
class Uploader:
    def __init__(self,model,lan):
        connectPort(goodPorts, needTesting=False, needSendTask=False)
        closeAllSerial(goodPorts, clearPorts=False)
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
        self.bParaUploaded = False
        Grid.rowconfigure(self.win, 0, weight=1)
        Grid.columnconfigure(self.win, 0, weight=1)
        self.strProduct = StringVar()
        global language
        language = lan
        self.BittleNyBoardModes = list(map(lambda x: txt(x),['Standard', 'RandomMind', 'Voice', 'Camera','GroveSerialPassThrough']))
        self.NybbleNyBoardModes = list(map(lambda x: txt(x),['Standard', 'RandomMind', 'Voice', 'Ultrasonic', 'RandomMind_Ultrasonic','GroveSerialPassThrough']))
        self.BittleBiBoardModes = list(map(lambda x: txt(x), ['Standard']))
        self.NybbleBiBoardModes = list(map(lambda x: txt(x), ['Standard']))
        self.inv_txt = {v: k for k, v in language.items()}
        self.initWidgets()

        self.win.protocol('WM_DELETE_WINDOW', self.on_closing)
        self.win.update()

        t = threading.Thread(target=keepListeningPort, args=(True, self.updatePortlist))
        t.daemon = True
        t.start()
        self.win.mainloop()
        self.force_focus()  # 强制主界面获取focus

    def buildMenu(self):
        self.menuBar = Menu(self.win)
        self.win.configure(menu=self.menuBar)
        
        if self.OSname == 'win32':
            self.win.iconbitmap(r'./resources/Petoi.ico')
        
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
            with open(defaultConfPath, "r") as f:
                lines = f.readlines()
                lines = [line[:-1] for line in lines] # remove the '\n' at the end of each line
                self.defaultLan = lines[0]
                model = lines[1]
                strDefaultPath = lines[2]
                strSwVersion = lines[3]
                strBdVersion = lines[4]
                mode = lines[5]
                f.close()
                
        except Exception as e:
            print ('Create configuration file')
            self.defaultLan = 'English'
            model = 'Bittle'
            strDefaultPath = releasePath
            strSwVersion = '2.0'
            strBdVersion = NyBoard_version_list[-1]
            mode = 'Standard'
            
        num = len(lines)
        logger.debug(f"len(lines): {num}")
        self.lastSetting = [model,strDefaultPath,strSwVersion,strBdVersion,mode]
        

        self.currentSetting = []
        
        logger.info(f"The firmware file folder is {strDefaultPath}")
        self.strFileDir.set(strDefaultPath)

        # 设置 Button 字体
        style = ttk.Style()
        # style.configure('my.TButton', font=('Arial', 14), background='gray')
        # style.configure('my.TEntry', background='white')

        product = ('Bittle', 'Nybble')

        # fmFileDir = ttk.Frame(self.win)
        fmFileDir = Frame(self.win)
        # fmFileDir.pack(side=TOP, fill=BOTH, expand=YES)
        fmFileDir.grid(row=0, columnspan=2, ipadx=2, padx=2, sticky=W + E + N + S)

        self.labFileDir = Label(fmFileDir, text=txt('labFileDir'), font=('Arial', 16))
        self.labFileDir.grid(row=0, column=0, ipadx=2, padx=2, sticky=W)


        self.btnFileDir = Button(fmFileDir, text=txt('btnFileDir'), font=('Arial', 12), foreground='blue',
                                        background=self.backgroundColor, command=self.open_dir)  # 绑定 open_dir 方法
        self.btnFileDir.grid(row=0, column=1, ipadx=5, padx=5, pady=5, sticky=E)

        self.entry = Entry(fmFileDir, textvariable=self.strFileDir, font=('Arial', 16), foreground='green', background='white')
        self.entry.grid(row=1, columnspan=2, ipadx=5, padx=5, sticky=E + W)
        
        fmFileDir.columnconfigure(0, weight=8)  # 尺寸适配
        fmFileDir.columnconfigure(1, weight=1)  # 尺寸适配
        fmFileDir.rowconfigure(1, weight=1)  # 尺寸适配

        fmSoftwareVersion = ttk.Frame(self.win)
        fmSoftwareVersion.grid(row=1, column=0, ipadx=2, padx=2, sticky=W)
        self.labSoftwareVersion = ttk.Label(fmSoftwareVersion, text=txt('labSoftwareVersion'), font=('Arial', 16))
        self.labSoftwareVersion.grid(row=0, ipadx=5, padx=5, sticky=W)
        self.cbSoftwareVersion = ttk.Combobox(fmSoftwareVersion, textvariable=self.strSoftwareVersion, foreground='blue', width=regularW, font=12)
        self.cbSoftwareVersion.bind("<<ComboboxSelected>>",self.chooseSoftwareVersion)

        # list of software_version
        software_version_list = ['1.0', '2.0']
        # 为 Combobox 设置默认项
        self.cbSoftwareVersion.set(self.lastSetting[2])
        
        # 为 Combobox 设置列表项
        self.cbSoftwareVersion['values'] = software_version_list
        self.cbSoftwareVersion.grid(row=1, ipadx=5, padx=5, sticky=W)
#        fmSoftwareVersion.rowconfigure(0, weight=1)  # 尺寸适配
#        fmSoftwareVersion.rowconfigure(1, weight=1)  # 尺寸适配

        fmBoardVersion = ttk.Frame(self.win)
        fmBoardVersion.grid(row=1, column=1, ipadx=2, padx=2, sticky=W)
        self.labBoardVersion = ttk.Label(fmBoardVersion, text=txt('labBoardVersion'), font=('Arial', 16))
        self.labBoardVersion.grid(row=0, ipadx=5, padx=5, sticky=W)
        
        self.cbBoardVersion = ttk.Combobox(fmBoardVersion, textvariable=self.strBoardVersion, foreground='blue', width=regularW, font=12)
        self.cbBoardVersion.bind("<<ComboboxSelected>>", self.chooseBoardVersion)
        # list of board_version
        board_version_list = NyBoard_version_list + BiBoard_version_list
        # 为 Combobox 设置默认项
        self.cbBoardVersion.set(self.lastSetting[3])
        # 为 Combobox 设置列表项
        self.cbBoardVersion['values'] = board_version_list
        self.cbBoardVersion.grid(row=1, ipadx=5, padx=5, sticky=W)
        
#        fmBoardVersion.rowconfigure(0, weight=1)  # 尺寸适配
#        fmBoardVersion.rowconfigure(1, weight=1)  # 尺寸适配

        fmProduct = ttk.Frame(self.win)
        fmProduct.grid(row=2, columnspan=2, ipadx=2, padx=2, sticky=W + E + N + S)
        self.labProduct = ttk.Label(fmProduct, text=txt('labProduct'), font=('Arial', 16))
        self.labProduct.grid(row=0, column=0, ipadx=5, padx=5, sticky=W)
        
        cbProduct = ttk.Combobox(fmProduct, textvariable=self.strProduct, foreground='blue', width=regularW, font=12)
        # list of product
        cbProductList = ['Nybble', 'Bittle']
        # 为 Combobox 设置默认项
        cbProduct.set(self.lastSetting[0])
        # 为 Combobox 设置列表项
        cbProduct['values'] = cbProductList
        cbProduct.grid(row=1, ipadx=5, padx=5, sticky=W)
        cbProduct.bind("<<ComboboxSelected>>",self.chooseProduct)
        
        fmMode = ttk.Frame(self.win)
        fmMode.grid(row=2, column=1, ipadx=2, padx=2, sticky=W + E + N + S)
        self.labMode = ttk.Label(fmMode, text=txt('labMode'), font=('Arial', 16))
        self.labMode.grid(row=0, column=0, ipadx=5, padx=5, sticky=W)

        if self.strProduct.get() == 'Bittle':
            if 'NyBoard' in self.strBoardVersion.get():
                cbModeList = self.BittleNyBoardModes
            else:
                cbModeList = self.BittleBiBoardModes
        elif self.strProduct.get() == 'Nybble':
            if 'NyBoard' in self.strBoardVersion.get():
                cbModeList = self.NybbleNyBoardModes
            else:
                cbModeList = self.NybbleBiBoardModes
        self.cbMode = ttk.Combobox(fmMode, textvariable=self.strMode, foreground='blue', width=regularW, font=12)
        # 为 Combobox 设置默认项
        self.cbMode.set(txt(self.lastSetting[4]))
        # 为 Combobox 设置列表项
        self.cbMode['values'] = cbModeList
        self.cbMode.grid(row=1, ipadx=5, padx=5, sticky=W)

        fmSerial = ttk.Frame(self.win)
        fmSerial.grid(row=3, columnspan=2, ipadx=2, padx=2, sticky=W + E)
        self.labPort = ttk.Label(fmSerial, text=txt('labPort'), font=('Arial', 16))
        self.labPort.grid(row=0, column=0, ipadx=5, padx=5, sticky=W)
        self.cbPort = ttk.Combobox(fmSerial, textvariable=self.strPort, foreground='blue', width=16, font=12)

        # list of serial port number
        port_number_list = []
        # port_list = list(serial.tools.list_ports.comports())
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
        # 为 Combobox 设置列表项
        self.cbPort['values'] = port_number_list
        self.cbPort.grid(row=1, column=0, ipadx=5, padx=5, sticky=W)
#        fmSerial.rowconfigure(0, weight=1)  # 尺寸适配
#        fmSerial.rowconfigure(1, weight=1)  # 尺寸适配

        self.btnUpload = Button(fmSerial, text=txt('btnUpload'), font=('Arial', 22, 'bold'), foreground='blue',
                                       background=self.backgroundColor, relief = 'groove', command=self.autoupload)    # 绑定 autoupload 方法 
        self.btnUpload.grid(row=0, column=1, rowspan=2, ipadx=5, padx=5, pady=5, sticky=E)

        fmStatus = ttk.Frame(self.win)
        fmStatus.grid(row=4, columnspan=2, ipadx=2, padx=2, pady=5, sticky=W + E + N + S)
        self.statusBar = ttk.Label(fmStatus, textvariable=self.strStatus, font=('Arial', 16), relief=SUNKEN)
        self.statusBar.grid(row=0, ipadx=5, padx=5, sticky=W + E + N + S)
        fmStatus.columnconfigure(0, weight=1)    # 尺寸适配
        # fmStatus.rowconfigure(0, weight=1)  # 尺寸适配

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
        # 为 Combobox 设置列表项
        self.cbPort['values'] = port_number_list

    def about(self):
        self.msgbox = messagebox.showinfo(txt('titleVersion'), txt('msgVersion'))
        self.force_focus()  # 强制主界面获取focus

    def setActiveMode(self):
        if self.strSoftwareVersion.get() == '1.0':
            stt = DISABLED
            self.strMode.set(txt('Standard'))
            board_version_list = NyBoard_version_list
            self.strBoardVersion.set(board_version_list[-1])
            
        else:
            stt = NORMAL
            # self.strMode.set(txt(self.lastSetting[4]))
            # self.cbBoardVersion.set(self.lastSetting[3])
            board_version_list = NyBoard_version_list + BiBoard_version_list
        # 为 Combobox 设置列表项
        self.cbBoardVersion['values'] = board_version_list
        self.cbMode.config(state = stt)
    
    def chooseSoftwareVersion(self, event):
        self.setActiveMode()

    def setActiveOption(self):
        if self.cbBoardVersion.get() in BiBoard_version_list:
            stt = DISABLED
            # self.strMode.set(txt('Standard'))
            self.strSoftwareVersion.set('2.0')
        else:
            stt = NORMAL
            # self.strMode.set(txt(self.lastSetting[4]))
            # self.strSoftwareVersion.set(self.lastSetting[2])
        # self.cbMode.config(state=stt)
        self.cbSoftwareVersion.config(state=stt)

    def chooseBoardVersion(self, event):
        self.setActiveOption()
        self.updateMode()

    def chooseProduct(self, event):
        self.updateMode()

    def updateMode(self):
#        print("self.strProduct is " + self.strProduct.get())
        if self.strProduct.get() == 'Bittle':
            if 'NyBoard' in self.strBoardVersion.get():
                modeList = self.BittleNyBoardModes
            else:
                modeList = self.BittleBiBoardModes
        elif self.strProduct.get() == 'Nybble':
            if 'NyBoard' in self.strBoardVersion.get():
                modeList = self.NybbleNyBoardModes
            else:
                modeList = self.NybbleBiBoardModes
        self.cbMode['values'] = modeList

        if self.strMode.get() not in modeList:
            messagebox.showwarning(txt('Warning'),txt('msgMode'))
            self.strMode.set(txt(modeList[0]))
            self.force_focus()  # 强制主界面获取focus

    def formalize(self, strdir=' '):
        sep = "/"
        listDir = strdir.split("/")
#        print("listDir:" + str(listDir))
        if (strdir == str(pathlib.Path().resolve())):
            strdir = sep.join(listDir) + '/release'
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
        # 调用 askdirectory 方法打开目录
        logger.debug(f"{self.strFileDir.get()}")
        if (self.strFileDir.get()).find(releasePath) != -1:
            initDir = releasePath # 初始目录
        else:
            initDir = self.strFileDir # 用户自定目录
        dirpath = filedialog.askdirectory(title=txt('titleFileDir'), initialdir=initDir)

        if dirpath:
            self.formalize(dirpath)
        self.force_focus()  # 强制主界面获取focus

    def encode(self, in_str, encoding='utf-8'):
        if isinstance(in_str, bytes):
            return in_str
        else:
            return in_str.encode(encoding)

    def WriteInstinctPrompts(self, port):
        ser = Communication(port, 115200, 0.5)
        logger.info(f"Connect to usb serial port: {port}.")
        strSoftwareVersion = self.strSoftwareVersion.get()
        bReset = False
        bCalibrate = False
        bUploadInst = False
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
        
        progress = 0
        retMsg = False
        while True:
            time.sleep(0.01)
            if ser.main_engine.in_waiting > 0:
                x = str(ser.main_engine.readline())
                logger.debug(f"{x}")
                if x != "":
                    print(x[2:-1])
                    questionMark = "Y/n"
                    if x.find(questionMark) != -1:
                        if x.find("joint") != -1:
                            prompt = promptJointCalib
                        elif x.find("Instinct") != -1:
                            prompt = promptInstinct
                        elif x.find("Calibrate") != -1:
                            prompt = promptIMU
                        elif x.find("assurance") != -1:
                            ser.Send_data(self.encode("n"))
                            continue
                        if progress>0 and retMsg == True:
                            self.strStatus.set(promptList[progress-1]['result'])
                            self.statusBar.update()
                        retMsg = messagebox.askyesno(txt('Warning'), prompt['message'])
                        if retMsg:
                            self.strStatus.set(prompt['operating'])
                            self.statusBar.update()
                            ser.Send_data(self.encode("Y"))
                        else:
                            ser.Send_data(self.encode("n"))
                            if progress == len(promptList) - 1:
                                break
                        progress+=1
                        
                    elif x.find("sent to mpu.setXAccelOffset") != -1 or x.find("Ready!") != -1:
                        self.strStatus.set(promptIMU['result'])
                        self.statusBar.update()
                        break

        ser.Close_Engine()
        logger.info("close the serial port.")
        self.force_focus()  # 强制主界面获取focus


    def saveConfigToFile(self,filename):
        config = [self.defaultLan,self.lastSetting[0],self.lastSetting[1],self.lastSetting[2],self.lastSetting[3],self.lastSetting[4]]
        print(config)
        with open(filename, "w") as f:
            lines = '\n'.join(config)+'\n'
            f.writelines(lines)
            f.close()

#    def progressiveDots(self, label):
#        while self.inProgress:
#            print(label,flush = True)
#            time.sleep(1)
#            label += '.'
#            self.strStatus.set(label)
#            self.win.update()
#        print('exit fun')
    
    def autoupload(self):
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
            self.force_focus()  # 强制主界面获取focus
            return False

        # NyBoard_V1_X software version are all the same
        if "NyBoard_V1" in strBoardVersion:
            pathBoardVersion = "NyBoard_V1"
        else:
            pathBoardVersion = strBoardVersion

        path = self.strFileDir.get() + '/' + strSoftwareVersion + '/' + strProd + '/' + pathBoardVersion + '/'

        port = self.strPort.get()
        print(self.strPort.get())
        if port == ' ' or port == '':
            messagebox.showwarning(txt('Warning'), txt('msgPort'))
            self.force_focus()  # 强制主界面获取focus
            return False

        if strBoardVersion in NyBoard_version_list:
            fnWriteI = path + 'WriteInstinct.ino.hex'
            fnOpenCat = path + 'OpenCat' + strMode + '.ino.hex'
            filename = [fnWriteI, fnOpenCat]
            print(filename)
            uploadStage = ['Parameters', 'Main function']
            for s in range(len(uploadStage)):
                if s == 0 and self.bParaUploaded and self.currentSetting[:4] == self.lastSetting[:4]:
                    continue
                self.strStatus.set(txt('Uploading') + txt(uploadStage[s]) + '...' )
                self.win.update()
                # self.inProgress = True
                # status = txt('Uploading') + txt(uploadStage[s]) + '.'
                # t = threading.Thread(target=self.progressiveDots, args=(status,))
                # t.start()
                if self.OSname == 'win32':
                    avrdudePath = './resources/avrdudeWin/'
                elif self.OSname == 'x11':     # Linux
                    avrdudePath = '/usr/bin/'
                    path = pathlib.Path(avrdudePath + 'avrdude')
                    if not path.exists():
                        messagebox.showwarning(txt('Warning'), txt('msgNoneAvrdude'))
                        self.force_focus()  # 强制主界面获取focus
                        return False
                    # avrdudeconfPath = '/etc/avrdude/'      # Fedora / CentOS
                    avrdudeconfPath = '/etc/'            # Debian / Ubuntu
                else:
                    avrdudePath = './resources/avrdudeMac/'
                print()
                try:
                    if self.OSname == 'x11':     # Linuxself.OSname == 'x11':     # Linux
                        check_call(avrdudePath + 'avrdude -C' + avrdudeconfPath + 'avrdude.conf -v -V -patmega328p -carduino -P%s -b115200 -D -Uflash:w:%s:i' % \
                                (port, filename[s]), shell=self.shellOption)
                    else:
                        check_call(avrdudePath+'avrdude -C'+avrdudePath+'avrdude.conf -v -V -patmega328p -carduino -P%s -b115200 -D -Uflash:w:%s:i' % \
                                (port, filename[s]), shell=self.shellOption)
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
                    messagebox.showinfo(title=None, message=txt('parameterFinish'))

        elif strBoardVersion in BiBoard_version_list:
            # fnBootLoader = path + 'OpenCatEsp32Standard.ino.bootloader.bin'
            fnBootLoader = path + 'OpenCatEsp32' + strMode + '.ino.bootloader.bin'
            # fnPartitions = path + 'OpenCatEsp32Standard.ino.partitions.bin'
            fnPartitions = path + 'OpenCatEsp32' + strMode + '.ino.partitions.bin'
            fnBootApp = path + 'boot_app0.bin'
            # fnMainFunc = path + 'OpenCatEsp32Standard.ino.bin '
            fnMainFunc = path + 'OpenCatEsp32' + strMode + '.ino.bin '

            filename = [fnBootLoader, fnPartitions, fnBootApp, fnMainFunc]
            print(filename)
            self.strStatus.set(txt('Uploading') + txt('Main function') + '...' )
            self.win.update()
            if self.OSname == 'win32':
                esptoolPath = './resources/esptoolWin/'
            else:
                esptoolPath = './resources/esptoolMac/'
            print()
            try:
                check_call(esptoolPath + 'esptool --chip esp32 --port %s --baud 921600 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 80m --flash_size 16MB 0x1000 %s 0x8000 %s 0xe000 %s 0x10000 %s' % \
                (port, filename[0], filename[1], filename[2], filename[3]), shell=self.shellOption)
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
        self.bParaUploaded = True
        self.saveConfigToFile(defaultConfPath)
            
        print('Finish!')
        messagebox.showinfo(title=None, message=txt('msgFinish'))
        self.force_focus()  # 强制主界面获取focus
        return True
        
    def force_focus(self):
        self.win.after(1, lambda: self.win.focus_force())
        
    def on_closing(self):
        if messagebox.askokcancel(txt('Quit'), txt('Do you want to quit?')):
            self.saveConfigToFile(defaultConfPath)
            self.win.destroy()

if __name__ == '__main__':
    Uploader = Uploader(model, language)
