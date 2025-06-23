#!/usr/bin/python3
# -*- coding: UTF-8 -*-

# Junfeng Wang
# Petoi LLC
# Jun. 26th, 2024


from commonVar import *
from tkinter import ttk

language = languageList['English']

def txt(key):
    return language.get(key, textEN[key])
    
class Debugger:
    def __init__(self,model,lan):
        global language
        language = lan
#        global goodPorts
        connectPort(goodPorts)
        start = time.time()
        while config.model_ == '':
            if time.time() - start > 5:
                config.model_ = model    # If can not get the model name, use the model set in the UI interface.
            time.sleep(0.01)
        self.configName = config.model_

        self.winDebug = Tk()
        self.debuggerReady = False
        self.dialogType = "voiceRst"
        self.strStatus = StringVar()

        self.OSname = self.winDebug.call('tk', 'windowingsystem')
        if self.OSname == 'win32':
            self.winDebug.iconbitmap(resourcePath + 'Petoi.ico')
            self.winDebug.geometry('398x280+800+400')
        elif self.OSname == 'aqua':
            self.winDebug.geometry('+800+400')
            self.backgroundColor = 'gray'
        else:
            self.winDebug.tk.call('wm', 'iconphoto', self.winDebug._w, "-default",
                                PhotoImage(file= resourcePath + 'Petoi.png'))
            self.winDebug.geometry('+800+400')

        self.myFont = tkFont.Font(
            family='Times New Roman', size=20, weight='bold')
        self.winDebug.title(txt('Debugger'))
        self.createMenu()
        bw = 23
        self.modelLabel = Label(self.winDebug, text=displayName(self.configName), font=self.myFont)
        self.modelLabel.grid(row=0, column=0, pady=10)

        voiceResetButton = Button(self.winDebug, text=txt('Reset voice module'), font=self.myFont, fg='blue',
                                  width=bw, relief='raised',command=lambda: self.resetVoice())
        voiceResetButton.grid(row=1, column=0, padx=10, pady=(0, 10))
        tip(voiceResetButton, txt('tipRstVoice'))

        imuCaliButton = Button(self.winDebug, text=txt('Calibrate gyroscope'), font=self.myFont, fg='blue', width=bw,
                               relief='raised',
                               command=lambda: self.imuCali())
        imuCaliButton.grid(row=2, column=0, padx=10, pady=(0, 10))
        tip(imuCaliButton, txt('tipImuCali'))

        # Set row weights to enable expandable middle space in the layout.
        self.winDebug.rowconfigure(3, weight=1)
        
        fmStatus = ttk.Frame(self.winDebug)
        fmStatus.grid(row=4, column=0, ipadx=2, padx=2, pady=5, sticky=W + E)
        self.statusBar = ttk.Label(fmStatus, textvariable=self.strStatus, font=('Arial', 16), relief=SUNKEN)
        self.statusBar.grid(row=0, ipadx=5, padx=5, sticky=W + E)
        fmStatus.columnconfigure(0, weight=1)

        self.debuggerReady = True
        self.winDebug.protocol('WM_DELETE_WINDOW', self.on_closing)
        self.winDebug.update()
        self.winDebug.resizable(False, False)
        self.winDebug.focus_force()    # force the main interface to get focus
        self.winDebug.mainloop()
        
    
    def createMenu(self):
        self.menubar = Menu(self.winDebug, background='#ff8000', foreground='black', activebackground='white',
                            activeforeground='black')
        file = Menu(self.menubar, tearoff=0, background='#ffcc99', foreground='black')
        for m in modelOptions:
            file.add_command(label=m, command=lambda model=m: self.changeModel(model))
        self.menubar.add_cascade(label=txt('Model'), menu=file)

        lan = Menu(self.menubar, tearoff=0)
        for l in languageList:
            lan.add_command(label=languageList[l]['lanOption'], command=lambda lanChoice=l: self.changeLan(lanChoice))
        self.menubar.add_cascade(label=txt('lanMenu'), menu=lan)

        helpMenu = Menu(self.menubar, tearoff=0)
        helpMenu.add_command(label=txt('About'), command=self.showAbout)
        self.menubar.add_cascade(label=txt('Help'), menu=helpMenu)

        self.winDebug.config(menu=self.menubar)


    def changeModel(self, modelName):
        self.modelLabel.configure(text=modelName)
        
    
    def changeLan(self, l):
        global language
        if self.debuggerReady and txt('lan') != l:
            language = copy.deepcopy(languageList[l])
            self.menubar.destroy()
            self.createMenu()
            self.winDebug.title(txt('Debugger'))
            self.winDebug.winfo_children()[1].config(text=txt('Reset voice module'))
            tip(self.winDebug.winfo_children()[1], txt('tipRstVoice'))
            self.winDebug.winfo_children()[2].config(text=txt('Calibrate gyroscope'))
            tip(self.winDebug.winfo_children()[2], txt('tipImuCali'))
            
    
    def showAbout(self):
        messagebox.showinfo(txt('titleVersion'), txt('msgVersion'))
        self.winDebug.focus_force()

            
    def createImage(self, frame, imgFile, imgW):
        img = Image.open(imgFile)

        ratio = img.size[0] / imgW
        img = img.resize((imgW, round(img.size[1] / ratio)))
        image = ImageTk.PhotoImage(img)
        imageFrame = Label(frame, image=image)
        imageFrame.image = image
        return imageFrame


    def resetVoice(self):
        if self.debuggerReady == 1:
            cmd = "XAc"
            send(goodPorts, [cmd, 0])
            self.strStatus.set(txt('Reset voice module'))
            self.statusBar.update()
            self.dialogType = "voiceRst"
            self.showDialog(self.dialogType)

    def imuCali(self):
        if self.debuggerReady == 1:
            cmd = "d"
            send(goodPorts, [cmd, 0])
            self.strStatus.set(txt('Calibrate gyroscope'))
            self.statusBar.update()
            self.dialogType = "imuCali"
            self.showDialog(self.dialogType)


    def showDialog(self, dialogType='voiceRst'):
        # Declare a global variable to access it within the function
        # Create the dialog window
        self.dialogWin = Toplevel(self.winDebug)
        self.dialogWin.title(txt('Warning'))
        self.dialogWin.geometry('680x320')

        fmDialog = Frame(self.dialogWin)  # relief=GROOVE to draw border
        fmDialog.grid(ipadx=3, ipady=3, padx=3, pady=5, sticky=W + E)

        if self.dialogType == "voiceRst":
            # creator label
            infoLabel = Label(fmDialog, text=txt('Voice indicates'), justify='left')
            infoLabel.grid(row=0, columnspan=2, padx=2, pady=6, sticky=W)

            # Load the image
            frameImage = self.createImage(fmDialog, resourcePath + 'VoiceSwitch.jpeg', 200)
            frameImage.grid(row=1, columnspan=2,pady=5)
        elif self.dialogType == "imuCali":
            # creator label
            infoLabel = Label(fmDialog, text=txt('IMU indicates'), justify='left')
            infoLabel.grid(row=0, columnspan=2, padx=2, pady=6, sticky=W)

            # Load the image
            frameImage = self.createImage(fmDialog, resourcePath + 'rest.jpeg', 200)
            frameImage.grid(row=1, columnspan=2, pady=5)

        # Create the buttons
        yesButton = tk.Button(fmDialog, text=txt('Yes'), width=10, command=lambda bFlag = True: self.getButtonClick(bFlag))
        yesButton.grid(row=2, column=0,  padx=3, pady=10)


        noButton = tk.Button(fmDialog, text=txt('No'), width=10, command=lambda bFlag = False: self.getButtonClick(bFlag))
        noButton.grid(row=2, column=1, padx=3, pady=10)

        self.dialogWin.mainloop()  # Start the event loop for the dialog window


    def encode(self, in_str, encoding='utf-8'):
        if isinstance(in_str, bytes):
            return in_str
        else:
            return in_str.encode(encoding)


    def getButtonClick(self, buttonValue):
        """Function to handle button clicks and close the window."""
        logger.debug(f"returnVal is {buttonValue}")
        self.dialogWin.destroy()  # Destroy the window
        if self.debuggerReady == 1:
            if self.dialogType == "voiceRst":
                if buttonValue == True:
                    if 'Chinese'in txt('lan'):
                        cmdList = ["XAa", "XAb"]
                    else:
                        cmdList = ["XAb", "XAa"]

                    for cmd in cmdList:
                        send(goodPorts, [cmd, 0])
                        if cmd == cmdList[0]:
                            time.sleep(2)
                        else:
                            txtResult = txt('Reset successfully')
                            self.strStatus.set(txtResult)
                            self.statusBar.update()
                            messagebox.showinfo(None, txtResult)
                else:
                    self.strStatus.set(' ')
                    self.statusBar.update()
            elif self.dialogType == "imuCali":
                if buttonValue == True:
                    self.strStatus.set(txt('calibrating IMU'))
                    self.statusBar.update()
                    for port in goodPorts:
                        port.Send_data(self.encode("gc"))
                    # Create a flag variable to track whether calibration is successful.
                    calibration_success = False
                    # Set timeout (seconds):
                    timeout = 20
                    start_time = time.time()
                    
                    # Monitor serial output in real-time:
                    while time.time() - start_time < timeout:
                        # Brief pause to avoid high CPU usage.
                        time.sleep(0.01)
                        # Check all available serial ports.
                        for port in goodPorts:
                            if port.main_engine.in_waiting > 0:
                                try:
                                    # Read serial data of each line
                                    data = port.main_engine.readline()
                                    data_str = data.decode('ISO-8859-1').strip()
                                    # Record output to log
                                    logger.info(f"IMU Calibration output: {data_str}")
                                    
                                    # Check the successful calibration information in the serial output.
                                    if "Calibration done" in data_str:
                                        calibration_success = True
                                        break
                                except Exception as e:
                                    logger.error(f"Error reading serial data: {e}")
                        
                        # If calibration is successful, exit the loop.
                        if calibration_success:
                            break

                    # Show a message according to the calibration results.
                    if calibration_success:
                        txtResult = txt('IMU Calibration successfully')
                        messagebox.showinfo('Petoi Desktop App', txtResult)
                    else:
                        txtResult = txt('IMU Calibration failed')
                        if self.OSname == 'aqua':  # for macOS
                            # folder_path = "file:///Applications/Petoi Desktop App.app/Contents/Resources"
                            folder_path = "/Applications/Petoi Desktop App.app/Contents/Resources" + '\n'
                        else:  # for Windows or Linux
                            path = os.getcwd()
                            # folder_path = "file://" + path  # Replace with the actual folder path
                            folder_path = path + '\n'  # Replace with the actual folder path
                        messagebox.showinfo('Petoi Desktop App',
                                            txtResult + '\n' + txt('logLocation') + folder_path + txt('checkLogfile'))

                    self.strStatus.set(txtResult)
                    self.statusBar.update()
                else:
                    self.strStatus.set(' ')
                    self.statusBar.update()

    def on_closing(self):
        if messagebox.askokcancel(txt('Quit'), txt('Do you want to quit?')):
            self.debuggerReady = False
            self.winDebug.destroy()
            closeAllSerial(goodPorts)
            os._exit(0)


if __name__ == '__main__':
    goodPorts = {}
    try:
        model = "Bittle"
        Debugger(model,language)
        closeAllSerial(goodPorts)
        os._exit(0)
    except Exception as e:
        logger.info("Exception")
        closeAllSerial(goodPorts)
        raise e
