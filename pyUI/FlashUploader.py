#!/opt/anaconda3/envs/petoi/bin/python3
# -*- coding: UTF-8 -*-

# !/usr/bin/python3

# Jason Wong
# Petoi LLC
# May.1st, 2022

from commonVar import *
import serial.tools.list_ports
import logging
from subprocess import call
from tkinter import ttk
from tkinter import filedialog

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
# logging.basicConfig(level=logging.DEBUG, format=FORMAT)
logging.basicConfig(level=logging.INFO, format=FORMAT)
logger = logging.getLogger(__name__)

language = languageList['English']
def txt(key):
    return language.get(key, textEN[key])
    
class Uploader:
    def __init__(self,model,lan):
        self.win = Tk()
        self.OSname = self.win.call('tk', 'windowingsystem')
        self.shellOption = True
        if self.OSname == 'aqua':
            self.win.geometry('530x356+260+200')
            self.backgroundColor ='gray'
        else:
            self.win.geometry('530x400+260+100')
            self.backgroundColor = None

        if self.OSname == 'win32':
            self.shellOption = False

        Grid.rowconfigure(self.win, 0, weight=1)
        Grid.columnconfigure(self.win, 0, weight=1)
        self.strProduct = StringVar()
        self.strProduct.set(model)
        global language
        language = lan
        self.initWidgets()

        self.win.protocol('WM_DELETE_WINDOW', self.on_closing)
        self.win.update()
        self.win.mainloop()
        self.force_focus()  # 强制主界面获取focus
                

#    def refreshLabels(self, language):
#        if self.strProduct.get() == 'Bittle':
#            print(language.labFileDir)
#            modeTuple = ('Standard', 'Random_Mind', 'Voice', 'Camera')
#        elif self.strProduct.get() == 'Nybble':
#            modeTuple = ('Standard', 'Random_Mind', 'Voice', 'Ultrasonic')
#
#        self.lanDict = {
#            self.labFileDir: txt('labFileDir'),
#            self.btnFileDir: txt('btnFileDir'),
#            self.labPort: txt('labPort'),
#            self.labSoftwareVersion: txt('labSoftwareVersion'),
#            self.labBoardVersion: txt('labBoardVersion'),
#            self.labProduct: txt('labProduct'),
#            self.labMode: txt('labMode'),
#            self.rbnModes[0]: modeTuple[0],
#            self.rbnModes[1]: modeTuple[1],
#            self.rbnModes[2]: modeTuple[2],
#            self.rbnModes[3]: modeTuple[3],
#            # self.labFile: txt('labFile'),
#            # self.checkBnWI: txt('cbnFileWI'),
#            # self.checkBnOC: txt('cbnFileMF'),
#            # self.labNote: txt('labNote'),
#            self.btnUpload: txt('btnUpload')
#        }
#
#        self.win.title(txt('title'))
#
#        self.buildMenu()
#
#        for key in self.lanDict:
#            key.configure(text=self.lanDict[key])

    def buildMenu(self):
        self.menuBar = Menu(self.win)
        self.win.configure(menu=self.menuBar)
        
        if self.OSname == 'win32':
            self.win.iconbitmap(r'./Petoi.ico')
#        self.LanguageMenu = Menu(self.menuBar, tearoff=0)
#        self.LanguageMenu.add_command(label=txt('labEng'), command=self._eng)
#        self.LanguageMenu.add_command(label=txt('labChi'), command=self._chi)
#        self.menuBar.add_cascade(label=txt('labTrans'), menu=self.LanguageMenu)
        
        self.helpMenu = Menu(self.menuBar, tearoff=0)
        self.helpMenu.add_command(label=txt('labAbout'), command=self.about)
        self.menuBar.add_cascade(label=txt('labHelp'), menu=self.helpMenu)
        
#    def _chi(self):
#        self.language = Language('Chinese')
#        self.refreshLabels(self.language)
#
#    def _eng(self):
#        self.language = Language('English')
#        self.refreshLabels(self.language)

    def initWidgets(self):
        self.win.title(txt('uploaderTitle'))
        self.buildMenu()

        self.strFileDir = StringVar()
        self.strPort = StringVar()
        self.strStatus = StringVar()
        self.strStatus.set(' ')
        self.strSoftwareVersion = StringVar()
        self.strSoftwareVersion.set('2.0')
        self.strBoardVersion = StringVar()
        self.strBoardVersion.set('NyBoard_V1_0')
        #self.strProduct = StringVar()
        
        self.intMode = IntVar()
        self.strMode = StringVar()
        self.strFileName = StringVar()

        lines = []
        with open("./defaultConfig.txt", "r") as f:
            lines = f.readlines()
            strDefaultPath = lines[0][:-1]  # 将txt文件的第一行读入到字符串strDefaultPath中
            model = lines[1][:-1]
            self.defaultLan = lines[2][:-1]
            f.close()
            #self.strProduct.set(model)
            
        self.lastSetting = []
        self.currentSetting = []
        logger.info(f"{strDefaultPath}")
        # f = open("./defaultConfig.txt", "r")  # 设置文件对象
        # strDefaultPath = f.read()  # 将txt文件的所有内容读入到字符串str中
        # f.close()  # 将文件关闭
        self.strFileDir.set(strDefaultPath)

        # 设置 Button 字体
        style = ttk.Style()
        # style.configure('my.TButton', font=('Arial', 14), background='gray')
        style.configure('my.TRadiobutton', font=('Arial', 15))
        style.configure('my.TCheckbutton', font=('Arial', 15))
        # style.configure('my.TEntry', background='white')

        product = ('Bittle', 'Nybble')

        # fmFileDir = ttk.Frame(self.win)
        fmFileDir = Frame(self.win)
        # fmFileDir.pack(side=TOP, fill=BOTH, expand=YES)
        fmFileDir.grid(row=0, ipadx=2, padx=2, sticky=W + E + N + S)

        self.labFileDir = Label(fmFileDir, text=txt('labFileDir'), font=('Arial', 16))
        self.labFileDir.grid(row=0, column=0, ipadx=2, padx=2, sticky=W)


        self.btnFileDir = Button(fmFileDir, text=txt('btnFileDir'), font=('Arial', 14), foreground='blue',
                                        background=self.backgroundColor, command=self.open_dir)  # 绑定 open_dir 方法
        self.btnFileDir.grid(row=0, column=1, ipadx=5, padx=5, sticky=E)

        self.entry = Entry(fmFileDir, textvariable=self.strFileDir,
                                  font=('Arial', 16), foreground='green', background='white')
        self.entry.grid(row=1, columnspan=2, ipadx=5, padx=5, sticky=E + W)
        
        fmFileDir.columnconfigure(0, weight=8)  # 尺寸适配
        fmFileDir.columnconfigure(1, weight=1)  # 尺寸适配
        fmFileDir.rowconfigure(1, weight=1)  # 尺寸适配

        fmSerial = ttk.Frame(self.win)
        fmSerial.grid(row=1, ipadx=2, padx=2, sticky=W + E + N + S)
        self.labPort = ttk.Label(fmSerial, text=txt('labPort'), font=('Arial', 16))
        self.labPort.grid(row=0, ipadx=5, padx=5, sticky=W)
        cb = ttk.Combobox(fmSerial, textvariable=self.strPort, foreground='blue', width=20, font=12)

        # list of serial port number
        port_list_number = []
        port_list = list(serial.tools.list_ports.comports())
        if len(port_list) == 0:
            port_list_number = [' ']
            print("The Serial port can't find!")
        else:
            cb.set(port_list[-1][0])
            for each_port in reversed(port_list):
                port_list_number.append(each_port[0])
        # 为 Combobox 设置列表项
        cb['values'] = port_list_number
        cb.grid(row=1, ipadx=5, padx=5, sticky=W)
        fmSerial.rowconfigure(0, weight=1)  # 尺寸适配
        fmSerial.rowconfigure(1, weight=1)  # 尺寸适配

        fmSoftwareVersion = ttk.Frame(self.win)
        fmSoftwareVersion.grid(row=2, ipadx=2, padx=2, sticky=W + E + N + S)
        self.labSoftwareVersion = ttk.Label(fmSoftwareVersion, text=txt('labSoftwareVersion'), font=('Arial', 16))
        self.labSoftwareVersion.grid(row=0, ipadx=5, padx=5, sticky=W)
        cbSoftwareVersion = ttk.Combobox(fmSoftwareVersion, textvariable=self.strSoftwareVersion, foreground='blue', width=20, font=12)
        cbSoftwareVersion.bind("<<ComboboxSelected>>",self.chooseSoftwareVersion)

        # list of board version
        software_version_list = ['1.0', '2.0']
        # 为 Combobox 设置默认项
        if self.lastSetting == []:
            cbSoftwareVersion.set(software_version_list[-1])
        else:
            cbSoftwareVersion.set(self.lastSetting[0])
        # 为 Combobox 设置列表项
        cbSoftwareVersion['values'] = software_version_list
        cbSoftwareVersion.grid(row=1, ipadx=5, padx=5, sticky=W)
        fmSoftwareVersion.rowconfigure(0, weight=1)  # 尺寸适配
        fmSoftwareVersion.rowconfigure(1, weight=1)  # 尺寸适配

        fmBoardVersion = ttk.Frame(self.win)
        fmBoardVersion.grid(row=3, ipadx=2, padx=2, sticky=W + E + N + S)
        self.labBoardVersion = ttk.Label(fmBoardVersion, text=txt('labBoardVersion'), font=('Arial', 16))
        self.labBoardVersion.grid(row=0, ipadx=5, padx=5, sticky=W)
        cbBoardVersion = ttk.Combobox(fmBoardVersion, textvariable=self.strBoardVersion, foreground='blue', width=20, font=12)

        # list of board version
        board_version_list = ['NyBoard_V1_0', 'NyBoard_V1_1']
        # 为 Combobox 设置默认项
        if self.lastSetting == []:
            cbBoardVersion.set(board_version_list[0])
        else:
            cbBoardVersion.set(self.lastSetting[1])
        # 为 Combobox 设置列表项
        cbBoardVersion['values'] = board_version_list
        cbBoardVersion.grid(row=1, ipadx=5, padx=5, sticky=W)
        fmBoardVersion.rowconfigure(0, weight=1)  # 尺寸适配
        fmBoardVersion.rowconfigure(1, weight=1)  # 尺寸适配

        fmProduct = ttk.Frame(self.win)
        fmProduct.grid(row=4, ipadx=2, padx=2, sticky=W + E + N + S)
        self.labProduct = ttk.Label(fmProduct, text=txt('labProduct'), font=('Arial', 16))
        self.labProduct.grid(row=0, column=0, ipadx=5, padx=5, sticky=W)
        # 为 rbProduct 设置默认项

        c = 1
        for p in product:
            rbProduct = ttk.Radiobutton(fmProduct, text=p, value=p, style='my.TRadiobutton',
                                     variable=self.strProduct, command=self.chooseProduct)
            rbProduct.grid(row=0, column=c, ipadx=5, padx=5, sticky=W) # pack(side=LEFT, padx=5)
            c += 1
        fmProduct.rowconfigure(0, weight=1)  # 尺寸适配

        fmMode = ttk.Frame(self.win)
        fmMode.grid(row=5, ipadx=2, padx=2, sticky=W + E + N + S)
        self.labMode = ttk.Label(fmMode, text=txt('labMode'), font=('Arial', 16))
        self.labMode.grid(row=0, column=0, ipadx=5, padx=5, sticky=W)

        if self.strProduct.get() == 'Bittle':
            modeTuple = ('Standard', 'Random_Mind', 'Voice', 'Camera')
        elif self.strProduct.get() == 'Nybble':
            modeTuple = ('Standard', 'Random_Mind', 'Voice', 'Ultrasonic')

        # 为 rbProduct 设置默认项
        if self.lastSetting == []:
            # self.strMode.set(self.intMode.get(), "Invalid num of selection"))
            # self.strMode.set(switcher.get(self.intMode.get(), "Invalid num of selection"))
            self.intMode.set(0)
            self.strMode.set("Standard")
        else:
            if self.strProduct.get() == 'Bittle':
                specialMode = "Camera"
            elif self.strProduct.get() == 'Nybble':
                specialMode = "Ultrasonic"
            switcher = {
                "Standard": 0,
                "Random_Mind": 1,
                "Voice": 2,
                specialMode: 3
            }
            self.intMode.set(switcher.get(self.lastSetting[3], "Invalid mode of selection"))
            self.strMode.set(self.lastSetting[3])
        self.strFileName.set("OpenCat" + self.strMode.get() + ".ino.hex")
        c = 0
        self.rbnModes = []
        for i in range(len(modeTuple)):
            rbMode = ttk.Radiobutton(fmMode, text=modeTuple[i], value=i, style='my.TRadiobutton',
                                     variable=self.intMode, state=NORMAL, command=self.chooseMode)
            rbMode.grid(row=1, column=c, ipadx=5, padx=5, sticky=W)
            self.rbnModes.append(rbMode)
            c += 1
        fmMode.rowconfigure(0, weight=1)  # 尺寸适配
        
        self.setActiveMode()


        self.intVarWI = IntVar()
        self.intVarWI.set(1)


        self.intVarOC = IntVar()
        self.intVarOC.set(1)

        fmUpload = Frame(self.win)
        fmUpload.grid(row=6, ipadx=2, padx=2, pady=5, sticky=W + E + N + S)
 

        self.btnUpload = Button(fmUpload, text=txt('btnUpload'), font=('Arial', 14), foreground='blue',
                                       background=self.backgroundColor, command=self.autoupload)    # 绑定 autoupload 方法
        self.btnUpload.grid(row=0, ipadx=5, padx=5, sticky=W + E + N + S)
        fmUpload.columnconfigure(0, weight=1)  # 尺寸适配
        fmUpload.rowconfigure(0, weight=1)  # 尺寸适配

        fmStatus = ttk.Frame(self.win)
        fmStatus.grid(row=7, ipadx=2, padx=2, pady=5, sticky=W + E + N + S)
        self.statusBar = ttk.Label(fmStatus, textvariable=self.strStatus, font=('Arial', 16), relief=SUNKEN)
        self.statusBar.grid(row=0, ipadx=5, padx=5, sticky=W + E + N + S)
        fmStatus.columnconfigure(0, weight=1)    # 尺寸适配
        fmStatus.rowconfigure(0, weight=1)  # 尺寸适配


    def about(self):
        self.msgbox = messagebox.showinfo(txt('titleVersion'), txt('msgVersion'))
        self.force_focus()  # 强制主界面获取focus

    def setActiveMode(self):
        if self.strSoftwareVersion.get() == '1.0':
            stt = DISABLED
            self.intMode.set(0)
            self.strMode.set('Standard')
            print(self.strMode.get())
            self.strFileName.set("OpenCat" + self.strMode.get() + ".ino.hex")
            print(self.strFileName.get())
        else:
            stt = NORMAL
        for i in range(1,4):
            self.rbnModes[i].configure(state=stt)
    
    def chooseSoftwareVersion(self, event):
        self.setActiveMode()


    def chooseProduct(self):
#        print("self.strProduct is " + self.strProduct.get())
        if self.strProduct.get() == 'Bittle':
            modeTuple = ('Standard', 'Random_Mind', 'Voice', 'Camera')
        elif self.strProduct.get() == 'Nybble':
            modeTuple = ('Standard', 'Random_Mind', 'Voice', 'Ultrasonic')

        for i in range(len(modeTuple)):
            self.rbnModes[i].configure(text=modeTuple[i])

        if (self.strProduct.get() == 'Bittle' and self.strMode.get() == "Ultrasonic") \
            or (self.strProduct.get() == 'Nybble' and self.strMode.get() == "Camera"):
            messagebox.showwarning(txt('titleWarning'),txt('msgMode'))
            self.intMode.set(0)
            self.strMode.set('Standard')
            self.strFileName.set("OpenCat" + self.strMode.get() + ".ino.hex")
            print(self.strFileName.get())
            self.force_focus()  # 强制主界面获取focus


    def chooseMode(self):

        if self.strProduct.get() == 'Bittle':
            specialMode = "Camera"
        elif self.strProduct.get() == 'Nybble':
            specialMode = "Ultrasonic"
        switcher = {
            0 : "Standard",
            1 : "Random_Mind",
            2 : "Voice",
            3 : specialMode
        }
        self.strMode.set(switcher.get(self.intMode.get(), "Invalid num of selection"))
        self.strFileName.set("OpenCat" + self.strMode.get() + ".ino.hex")
        


    def formalize(self, strdir=' '):
        sep = "/"
        listDir = strdir.split("/")
        print("listDir:" + str(listDir))
        if (listDir[-1] == "FlashUploader"):
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
        if (self.strFileDir.get()).find('./release') != -1:
            initDir = r'./release' # 初始目录
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

    def WriteInstinctProcess(self, port):
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
            promptList = [promptJointCalib,promptInstinct,promptJointCalib]
        elif strSoftwareVersion == '2.0':
            promptList = [promptJointCalib,promptJointCalib]
        
        progress = 0;
        while True:
            time.sleep(0.01)
            if ser.main_engine.in_waiting > 0:
                x = str(ser.main_engine.readline())
                logger.debug(f"{x}")
                if x != "":
                    questionMark = "Y/n"
                    if x.find(questionMark) != -1:
                        if x.find("joint") != -1:
                            prompt = promptJointCalib
                        elif x.find("Instinct") != -1:
                            prompt = promptInstinct
                        elif x.find("Calibrate") != -1:
                            prompt = promptIMU
                        if progress>0:
                            print(progress)
                            self.strStatus.set(promptList[progress-1]['result'])
                            self.statusBar.update()
                        progress+=1
                        retMsg = messagebox.askyesno(txt('titleWarning'), prompt['message'])
                        if retMsg:
                            self.strStatus.set(prompt['operating'])
                            self.statusBar.update()
                            ser.Send_data(self.encode("Y"))
                        else:
                            ser.Send_data(self.encode("n"))
                            if progress == 2:
                                break
                        
                    elif x.find("sent to mpu.setXAccelOffset") != -1 or x.find("Ready!") != -1:
                        self.strStatus.set(prompt['result'])
                        self.statusBar.update()
                        break
                        
                        

        ser.Close_Engine()
        logger.info("close the serial port.")
        self.force_focus()  # 强制主界面获取focus


    def autoupload(self):
        strProd = self.strProduct.get()
        strSoftwareVersion = self.strSoftwareVersion.get()
        strBoardVersion = self.strBoardVersion.get()
        strMode = self.strMode.get()
        self.currentSetting = [strSoftwareVersion, strBoardVersion, strProd, strMode]
        logger.info(f"currentSetting: {self.currentSetting}.")
        path = self.strFileDir.get() + "/" + strSoftwareVersion + "/" + strBoardVersion + "/" + strProd
        strFileName = self.strFileName.get()
        
        fnWriteI = path + "/WriteInstinct.ino.hex"
        fnOpenCat = path + "/" + strFileName
        filename = [fnWriteI, fnOpenCat]
        print(filename)
        port = self.strPort.get()
        print(self.strPort.get())

        if self.strFileDir.get() == '' or self.strFileDir.get() == ' ':
            messagebox.showwarning(txt('titleWarning'), txt('msgFileDir'))
            self.force_focus()  # 强制主界面获取focus
            return False

        if port == ' ' or port == '':
            messagebox.showwarning(txt('titleWarning'), txt('msgPort'))
            self.force_focus()  # 强制主界面获取focus
            return False
        logger.info(f"lastSetting: {self.lastSetting}.")
        if (self.lastSetting == []) or (self.currentSetting[:3] != self.lastSetting[:3]):
            self.intVarWI.set(1)
        else:
            self.intVarWI.set(0)




        ret = -1
        for file in filename:
            if (self.intVarWI.get() == 0) and (file == fnWriteI):
                continue
            if (self.intVarOC.get() == 0) and (file == fnOpenCat):
                continue

            if file == fnWriteI:
                self.strStatus.set(txt('labStatus1') + txt('cbnFileWI') + '...' )
            elif file == fnOpenCat:
                self.strStatus.set(txt('labStatus1') + txt('cbnFileMF') + '...')
            self.statusBar.update()

            ret = call('./avrdude -C./avrdude.conf -v -patmega328p -carduino -P%s -b115200 -D -Uflash:w:%s:i' % \
                            (port, file), shell=self.shellOption)

            if ret == 0:
                status =txt('labStatus3')
            else:
                status = txt('labStatus2')
            if file == fnWriteI:
                self.strStatus.set(txt('cbnFileWI') + ' ' + status)
                self.statusBar.update()
                if ret == 0:
                    self.WriteInstinctProcess(port)
                    messagebox.showinfo(title=None, message=txt('parameterFinish'))
            elif file == fnOpenCat:
                self.strStatus.set(txt('cbnFileMF') + ' ' + status)
                self.statusBar.update()
                
            if status == txt('labStatus2'):
                return False

        self.lastSetting = self.currentSetting
        
        with open("./defaultConfig.txt", "w") as f:
            f.write(self.strFileDir.get() +'\n'+self.strProduct.get()+'\n' + self.defaultLan+'\n')
            f.close()
            
        print('Finish!')
        messagebox.showinfo(title=None, message=txt('msgFinish'))
        self.force_focus()  # 强制主界面获取focus
        return True
        
    def force_focus(self):
        self.win.after(1, lambda: self.win.focus_force())
        
    def on_closing(self):
        if messagebox.askokcancel(txt('Quit'), txt('Do you want to quit?')):
            with open("./defaultConfig.txt", "w") as f:
                f.write(self.strFileDir.get() +'\n'+self.strProduct.get()+'\n' + self.defaultLan+'\n')
            self.win.destroy()

if __name__ == '__main__':
    Uploader = Uploader(model, language)
