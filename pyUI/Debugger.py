#!/usr/bin/python3
# -*- coding: UTF-8 -*-

# Junfeng Wang
# Petoi LLC
# Jun. 26th, 2024


from commonVar import *

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
                config.model_ = model
                print('Use the model set in the UI interface.')
            time.sleep(0.01)
        self.model = config.model_

        self.winDebug = Tk()
        self.debuggerReady = False

        self.OSname = self.winDebug.call('tk', 'windowingsystem')
        if self.OSname == 'win32':
            self.winDebug.iconbitmap(resourcePath + 'Petoi.ico')
            self.winDebug.geometry('398x300+800+400')
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
        if self.model == "BittleX":
            modelName = "Bittle X"
        elif self.model == "BittleR":
            modelName = "Bittle R"
        else:
            modelName = self.model
        self.modelLabel = Label(self.winDebug, text=modelName, font=self.myFont)
        self.modelLabel.grid(row=0, column=0, pady=10)
        voiceResetButton = Button(self.winDebug, text=txt('Reset voice module'), font=self.myFont, fg='blue', width=bw, relief='raised',
                   command=lambda: self.resetVoice())
        voiceResetButton.grid(row=1, column=0, padx=10, pady=(0, 10))
        tip(voiceResetButton, txt('tipRstVoice'))

        biboardConfigButton = Button(self.winDebug, text=txt('BiBoard Config'), font=self.myFont, fg='blue', width=bw,
                                  relief='raised',
                                  command=lambda: self.biboardConfig())
        biboardConfigButton.grid(row=2, column=0, padx=10, pady=(0, 10))
        tip(voiceResetButton, txt('tipbiboardConfig'))

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
        for key in NaJoints:
            file.add_command(label=key, command=lambda model=key: self.changeModel(model))
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
            self.showDialog()

    def showDialog(self):
        # Declare a global variable to access it within the function
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
        logger.debug(f"voiceReturn is {self.voiceReturn}")
        self.voiceResetWin.destroy()  # Destroy the window
        if self.debuggerReady == 1:
            if self.voiceReturn == True:
                if 'Chinese'in txt('lan'):
                    cmdList = ["XAa", "XAb"]
                else:
                    cmdList = ["XAb", "XAa"]

                for cmd in cmdList:
                    send(goodPorts, [cmd, 0])
                    if cmd == cmdList[0]:
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
        model = "Bittle"
        Debugger(model,language)
        closeAllSerial(goodPorts)
        os._exit(0)
    except Exception as e:
        logger.info("Exception")
        closeAllSerial(goodPorts)
        raise e
