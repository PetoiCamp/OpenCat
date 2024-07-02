#!/usr/bin/python3
# -*- coding: UTF-8 -*-
from commonVar import *

language = languageList['English']

def txt(key):
    return language.get(key, textEN[key])
    
class Debugger:
    def __init__(self,model,lan):
        self.debuggerReady = False
        global language
        language = lan
#        global goodPorts

        self.model = config.model_
        self.winDebug = Tk()
        self.winDebug.title(txt('Debugger'))
        self.winDebug.geometry('296x56+200+100')
        self.winDebug.resizable(False, False)
        # self.calibSliders = list()
        self.OSname = self.winDebug.call('tk', 'windowingsystem')
        if self.OSname == 'win32':
            self.winDebug.iconbitmap(resourcePath + 'Petoi.ico')
            self.voiceResetButtonW = 30
        else:
            self.voiceResetButtonW = 20
        self.frameDebugButtons = Frame(self.winDebug)
        self.frameDebugButtons.grid(row=0, column=0)
        self.myFont = tkFont.Font(family='Times New Roman', size=12, weight='bold')
        voiceResetButton = Button(self.frameDebugButtons, text=txt('Reset voice module'), font=self.myFont, fg = 'blue', width=self.voiceResetButtonW,command=lambda: self.resetVoice())
        voiceResetButton.grid(row=0, column=0, padx=8, pady=(10, 10))

        self.winDebug.protocol('WM_DELETE_WINDOW', self.on_closing)

        connectPort(goodPorts)
        self.debuggerReady = True

        self.winDebug.focus_force()    # force the main interface to get focus
        self.winDebug.mainloop()


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
            self.showDialog()

    def showDialog(self):
        # # Declare a global variable to access it within the function
        # Create the dialog window
        self.voiceResetWin = Toplevel(self.winDebug)
        self.voiceResetWin.title(txt('Warning'))
        self.voiceResetWin.geometry('680x320')

        fmVoiceInfo = Frame(self.voiceResetWin)  # relief=GROOVE to draw border
        fmVoiceInfo.grid(ipadx=3, ipady=3, padx=3, pady=5, sticky=W + E)

        # creator label
        infoLabel = Label(fmVoiceInfo, text=txt('Voice indicates'), justify='left')
        infoLabel.grid(row=0, columnspan=2, padx=2, pady=6, sticky=W)

        # Load the image
        frameImage = self.createImage(fmVoiceInfo, resourcePath + 'VoiceSwitch.jpeg', 200)
        frameImage.grid(row=1, columnspan=2,pady=5)

        # Create the buttons
        yesButton = tk.Button(fmVoiceInfo, text=txt('Yes'), width=10, command=lambda bFlag = True: self.getButtonClick(bFlag))
        yesButton.grid(row=2, column=0,  padx=3, pady=10)


        noButton = tk.Button(fmVoiceInfo, text=txt('No'), width=10, command=lambda bFlag = False: self.getButtonClick(bFlag))
        noButton.grid(row=2, column=1, padx=3, pady=10)

        self.voiceResetWin.mainloop()  # Start the event loop for the dialog window


    def getButtonClick(self, buttonValue):
        """Function to handle button clicks and close the window."""
        self.voiceReturn = buttonValue  # Assign the button value to the global variable
        logger.info(f"voiceReturn is {self.voiceReturn}")    # debug
        self.voiceResetWin.destroy()  # Destroy the window
        if self.debuggerReady == 1:
            if self.voiceReturn == True:
                cmdList = ["XAb", "XAa"]
                for cmd in cmdList:
                    send(goodPorts, [cmd, 0])
                    if cmd == "XAb":
                        time.sleep(2)
                    else:
                        messagebox.showinfo(None, txt('Reset successfully'))


    def on_closing(self):
        if messagebox.askokcancel(txt('Quit'), txt('Do you want to quit?')):
            self.debuggerReady = False
            self.winDebug.destroy()
            closeAllSerial(goodPorts)
            os._exit(0)


if __name__ == '__main__':
    goodPorts = {}
    try:
        Debugger(model,language)
        closeAllSerial(goodPorts)
        os._exit(0)
    except Exception as e:
        logger.info("Exception")
        closeAllSerial(goodPorts)
        raise e
