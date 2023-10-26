#!/usr/bin/python3
# -*- coding: UTF-8 -*-

# Rongzhong Li
# Petoi LLC
# May.22nd, 2022
import sys
sys.path.append('../serialMaster/')
import random
import tkinter.font as tkFont
import copy
import threading
from tkinter.filedialog import asksaveasfile, askopenfilename
from tkinter.colorchooser import askcolor
from commonVar import *
import re
from tkinter import ttk
language = languageList['English']
def txt(key):
    return language.get(key, textEN[key])
    
def rgbtohex(r, g, b):
    return f'#{r:02x}{g:02x}{b:02x}'

sixAxisNames = ['Yaw', 'Pitch', 'Roll', 'Spinal', 'Height', 'Sideway']
dialTable = {'Connect': 'Connected', 'Servo': 'p', 'Gyro': 'G', 'Random': 'z'}
tipDial = ['tipConnect', 'tipServo', 'tipGyro', 'tipRandom']
labelSkillEditorHeader = ['Repeat', 'Set', 'Step', 'Trig', 'Angle', 'Delay', 'Note', 'Del', 'Add']
tipSkillEditor = ['tipRepeat', 'tipSet', 'tipStep',  'tipTrig', 'tipTrigAngle','tipDelay', 'tipNote', 'tipDel', 'tipAdd', ]
cLoop, cSet, cStep,  cTrig, cAngle, cDelay, cNote, cDel, cAdd = range(len(labelSkillEditorHeader))

axisDisable = {
    'Nybble': [0, 5],
    'Bittle': [0, 5],
    'DoF16' : []

}
NaJoints = {
    'Nybble': [3, 4, 5, 6, 7],
    'Bittle': [1, 2, 3, 4, 5, 6, 7],
    'Bittle X': [1, 2, 3, 4, 5, 6, 7],
    'DoF16' : []
}
jointConfig = {
    'Nybble': '><',
    'Bittle': '>>',
    'DoF16': '>>'
}
triggerAxis = {
    0: 'None',
    1: 'Pitch',
    -1: '-Pitch',
    2: 'Roll',
    -2: '-Roll',
}

# word_file = '/usr/share/dict/words'
# WORDS = open(word_file).read().splitlines()
animalNames = [  # used for memorizing individual frames
    'ant', 'bat', 'bear', 'bee', 'bird', 'buffalo', 'cat', 'chicken', 'cow', 'dog', 'dolphin', 'duck', 'elephant',
    'fish', 'fox', 'frog', 'goose', 'goat', 'horse', 'kangaroo', 'lion', 'monkey', 'owl', 'ox', 'penguin', 'person',
    'pig', 'rabbit', 'sheep', 'tiger', 'whale', 'wolf', 'zebra']
WORDS = animalNames


class SkillComposer:
    def __init__(self,model, lan):
        global language
        language = lan
        connectPort(goodPorts)
        start = time.time()
        while config.model_ == '':
            if time.time()-start > 5:
                config.model_ = 'Bittle'
                print('Use Bittle as default model')
            time.sleep(0.01)
        self.model = config.model_
        try:
            with open(defaultConfPath, "r", encoding="utf-8") as f:
                lines = f.readlines()
                # f.close()
            lines = [line.split('\n')[0] for line in lines]  # remove the '\n' at the end of each line
            num = len(lines)
            logger.debug(f"len(lines): {num}")
            self.defaultLan = lines[0]
            self.defaultPath = lines[2]
            self.defaultSwVer = lines[3]
            self.defaultBdVer = lines[4]
            self.defaultMode = lines[5]
            if len(lines) >= 8:
                self.defaultCreator = lines[6]
                self.defaultLocation = lines[7]
            else:
                self.defaultCreator = txt('Nature')
                self.defaultLocation = txt('Earth')

            self.configuration = [self.defaultLan, self.model, self.defaultPath, self.defaultSwVer, self.defaultBdVer,
                                  self.defaultMode, self.defaultCreator, self.defaultLocation]

        except Exception as e:
            print('Create configuration file')
            self.defaultLan = 'English'
            self.defaultPath = releasePath[:-1]
            self.defaultSwVer = '2.0'
            self.defaultBdVer = NyBoard_version
            self.defaultMode = 'Standard'
            self.defaultCreator = txt('Nature')
            self.defaultLocation = txt('Earth')
            self.configuration = [self.defaultLan, self.model, self.defaultPath, self.defaultSwVer, self.defaultBdVer,
                                  self.defaultMode, self.defaultCreator, self.defaultLocation]
        self.postureTable = postureDict[self.model]
        ports = goodPorts
        self.window = Tk()
        self.sliders = list()
        self.values = list()
        self.dialValue = list()
        self.controllerLabels = list()
        self.binderValue = list()
        self.binderButton = list()
        self.previousBinderValue = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ]
        self.keepChecking = True
        self.ready = 0
        self.creatorInfoAcquired = False
        self.creator = StringVar()
        self.location = StringVar()
        self.creator.set(txt('Nature'))
        self.location.set(txt('Earth'))

        self.OSname = self.window.call('tk', 'windowingsystem')
        print(self.OSname)
        self.window.geometry('+100+10')
        self.window.resizable(False, False)
        
        if self.OSname == 'aqua':
            self.backgroundColor = 'gray'
        else:
            self.backgroundColor = None

        if self.OSname == 'win32':
            self.window.iconbitmap(resourcePath + 'Petoi.ico')
            # global frameItemWidth
            self.frameItemWidth = [2, 4, 3, 5, 4, 4, 7, 3, 3]
            self.headerOffset = [0, 0, 0, 0, 0, 0, 0, 0, 0]

            self.sixW = 6
            self.sliderW = 320
            self.buttonW = 10
            self.calibButtonW = 8
            self.canvasW = 330
            self.mirrorW = 2
            self.MirrorW = 10
            self.connectW = 8
            self.dialW = 7
            self.portW = 5
            self.dialPad = 2
        else:
            if self.OSname == 'aqua':
                self.frameItemWidth = [2, 2, 3,  4, 3, 3,4, 1, 1]
                self.headerOffset = [2, 2, 2, 2, 2, 2, 2, 2, 2]

            else:
                self.frameItemWidth = [2, 2, 3, 4, 4, 4, 5, 2, 2]
                self.headerOffset = [0, 0, 1,  1, 0,0, 0, 0, 1]

            self.sixW = 5
            self.sliderW = 338
            self.buttonW = 8
            self.calibButtonW = 6
            self.canvasW = 420
            self.mirrorW = 1
            self.MirrorW = 9
            self.connectW = 8
            self.dialW = 6
            self.portW = 12
            self.dialPad = 3

        self.myFont = tkFont.Font(
            family='Times New Roman', size=20, weight='bold')
        self.window.title(txt('skillComposerTitle'))
        self.totalFrame = 0
        self.activeFrame = 0
        self.frameList = list()
        self.frameData = [0, 0, 0, 0,
                          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                          8, 0, 0, 0, ]
        self.originalAngle = [0, 0, 0, 0,
                              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                              8, 0, 0, 0, ]
        self.pause = False
        self.playStop = False
        self.mirror = False
        self.createMenu()
        self.createController()
        self.placeProductImage()
        self.createDial()
        self.createPosture()
        self.createSkillEditor()
        self.createRowScheduler()

        self.ready = 1
        self.window.protocol('WM_DELETE_WINDOW', self.on_closing)
        self.window.update()
        t = threading.Thread(target=keepCheckingPort, args=(goodPorts,lambda:self.keepChecking,lambda:True,self.updatePortMenu,))
        t.daemon = True
        t.start()
        time.sleep(2)
        res = send(goodPorts, ['G', 0])
        printH("gyro status:",res )
        if res != -1 and res[0] == 'G':
            res = send(goodPorts, ['G', 0])
            printH("gyro status:",res )
        
        self.window.mainloop()

    def createMenu(self):
        self.menubar = Menu(self.window, background='#ff8000', foreground='black', activebackground='white',
                            activeforeground='black')
        file = Menu(self.menubar, tearoff=0, background='#ffcc99', foreground='black')
        for key in NaJoints:
            file.add_command(label=key, command=lambda model=key: self.changeModel(model))
        self.menubar.add_cascade(label=txt('Model'), menu=file)


        lan = Menu(self.menubar, tearoff=0)
        for l in languageList:
            lan.add_command(label=languageList[l]['lanOption'], command=lambda lanChoice=l: self.changeLan(lanChoice))
        self.menubar.add_cascade(label=txt('lanMenu'), menu=lan)

        util = Menu(self.menubar, tearoff=0)
        util.add_command(label=txt('Eye color picker'), command=lambda: self.popEyeColor())
        util.add_command(label=txt('Creator Information'), command=lambda: self.getCreatorInfo(True))
        self.menubar.add_cascade(label=txt('Utility'), menu=util)
        
        helpMenu = Menu(self.menubar, tearoff=0)
        helpMenu.add_command(label=txt('About'), command=self.showAbout)
        self.menubar.add_cascade(label=txt('Help'), menu=helpMenu)

        self.window.config(menu=self.menubar)

    def scheduler(self):
        print('Scheduler')

    def uploadFirmware(self):
        print('Uploader')
        Uploader()

    def createController(self):
        self.frameController = Frame(self.window)
        self.frameController.grid(row=0, column=0, rowspan=9, padx=(5, 10), pady=5)
        label = Label(self.frameController, text=txt('Joint Controller'), font=self.myFont)
        label.grid(row=0, column=0, columnspan=8)
        self.controllerLabels.append(label)
        unbindButton = Button(self.frameController, text=txt('Unbind All'), fg='blue', command=self.unbindAll)
        unbindButton.grid(row=5, column=3, columnspan=2)
        self.controllerLabels.append(unbindButton)
        
        centerWidth = 2
        for i in range(16):
            cSPAN = 1
            if i < 4:
                tickDirection = 1
                cSPAN = 4
                if i < 2:
                    ROW = 0
                else:
                    ROW = 11
                if 0 < i < 3:
                    COL = 4
                else:
                    COL = 0
                rSPAN = 1
                ORI = HORIZONTAL
                LEN = self.sliderW

            else:
                tickDirection = -1
                leftQ = (i - 1) % 4 > 1
                frontQ = i % 4 < 2
                upperQ = i / 4 < 3

                rSPAN = 3
                ROW = 2 + (1 - frontQ) * (rSPAN + 2)
                if leftQ:
                    COL = 3 - i // 4
                else:
                    COL = centerWidth + 2 + i // 4
                ORI = VERTICAL
                LEN = 150

            if i in NaJoints[self.model]:
                stt = DISABLED
                clr = 'light yellow'
            else:
                stt = NORMAL
                clr = 'yellow'
            if i in range(8, 12):
                sideLabel = txt(sideNames[i % 8]) + '\n'
            else:
                sideLabel = ''
            label = Label(self.frameController,
                          text=sideLabel + '(' + str(i) + ')\n' + txt(scaleNames[i]))

            value = DoubleVar()
            sliderBar = Scale(self.frameController, state=stt, fg='blue', bg=clr, variable=value, orient=ORI,
                              borderwidth=2, relief='flat', width=8, from_=-180 * tickDirection, to=180 * tickDirection,
                              length=LEN, tickinterval=90, resolution=1, repeatdelay=100, repeatinterval=100,
                              command=lambda value, idx=i: self.setAngle(idx, value))
            sliderBar.set(0)
            label.grid(row=ROW + 1, column=COL, columnspan=cSPAN, pady=2, sticky='s')
            sliderBar.grid(row=ROW + 2, column=COL, rowspan=rSPAN, columnspan=cSPAN)

            self.sliders.append(sliderBar)
            self.values.append(value)
            self.controllerLabels.append(label)

            if i in range(16):
                binderValue = IntVar()
                values = {"+": 1,
                          "-": -1, }
                for d in range(2):
                    button = Radiobutton(self.frameController, text=list(values)[d], fg='blue', variable=binderValue,
                                         value=list(values.values())[d], indicator=0, state=stt,
                                         background="light blue", width=1,
                                         command=lambda joint=i: self.updateRadio(joint))
                    if i < 4:
                        button.grid(row=ROW + 1, column=COL + (1 - d) * (cSPAN - 1), sticky='s')
                    else:
                        button.grid(row=ROW + 2 + d * (rSPAN - 1), column=COL, sticky='ns'[d])
                    binderValue.set(0)
                    if d == 0:
                        tip(button, txt('tipBinder'))
                    else:
                        tip(button, txt('tipRevBinder'))
                    self.binderButton.append(button)
                self.binderValue.append(binderValue)

        self.frameImu = Frame(self.frameController)
        self.frameImu.grid(row=6, column=3, rowspan=6, columnspan=2)
        for i in range(6):
            frm = -40
            to2 = 40
            if i in axisDisable[self.model]:
                stt = DISABLED
                clr = 'light yellow'
            else:
                stt = NORMAL
                clr = 'yellow'
            if i == 2:
                frm = -30
                to2 = 30
            elif i == 3:
                frm = -15
                to2 = 15
            elif i == 4:
                frm = -50
                to2 = 40

            label = Label(self.frameImu, text=txt(sixAxisNames[i]), width=self.sixW, height=2, fg='blue',
                          bg='Light Blue')

            value = DoubleVar()
            sliderBar = Scale(self.frameImu, state=stt, fg='blue', bg=clr, variable=value, orient=HORIZONTAL,
                              borderwidth=2, relief='flat', width=10, from_=frm, to=to2, length=125, resolution=1,
                              command=lambda ang, idx=i: self.set6Axis(idx, ang))  # tickinterval=(to2-frm)//4,
            sliderBar.set(0)
            label.grid(row=i, column=0)
            sliderBar.grid(row=i, column=1, columnspan=centerWidth)
            self.sliders.append(sliderBar)
            self.values.append(value)
            self.controllerLabels.append(label)

    def createDial(self):
        self.frameDial = Frame(self.window)
        self.frameDial.grid(row=0, column=1)
        labelDial = Label(self.frameDial, text=txt('State Dials'), font=self.myFont)
        labelDial.grid(row=0, column=0, columnspan=5, pady=5)
        defaultValue = [1, 1, 0, 0]
        textColor = ['red','green']
        for i in range(len(dialTable)):
            key = list(dialTable)[i]
            if len(goodPorts) > 0 or i == 0:
                dialState = NORMAL
            else:
                dialState = DISABLED
#            if i == 2:#disable gyro
#                dialState = DISABLED
            if i == 0:
                wth = self.connectW
                if len(goodPorts) > 0:
                    defaultValue[0] = True
                    key = 'Connected'
                else:
                    defaultValue[0] = False
                    key = 'Connect'
            else:
                wth = self.dialW
            value = BooleanVar()
            button = Checkbutton(self.frameDial, text=txt(key), indicator=0, width=wth, fg=textColor[defaultValue[i]], state=dialState,
                                 var=value, command=lambda idx=i: self.dial(idx))
            value.set(defaultValue[i])
            self.dialValue.append(value)
            button.grid(row=1, column=i + (i > 0), padx=self.dialPad)
            tip(button, txt(tipDial[i]))

        self.createPortMenu()
        
        self.newCmd = StringVar()
        entryCmd = Entry(self.frameDial, textvariable=self.newCmd)
        entryCmd.grid(row=2, column=0, columnspan=4, padx=3, sticky=E + W)
        button = Button(self.frameDial,text=txt('Send'),fg='blue',width=self.dialW-2,command=self.sendCmd)
        button.grid(row=2, column=4, padx=3)
        entryCmd.bind('<Return>',self.sendCmd)
        


    def createPortMenu(self):
        self.port = StringVar()
        self.options = [txt('None')]  # goodPorts.values())
        self.portMenu = OptionMenu(self.frameDial, self.port, *self.options)

        self.portMenu.config(width=self.portW, fg='blue')
        self.port.trace('w', lambda *args: self.changePort(''))
        self.portMenu.grid(row=1, column=1, padx=2)
        self.updatePortMenu()

        tip(self.portMenu, txt('tipPortMenu'))

    def updatePortMenu(self):
        print('***@@@ update menu function started')#debug
        self.options = list(goodPorts.values())
        menu = self.portMenu['menu']
        menu.delete(0, 'end')
        stt = NORMAL
        #        self.dialValue[0].set(self.keepChecking)
        if len(self.options) == 0:
            self.options.insert(0, txt('None'))
            stt = DISABLED
            if self.keepChecking:
                self.dialValue[0].set(True)
                self.frameDial.winfo_children()[1].config(text=txt('Listening'), fg='orange')
            else:
                self.frameDial.winfo_children()[1].config(text=txt('Connect'), fg='red')
        else:
            #            global currentModel
            #            if currentModel != model:
            #                self.changeModel(currentModel)# doesn't work yet
            if len(self.options) > 1:
                self.options.insert(0, txt('All'))
            if self.keepChecking:
                self.frameDial.winfo_children()[1].config(text=txt('Connected'), fg='green')
        for string in self.options:
            menu.add_command(label=string, command=lambda p=string: self.port.set(p))
        self.port.set(self.options[0])

        buttons = [self.frameDial.winfo_children()[i] for i in [2,3,4]]# [2,4] by skipping 3 was used to disable the gyro
        for b in buttons:
            b.config(state=stt)
        self.portMenu.config(state=stt)
        print('***@@@ update menu function ended')#debug

    def changePort(self, magicArg):
        global ports
        buttons = [self.frameDial.winfo_children()[i] for i in [2,3,4]]# [2,4] by skipping 3 was used to disable the gyro
        for b in buttons:
            if len(goodPorts) == 0:
                b.config(state=DISABLED)
            else:
                b.config(state=NORMAL)

        inv_dict = {v: k for k, v in goodPorts.items()}
        if self.port.get() == txt('All'):
            ports = goodPorts
        elif self.port.get() == txt('None'):
            ports = []
        else:
            singlePort = inv_dict[self.port.get()]
            ports = [singlePort]

    def createPosture(self):
        self.framePosture = Frame(self.window)
        self.framePosture.grid(row=1, column=1)
        labelPosture = Label(self.framePosture, text=txt('Postures'), font=self.myFont)
        labelPosture.grid(row=0, column=0, columnspan=4)
        i = 0
        for pose in self.postureTable:
            button = Button(self.framePosture, text=pose, fg='blue', width=self.buttonW,
                            command=lambda p=pose: self.setPose(p))
            button.grid(row=i // 4 + 1, column=i % 4, padx=3)
            i += 1

    def createSkillEditor(self):
        self.frameSkillEditor = Frame(self.window)
        self.frameSkillEditor.grid(row=2, column=1)

        labelSkillEditor = Label(self.frameSkillEditor, text=txt('Skill Editor'), font=self.myFont)
        labelSkillEditor.grid(row=0, column=0, columnspan=4)
        pd = 3
        self.buttonPlay = Button(self.frameSkillEditor, text=txt('Play'), width=self.buttonW, fg='green',
                                 command=self.playThread)
        self.buttonPlay.grid(row=1, column=0, padx=pd)

        tip(self.buttonPlay, txt('tipPlay'))

        buttonImp = Button(self.frameSkillEditor, text=txt('Import'), width=self.buttonW, fg='blue',
                           command=self.popImport)
        buttonImp.grid(row=1, column=1, padx=pd)

        tip(buttonImp, txt('tipImport'))
        
        buttonRestart = Button(self.frameSkillEditor, text=txt('Restart'), width=self.buttonW, fg='red',
                               command=self.restartSkillEditor)
        buttonRestart.grid(row=1, column=2, padx=pd)

        tip(buttonRestart, txt('tipRestart'))

        buttonExp = Button(self.frameSkillEditor, text=txt('Export'), width=self.buttonW, fg='blue',
                           command=self.export)
        buttonExp.grid(row=1, column=3, padx=pd)

        tip(buttonExp, txt('tipExport'))

        buttonUndo = Button(self.frameSkillEditor, text=txt('Undo'), width=self.buttonW, fg='blue', state=DISABLED,
                            command=self.restartSkillEditor)
        buttonUndo.grid(row=2, column=0, padx=pd)

        buttonRedo = Button(self.frameSkillEditor, text=txt('Redo'), width=self.buttonW, fg='blue', state=DISABLED,
                            command=self.restartSkillEditor)
        buttonRedo.grid(row=2, column=1, padx=pd)

        cbMiroX = Checkbutton(self.frameSkillEditor, text=txt('mirror'), indicator=0, width=self.MirrorW,
                              fg='blue', variable=self.mirror, onvalue=True, offvalue=False,
                              command=self.setMirror)
        cbMiroX.grid(row=2, column=2, sticky='e', padx=pd)

        tip(cbMiroX, txt('tipMirrorXport'))

        buttonMirror = Button(self.frameSkillEditor, text=txt('>|<'), width=self.mirrorW, fg='blue',
                              command=self.generateMirrorFrame)
        buttonMirror.grid(row=2, column=2, sticky='w', padx=pd)

        tip(buttonMirror, txt('tipMirror'))

        self.gaitOrBehavior = StringVar()
        self.GorB = OptionMenu(self.frameSkillEditor, self.gaitOrBehavior, txt('Gait'), txt('Behavior'))
        self.GorB.config(width=6, fg='blue')
        self.gaitOrBehavior.set(txt('Behavior'))
        self.GorB.grid(row=2, column=3, padx=pd)

        tip(self.GorB, txt('tipGorB'))

    def setMirror(self):
        self.mirror = not self.mirror

    def createRowScheduler(self):
        self.frameRowScheduler = Frame(self.window)  # https://blog.teclado.com/tkinter-scrollable-frames/
        self.frameRowScheduler.grid(row=3, column=1, sticky='we')

        self.vRepeat = IntVar()
        self.loopRepeat = Entry(self.frameRowScheduler, width=self.frameItemWidth[cLoop], textvariable=self.vRepeat)
        self.loopRepeat.grid(row=0, column=cLoop)

        tip(self.loopRepeat, txt('tipRepeat'))

        for i in range(1, len(labelSkillEditorHeader)):
            label = Label(self.frameRowScheduler, text=txt(labelSkillEditorHeader[i]),
                          width=self.frameItemWidth[i] + self.headerOffset[i])
            label.grid(row=0, column=i, sticky='w')
            if tipSkillEditor[i]:
                tip(label, txt(tipSkillEditor[i]))

        canvas = Canvas(self.frameRowScheduler, width=self.canvasW, height=310, bd=0)
        scrollbar = Scrollbar(self.frameRowScheduler, orient='vertical', cursor='double_arrow', troughcolor='yellow',
                              width=15, command=canvas.yview)
        self.scrollable_frame = Frame(canvas)

        self.scrollable_frame.bind(
            '<Configure>',
            lambda e: canvas.config(
                scrollregion=canvas.bbox('all')
            )
        )
        canvas.create_window((0, 0), window=self.scrollable_frame, anchor='nw')
        canvas.config(yscrollcommand=scrollbar.set)
        canvas.grid(row=1, column=0, columnspan=len(labelSkillEditorHeader))

        scrollbar.grid(row=1, column=len(labelSkillEditorHeader), sticky='ens')
        self.restartSkillEditor()

    def createImage(self, frame, imgFile, imgW):
        img = Image.open(imgFile)
        ratio = img.size[0] / imgW
        img = img.resize((imgW, round(img.size[1] / ratio)))
        image = ImageTk.PhotoImage(img)
        imageFrame = Label(frame, image=image)
        imageFrame.image = image
        return imageFrame

    def placeProductImage(self):
        self.frameImage = self.createImage(self.frameController, resourcePath + self.model + '.jpeg', 200)
        self.frameImage.grid(row=3, column=3, rowspan=2, columnspan=2)

    def changeLan(self, l):
        global language
        if self.ready and txt('lan') != l:
            global triggerAxis
            inv_triggerAxis = {txt(v): k for k, v in triggerAxis.items()}
            language = languageList[l]
            self.defaultLan = l
            logger.debug(f"{self.defaultLan}")
            self.window.title(txt('skillComposerTitle'))
            self.menubar.destroy()
            self.controllerLabels[0].config(text=txt('Joint Controller'))
            self.controllerLabels[1].config(text=txt('Unbind All'))
            for i in range(6):
                self.controllerLabels[2 + 16 + i].config(text=txt(sixAxisNames[i]))
            for i in range(16):
                if i in range(8, 12):
                    sideLabel = txt(sideNames[i % 8]) + '\n'
                else:
                    sideLabel = '\n'
                self.controllerLabels[2 + i].config(text=sideLabel + '(' + str(i) + ')\n' + txt(scaleNames[i]))

                for d in range(2):
                    if d == 0:
                        tip(self.binderButton[i * 2], txt('tipBinder'))
                    else:
                        tip(self.binderButton[i * 2 + 1], txt('tipRevBinder'))

            self.frameDial.destroy()
            self.framePosture.destroy()
            self.frameSkillEditor.destroy()
            self.createMenu()
            self.createDial()
            self.createPosture()
            self.createSkillEditor()
            for i in range(len(labelSkillEditorHeader)):
                if i > 0:
                    self.frameRowScheduler.winfo_children()[i].config(text=txt(labelSkillEditorHeader[i]))
                if tipSkillEditor[i]:
                    tip(self.frameRowScheduler.winfo_children()[i], txt(tipSkillEditor[i]))
            for r in range(len(self.frameList)):
                tip(self.getWidget(r, cLoop), txt('tipLoop'))
                tt = '='  # +txt('Set')
                ft = 'sans 12'
                if self.activeFrame == r:
                    ft = 'sans 14 bold'
                    if self.frameList[r][2] != self.frameData:
                        tt = '!'  # + txt('Save')
                        self.getWidget(r, cSet).config(fg='red')
                self.getWidget(r, cSet).config(text=tt, font=ft)

                step = self.getWidget(r, cStep).get()
                self.getWidget(r, cStep).config(values=('1', '2', '4', '8', '12', '16', '32', '48', '64', txt('max')))
                self.getWidget(r, cStep).delete(0, END)
                if step.isnumeric():
                    self.getWidget(r, cStep).insert(0, step)
                else:
                    self.getWidget(r, cStep).insert(0, txt('max'))

                vTrig = self.getWidget(r, cTrig).get()
                self.getWidget(r, cTrig).config(values=list(map(lambda x:txt(x),triggerAxis.values())))
                self.getWidget(r, cTrig).delete(0, END)
                self.getWidget(r, cTrig).insert(0, txt(triggerAxis[inv_triggerAxis[vTrig]]))

    def showAbout(self):
        messagebox.showinfo('Petoi Controller UI',
                            u'Petoi Controller for OpenCat\nOpen Source on GitHub\nCopyright © Petoi LLC\nwww.petoi.com')

    def changeModel(self, modelName):
        if self.ready and modelName != self.model:
            if 'Bittle' in modelName:
                modelName = 'Bittle'
            self.model = copy.deepcopy(modelName)
            self.postureTable = postureDict[self.model]
            self.framePosture.destroy()
            self.frameImage.destroy()

            for i in range(16):
                if i in NaJoints[self.model]:
                    stt = DISABLED
                    clr = 'light yellow'
                else:
                    stt = NORMAL
                    clr = 'yellow'
                self.sliders[i].config(state=stt, bg=clr)
                self.binderButton[i * 2].config(state=stt)
                self.binderButton[i * 2 + 1].config(state=stt)
            self.createPosture()
            self.placeProductImage()
            self.restartSkillEditor()

    def addFrame(self, currentRow):
        singleFrame = Frame(self.scrollable_frame, borderwidth=1, relief=RAISED)

        vChecked = BooleanVar()
        loopCheck = Checkbutton(singleFrame, variable=vChecked, text=str(currentRow), onvalue=True, offvalue=False,
                                indicator=0, width=self.frameItemWidth[cLoop],
                                command=lambda idx=currentRow: self.setCheckBox(idx))
        loopCheck.grid(row=0, column=cLoop)
        tip(loopCheck, txt('tipLoop'))
        #        rowLabel = Label(singleFrame, text = str(currentRow), width = self.frameItemWidth[cLabel])
        #        rowLabel.grid(row=0, column=cLabel)

        setButton = Button(singleFrame, text='=',  # +txt('Set')
                           font='sans 14 bold', fg='blue',  # width=self.frameItemWidth[cSet],
                           command=lambda idx=currentRow: self.setFrame(idx))

        vStep = StringVar()
        Spinbox(singleFrame, width=self.frameItemWidth[cStep],
                values=('1', '2', '4', '8', '12', '16', '32', '48', '64', txt('max')), textvariable=vStep, wrap=True).grid(
            row=0, column=cStep)

        vTrig = StringVar()
        spTrig = Spinbox(singleFrame, width=self.frameItemWidth[cTrig], values=list(map(lambda x:txt(x),triggerAxis.values())),
                         textvariable=vTrig, wrap=True)
        spTrig.grid(row=0, column=cTrig)

        vAngle = IntVar()
        Spinbox(singleFrame, width=self.frameItemWidth[cAngle], from_=-128, to=127, textvariable=vAngle,
                wrap=True).grid(
            row=0, column=cAngle)
            
        vDelay = IntVar()
        delayOption = list(range(0, 100, 50)) + list(range(100, 1000, 100)) + list(range(1000, 6001, 1000))

        Spinbox(singleFrame, width=self.frameItemWidth[cDelay], values=delayOption, textvariable=vDelay,
                wrap=True).grid(
            row=0, column=cDelay)

        vNote = StringVar()
        #        letters = string.ascii_lowercase + string.ascii_uppercase + string.digits
        #        vNote.set('note: '+ ''.join(random.choice(letters) for i in range(5)) )

        while True:
            note = random.choice(WORDS)
            if len(note) <= 5:
                break
        vNote.set(note + str(currentRow))  # 'note')
        color = rgbtohex(random.choice(range(64, 192)), random.choice(range(64, 192)), random.choice(range(64, 192)))
        Entry(singleFrame, width=self.frameItemWidth[cNote], fg=color, textvariable=vNote, bd=1).grid(row=0,
                                                                                                      column=cNote)

        delButton = Button(singleFrame, text='<', fg='red', width=self.frameItemWidth[cDel],
                           command=lambda idx=currentRow: self.delFrame(idx))

        addButton = Button(singleFrame, text='v', fg='green', width=self.frameItemWidth[cAdd],
                           command=lambda idx=currentRow: self.addFrame(idx + 1))

        setButton.grid(row=0, column=cSet)
        delButton.grid(row=0, column=cDel)
        addButton.grid(row=0, column=cAdd)

        self.updateButtonCommand(currentRow, 1)
        if currentRow == 0:
            newFrameData = copy.deepcopy(self.frameData)
        else:
            #            newFrameData = copy.deepcopy(self.frameList[currentRow - 1][2])
            newFrameData = copy.deepcopy(self.frameList[self.activeFrame][2])
            if self.activeFrame >= currentRow:
                self.activeFrame += 1
        newFrameData[3] = 0  # don't add the loop tag
        vStep.set('8')
        vDelay.set(0)

        self.frameList.insert(currentRow, [currentRow, singleFrame, newFrameData])
        self.changeButtonState(currentRow)
        singleFrame.grid(row=currentRow + 1, column=0)

    def delFrame(self, currentRow):
        self.frameList[currentRow][1].destroy()
        del self.frameList[currentRow]
        self.updateButtonCommand(currentRow, -1)
        if self.activeFrame == currentRow:
            if currentRow > 0:
                self.setFrame(self.activeFrame - 1)
            elif self.totalFrame > self.activeFrame:
                self.activeFrame += 1
                self.setFrame(self.activeFrame - 1)
        elif self.activeFrame > currentRow:
            #        if self.activeFrame >= currentRow:
            self.activeFrame -= 1
        if self.frameList == []:
            self.scrollable_frame.update()
            time.sleep(0.5)
            self.restartSkillEditor()

    def getWidget(self, row, idx):
        frame = self.frameList[row]
        widgets = frame[1].winfo_children()
        return widgets[idx]

    def updateButtonCommand(self, currentRow, shift):
        for f in range(currentRow, len(self.frameList)):
            frame = self.frameList[f]
            frame[0] += shift
            widgets = frame[1].winfo_children()
            #            widgets[cLabel].config(text = str(frame[0])) #set
            widgets[cLoop].config(text=str(frame[0]), command=lambda idx=frame[0]: self.setCheckBox(idx))
            widgets[cSet].config(command=lambda idx=frame[0]: self.setFrame(idx))  # set
            widgets[cDel].config(command=lambda idx=frame[0]: self.delFrame(idx))  # delete
            widgets[cAdd].config(command=lambda idx=frame[0]: self.addFrame(idx + 1))  # add
            frame[1].grid(row=frame[0] + 1)
        self.totalFrame += shift

    def changeButtonState(self, currentRow):
        if self.totalFrame > 0:
            self.getWidget(currentRow, cSet).config(text='=',  # +txt('Set')
                                                    font='sans 14 bold', fg='blue')
            if currentRow != self.activeFrame:
                if 0 <= self.activeFrame < self.totalFrame:
                    self.getWidget(self.activeFrame, cSet).config(text='=',  # +txt('Set')
                                                                  font='sans 12', fg='blue')
                self.activeFrame = currentRow
            self.originalAngle[0] = 0

    def transformToFrame(self, f):
        frame = self.frameList[f]

        indexedList = list()
        for i in range(16):
            if self.frameData[4 + i] != frame[2][4 + i]:
                indexedList += [i, frame[2][4 + i]]
        self.frameData = copy.deepcopy(frame[2])
        self.updateSliders(self.frameData)
        self.changeButtonState(f)

        if len(indexedList) > 10:
            send(ports, ['L', self.frameData[4:20], 0.05])
        elif len(indexedList):
            send(ports, ['I', indexedList, 0.05])

    def setFrame(self, currentRow):
        frame = self.frameList[currentRow]
        if currentRow != self.activeFrame:
            self.transformToFrame(currentRow)
            self.frameController.update()

        else:
            for i in range(20):
                if frame[2][4 + i] != self.frameData[4 + i]:  # the joint that's changed
                    for f in range(currentRow + 1, self.totalFrame):
                        frame1 = self.frameList[f - 1]
                        frame2 = self.frameList[f]
                        if frame1[2][4 + i] == frame2[2][4 + i]:  # carry over to the next frame
                            frame2[2][4 + i] = self.frameData[4 + i]
                        else:
                            break
            #                frame[2][4+i] = self.frameData[4+i]
            frame[2][4:] = copy.deepcopy(self.frameData[4:])

            self.getWidget(currentRow, cSet).config(text='=',  # +txt('Set')
                                                    font='sans 14 bold', fg='blue')
        if self.totalFrame == 1:
            self.activeFrame = 0

    def closePop(self, popWin):
        popWin.destroy()

    def insert_val(self, e):
        e.insert(0, 'Hello World!')

    def clearSkillText(self):
        self.skillText.delete('1.0', 'end')

    def openFile(self, top):
        print('open')
        file = askopenfilename()
        if file:
            print(file)
            with open(file, 'r', encoding="utf-8") as f:
                self.clearSkillText()
                self.skillText.insert('1.0', f.read())
        top.after(1, lambda: top.focus_force())

    def loadSkillDataText(self, top):
        skillDataString = self.skillText.get('1.0', 'end')
        if len(skillDataString) == 1:
            messagebox.showwarning(title='Warning', message='Empty input!')
            print('Empty input!')
            top.after(1, lambda: top.focus_force())
            return
        self.restartSkillEditor()
        skillDataString = ''.join(skillDataString.split()).split('{')[1].split('}')[0].split(',')
        if skillDataString[-1] == '':
            skillDataString = skillDataString[:-1]
        skillData = list(map(int, skillDataString))
        print(skillData)

        if skillData[0] < 0:
            header = 7
            frameSize = 20
            loopFrom, loopTo, repeat = skillData[4:7]
            self.vRepeat.set(repeat)
            copyFrom = 4
            self.gaitOrBehavior.set(txt('Behavior'))
        else:
            header = 4
            if skillData[0] == 1:  # posture
                frameSize = 16
                copyFrom = 4
            else:  # gait
                if self.model == 'Nybble' or 'Bittle':
                    frameSize = 8
                    copyFrom = 12
                else:
                    frameSize = 12
                    copyFrom = 8
            self.gaitOrBehavior.set(txt('Gait'))
        if (len(skillData) - header) % abs(skillData[0]) != 0 or frameSize != (len(skillData) - header) // abs(
                skillData[0]):
            messagebox.showwarning(title='Warning', message='Wrong format!')
            print('Wrong format!')
            top.after(1, lambda: top.focus_force())
            return
        top.destroy()

        for f in range(abs(skillData[0])):
            if f != 0:
                self.addFrame(f)
            frame = self.frameList[f]
            frame[2][copyFrom:copyFrom + frameSize] = copy.deepcopy(
                skillData[header + frameSize * f:header + frameSize * (f + 1)])
            if skillData[3] > 1:
                frame[2][4:20] = list(map(lambda x: x * 2, frame[2][4:20]))
                print(frame[2][4:24])

            if skillData[0] < 0:
                if f == loopFrom or f == loopTo:
                    self.getWidget(f, cLoop).select()
                    frame[2][3] = 1
                else:
                    frame[2][3] = 0
                #                    print(self.getWidget(f, cLoop).get())
                self.getWidget(f, cStep).delete(0, END)
                if frame[2][20] == 0:
                    self.getWidget(f, cStep).insert(0, txt('max'))
                else:
                    self.getWidget(f, cStep).insert(0, frame[2][20])
                self.getWidget(f, cDelay).delete(0, END)
                self.getWidget(f, cDelay).insert(0, frame[2][21] * 50)

                self.getWidget(f, cTrig).delete(0, END)
                self.getWidget(f, cAngle).delete(0, END)
                self.getWidget(f, cTrig).insert(0, triggerAxis[frame[2][22]])
                self.getWidget(f, cAngle).insert(0, frame[2][23])

            else:
                self.getWidget(f, cStep).delete(0, END)
                self.getWidget(f, cStep).insert(0, txt('max'))
            self.activeFrame = f
        if self.totalFrame == 1:
            self.activeFrame = -1
        self.setFrame(0)
    def loadSkill(self,skillData):
        
        print(skillData)
        self.restartSkillEditor()
        if skillData[0] < 0:
            header = 7
            frameSize = 20
            loopFrom, loopTo, repeat = skillData[4:7]
            self.vRepeat.set(repeat)
            copyFrom = 4
            self.gaitOrBehavior.set(txt('Behavior'))
        else:
            header = 4
            if skillData[0] == 1:  # posture
                frameSize = 16
                copyFrom = 4
            else:  # gait
                if self.model == 'Nybble' or 'Bittle':
                    frameSize = 8
                    copyFrom = 12
                else:
                    frameSize = 12
                    copyFrom = 8
            self.gaitOrBehavior.set(txt('Gait'))
        if (len(skillData) - header) % abs(skillData[0]) != 0 or frameSize != (len(skillData) - header) // abs(
                skillData[0]):
            messagebox.showwarning(title='Warning', message='Wrong format!')
            print('Wrong format!')
            
            return
        
        for f in range(abs(skillData[0])):
            if f != 0:
                self.addFrame(f)
            frame = self.frameList[f]
            frame[2][copyFrom:copyFrom + frameSize] = copy.deepcopy(
                skillData[header + frameSize * f:header + frameSize * (f + 1)])
            if skillData[3] > 1:
                frame[2][4:20] = list(map(lambda x: x * 2, frame[2][4:20]))
                print(frame[2][4:24])

            if skillData[0] < 0:
                if f == loopFrom or f == loopTo:
                    self.getWidget(f, cLoop).select()
                    frame[2][3] = 1
                else:
                    frame[2][3] = 0
                #                    print(self.getWidget(f, cLoop).get())
                self.getWidget(f, cStep).delete(0, END)
                if frame[2][20] == 0:
                    self.getWidget(f, cStep).insert(0, txt('max'))
                else:
                    self.getWidget(f, cStep).insert(0, frame[2][20])
                self.getWidget(f, cDelay).delete(0, END)
                self.getWidget(f, cDelay).insert(0, frame[2][21] * 50)

                self.getWidget(f, cTrig).delete(0, END)
                self.getWidget(f, cAngle).delete(0, END)
                self.getWidget(f, cTrig).insert(0, txt(triggerAxis[frame[2][22]]))
                self.getWidget(f, cAngle).insert(0, frame[2][23])

            else:
                self.getWidget(f, cStep).delete(0, END)
                self.getWidget(f, cStep).insert(0, txt('max'))
            self.activeFrame = f
        if self.totalFrame == 1:
            self.activeFrame = -1
        self.setFrame(0)
        
    def loadSkillDataTextMul(self,top):
        skillDataString = self.skillText.get('1.0', 'end')
        if len(skillDataString) == 1:
            messagebox.showwarning(title='Warning', message='Empty input!')
            print('Empty input!')
            top.after(1, lambda: top.focus_force())
            return
        self.restartSkillEditor()
        skillNames = re.findall(r"int8_t\ .*\[\]",skillDataString)
        skillNames = [st[7:-2] for st in skillNames]
        skills = re.findall(r"\{[-\s\d\W,]+\}",skillDataString)
        def processing(st):
            lst = st[1:-1].split(',')
            if lst[-1] == '' or lst[-1].isspace():
                lst = lst[:-1]
            lst = list(map(int, [l.strip() for l in lst]))
            return lst
        self.skills = list(map(processing, skills))
        if len(self.skills) ==1:
            top.destroy()
            self.loadSkill(self.skills[0])
        else:
            assert len(skillNames) == len(self.skills)
            self.skillDic = dict(zip(skillNames,range(len(skillNames))))
            self.skillN = ([],[],[])
            for (n,s) in zip(skillNames,range(len(skillNames))):
                if self.skills[s][0] < 0:
                    self.skillN[2].append(n)
                elif self.skills[s][0] > 1:
                    self.skillN[1].append(n)
                else:
                    self.skillN[0].append(n)
            print(self.skillN)
            top.destroy()
            self.comboTop = Toplevel(self.window)
            self.comboTop.title(txt("Skill List"))
            typeLabel = Label(self.comboTop,text = txt("Type of skill"))
            typeLabel.grid(row=1,column=0)
            nameLabel = Label(self.comboTop,text = txt("Name of skill"))
            nameLabel.grid(row=1,column=1)
            values = []
            if len(self.skillN[0]):
                values.append(txt("Posture"))
            if len(self.skillN[1]):
                values.append(txt("Gait"))
            if len(self.skillN[2]):
                values.append(txt("Behavior"))
            self.typeComb = ttk.Combobox(self.comboTop,values=values,state='readonly')
            self.typeComb.grid(row=2, column=0)
            self.nameComb = ttk.Combobox(self.comboTop,values=[])
            self.nameComb.grid(row=2, column=1)
            def selectTy(event):
                V = self.typeComb.get()
                self.nameComb.set('')
                if V==txt("Posture"):
                    self.nameComb['values'] = self.skillN[0]
                elif V==txt("Gait"):
                    self.nameComb['values'] = self.skillN[1]
                else:
                    self.nameComb['values'] = self.skillN[2]
            def select():
                v = self.nameComb.get()
                print(v)
                if v=='':
                    print("No option selected")
                    return
                self.loadSkill(self.skills[self.skillDic[v]])
            
            self.typeComb.bind('<<ComboboxSelected>>',selectTy)
            Button(self.comboTop, text=txt('Cancel'), width=10, command=lambda: self.closePop(self.comboTop)).grid(row=3, column=1)
            Button(self.comboTop, text=txt('OK'), width=10, command=select).grid(row=3, column=0)
    def popImport(self):
        # Create a Toplevel window
        top = Toplevel(self.window)
        # top.geometry('+20+20')
        
        entryFrame = Frame(top)
        entryFrame.grid(row=1, column=0, columnspan=4, padx=10, pady=10)
        self.skillText = Text(entryFrame, width=120, spacing1=2)
        self.skillText.insert('1.0', txt('exampleFormat')
                              + '\n\nconst int8_t hi[] PROGMEM ={\n\
            -5,  0,   0, 1,\n\
             1,  2,   3,\n\
             0,-20, -60,   0,   0,   0,   0,   0,  35,  30, 120, 105,  75,  60, -40, -30,     4, 2, 0, 0,\n\
            35, -5, -60,   0,   0,   0,   0,   0, -75,  30, 125,  95,  40,  75, -45, -30,    10, 0, 0, 0,\n\
            40,  0, -35,   0,   0,   0,   0,   0, -60,  30, 125,  95,  60,  75, -45, -30,    10, 0, 0, 0,\n\
             0,  0, -45,   0,  -5,  -5,  20,  20,  45,  45, 105, 105,  45,  45, -45, -45,     8, 0, 0, 0,\n\
             0,  0,   0,   0,   0,   0,   0,   0,  30,  30,  30,  30,  30,  30,  30,  30,     5, 0, 0, 0,\n\
        };')
        self.skillText.grid(row=0, column=0)
        # Create an Entry Widget in the Toplevel window
        Button(top, text=txt('Open File'), width=10, command=lambda: self.openFile(top)).grid(row=0, column=0)
        Button(top, text=txt('Clear'), width=10, command=self.clearSkillText).grid(row=0, column=1)
        # Create a Button Widget in the Toplevel Window
        Button(top, text=txt('Cancel'), width=10, command=lambda: self.closePop(top)).grid(row=0, column=2)
#        Button(top, text=txt('OK'), width=10, command=lambda: self.loadSkillDataText(top)).grid(row=0, column=3)
        Button(top, text=txt('OK'), width=10, command=lambda: self.loadSkillDataTextMul(top)).grid(row=0, column=3)
        scrollY = Scrollbar(entryFrame, width=20, orient=VERTICAL)
        scrollY.grid(row=0, column=1, sticky='ns')
        scrollY.config(command=self.skillText.yview)
        self.skillText.config(yscrollcommand=scrollY.set)

        #        scrollX = Scrollbar(entryFrame, width = 20, orient = VERTICAL)
        #        scrollX.grid(row = 1, column = 0, sticky='ew')
        #        scrollX.config(command=self.skillText.xview)
        #        self.skillText.config(xscrollcommand=scrollX.set)
        entryFrame.columnconfigure(0, weight=1)
        entryFrame.rowconfigure(0, weight=1)
    
    def changeColor(self,i):
        colorTuple = askcolor(title="Tkinter Color Chooser")
        logger.debug(f"colorTuple: {colorTuple}")
        if (colorTuple[0] is not None) and (colorTuple[1] is not None):
            colors = list(colorTuple[0])
            self.colorHex = colorTuple[1]
    #        for c in range(3):
    #            colors[c] //= 2    #it's not always returning interger.
    #        colors = list(map(lambda x:int(x//2),colors)) #colors have to be integer
    #        printH('RGB: ',colors)
            if self.colorBinderValue.get():
                self.activeEye = 0
                self.eyeColors[0]=colors
                for c in range(2):
                    self.canvasFace.itemconfig(self.eyes[c], fill=self.colorHex)
                    self.eyeColors[c+1] = colors
                    self.eyeBtn[c].config(text = str(colors))
                send(ports, ['C', colors+[0,3], 0])
            else:
                self.activeEye = i
                self.canvasFace.itemconfig(self.eyes[i], fill=self.colorHex)
                self.eyeColors[i+1] = colors
                self.eyeBtn[i].config(text = str(colors))
                send(ports, ['C', colors+[i+1,3], 0])

    def changeEffect(self,e):
        if self.colorBinderValue.get():
            colors = self.eyeColors[self.activeEye+1]
            self.activeEye = 0
            self.eyeColors[0]=colors
            for c in range(2):
                self.canvasFace.itemconfig(self.eyes[c], fill=self.colorHex)
                self.eyeColors[c+1] = colors
                self.eyeBtn[c].config(text = str(colors))
            send(ports, ['C', colors+[0, e], 0])
        else:
            colors = self.eyeColors[self.activeEye+1]
            self.eyeBtn[self.activeEye].config(text = str(colors))
            send(ports, ['C', colors+[self.activeEye+1, e], 0])
        
    def popEyeColor(self):
        #E_RGB_ALL = 0
        #E_RGB_RIGHT = 1
        #E_RGB_LEFT = 2
        #
        #E_EFFECT_BREATHING = 0
        #E_EFFECT_ROTATE = 1
        #E_EFFECT_FLASH = 2
        #E_EFFECT_NONE = -1
        ledEffects = ['Breath','Rotate','Flash']
        effectDictionary = {
            'Breath':0,
            'Rotate':1,
            'Flash':2,
            }
        dia = 100
        crd = [10,10]
        gap = 40
        btShift = [100,25]
        width = dia*2 + gap + 2*crd[0]
        self.eyeColors = [[0,0,0],[0,0,0],[0,0,0]]
        self.activeEye = 0
        topEye = Toplevel(self.window)
        topEye.title('Eye Color Setter')
        topEye.geometry(str(width)+'x170+400+200')
        face = Frame(topEye)
        face.grid(row = 0,column = 0)
        self.canvasFace = Canvas(face,height=120)
        self.canvasFace.grid(row = 0,column = 0, columnspan = 2)
        eyeR = self.canvasFace.create_oval(crd[0], crd[1], crd[0]+dia, crd[1]+dia, outline="#000",
                    fill="#606060", width=2)
        eyeL = self.canvasFace.create_oval(crd[0]+dia+gap, crd[1], crd[0]+2*dia+gap, crd[1]+dia, outline="#000",
                    fill="#606060", width=2)
        self.eyes = [eyeR,eyeL]
        btR = Button(face,text=str(self.eyeColors[1]),width = 7, command=lambda:self.changeColor(0))
        btR.place(x=crd[0]+dia/2-btShift[0]/2,y=crd[1]+dia/2-btShift[1]/2)
        btL = Button(face,text=str(self.eyeColors[2]),width = 7, command=lambda:self.changeColor(1))
        btL.place(x=crd[0]+dia*3/2+gap-btShift[0]/2,y=crd[0]+dia/2-btShift[1]/2)
        self.eyeBtn = [btR,btL]
        self.colorBinderValue = BooleanVar()
        colorBinder = Checkbutton(face, text='<>', indicator=0, width=3,
                                         variable=self.colorBinderValue,onvalue=True, offvalue=False)
        colorBinder.place(x=crd[0]+dia+5,y=crd[1]+dia/2-btShift[1]/2)
        
        btnsEff = Frame(face)
        btnsEff.grid(row = 1,column = 0)
        if self.OSname == 'win32':
            wValue = 5
        else:
            wValue = 3
        for e in range(len(effectDictionary)):
            Button(btnsEff,text=txt(list(effectDictionary.keys())[e]),width = wValue,command = lambda eff=list(effectDictionary.values())[e]:self.changeEffect(eff)).grid(row = 0,column = e)
        Button(btnsEff,text=txt('Meow'),width = wValue,command = lambda :send(ports, ['u', 0])).grid(row = 0,column = 3)


    def playThread(self):
        self.playStop = False
        self.buttonPlay.config(text=txt('Stop'), fg='red', command=self.stop)
        t = threading.Thread(target=self.play)
        t.start()

    def play(self):
        if self.activeFrame + 1 == self.totalFrame:
            self.getWidget(self.activeFrame, cSet).config(text='=',  # +txt('Set')
                                                          font='sans 12')
            self.activeFrame = 0
        for f in range(self.activeFrame, self.totalFrame):
            if self.playStop:
                break

            self.transformToFrame(f)

        self.buttonPlay.config(text=txt('Play'), fg='green', command=self.playThread)
        self.playStop = False

    def stop(self):
        self.buttonPlay.config(text=txt('Play'), fg='green', command=self.playThread)
        self.playStop = True

    def mirrorAngles(self, singleFrame):
        singleFrame[1] = -singleFrame[1]
        singleFrame[4] = -singleFrame[4]
        singleFrame[4 + 2] = -singleFrame[4 + 2]
        for i in range(4, 16, 2):
            singleFrame[4 + i], singleFrame[4 + i + 1] = singleFrame[4 + i + 1], singleFrame[4 + i]
        if abs(singleFrame[22]) == 2:
            singleFrame[22] = -singleFrame[22]
            singleFrame[23] = -singleFrame[23]

    def generateMirrorFrame(self):
        self.values[16 + 2].set(-1 * self.values[16 + 2].get())
        self.values[16 + 5].set(-1 * self.values[16 + 5].get())
        self.mirrorAngles(self.originalAngle)
        self.mirrorAngles(self.frameData)
        self.updateSliders(self.frameData)
        self.indicateEdit()
        self.frameController.update()
        send(ports, ['L', self.frameData[4:20], 0.05])
        
    def popCreator(self):
        self.creatorWin = Toplevel(self.window)
        self.creatorWin.title(txt("Creator Information"))
        self.creatorWin.geometry('216x110+500+400')

        fmCreInfo = ttk.Frame(self.creatorWin)    # relief=GROOVE to draw border
        fmCreInfo.grid(ipadx=3, ipady=3, padx=3, pady=5, sticky=W + E)

        # creator label and entry
        creator_label = Label(fmCreInfo, text=txt('Creator'))
        creator_label.grid(row=0, column=0, padx=2, pady=6, sticky=W)
        self.creator_entry = Entry(fmCreInfo, textvariable=self.creator, font=('Arial', 10), foreground='blue', background='white')
        self.creator_entry.grid(row=0, column=1,padx=2, pady=6, sticky=W)

        # location label and entry
        location_label = Label(fmCreInfo, text=txt('Location'))
        location_label.grid(row=1, column=0, padx=2, pady=3, sticky=W)
        self.location_entry = Entry(fmCreInfo, textvariable=self.location, font=('Arial', 10), foreground='blue', background='white')
        self.location_entry.grid(row=1, column=1, padx=2, pady=3, sticky=W)

        fmCreInfo.columnconfigure(0, weight=1)  # set column width
        fmCreInfo.columnconfigure(1, weight=3)  # set column width

        # saveID button
        saveID_button = Button(fmCreInfo, text=txt('Save'), command=self.saveID)
        saveID_button.grid(row=2, columnspan=2, padx=3, pady=6, sticky=W + E)

        self.creatorWin.protocol('WM_DELETE_WINDOW', self.saveID)
        
    def saveID(self):
        creatorValue = self.creator_entry.get()
        locationValue = self.location_entry.get()
        if creatorValue == '':
            messagebox.showwarning(txt('Warning'), txt('InputCreator'))
            self.creatorWin.after(1, lambda: self.creatorWin.focus_force())
            self.creator_entry.focus()  # force the entry to get focus
            return False
        else:
            self.creator.set(creatorValue)

        if locationValue == '':
            messagebox.showwarning(txt('Warning'), txt('InputLocation'))
            self.creatorWin.after(1, lambda: self.creatorWin.focus_force())
            self.location_entry.focus()  # force the entry to get focus
            return False
        else:
            self.location.set(locationValue)

        print("Creator:", self.creator.get())
        print("Location:", self.location.get())

        self.configuration = self.configuration[:6] + [self.creator.get(), self.location.get()]

        self.saveConfigToFile(defaultConfPath)
        self.creatorInfoAcquired = True
        logger.debug(f"saveID, self.creatorInfoAcquired: {self.creatorInfoAcquired}")
        self.creatorWin.destroy()

        
    def saveConfigToFile(self, filename):
        self.configuration = [self.defaultLan, self.model, self.defaultPath, self.defaultSwVer, self.defaultBdVer,
                                  self.defaultMode, self.configuration[6], self.configuration[7]]

        f = open(filename, 'w+', encoding="utf-8")
        logger.debug(f"config: {self.configuration}")
        lines = '\n'.join(self.configuration) + '\n'
        f.writelines(lines)
        time.sleep(0.1)
        f.close()

    def getCreatorInfo(self, modifyQ):
        # self.creatorInfoAcquired = True
        try:
            with open(defaultConfPath, "r", encoding="utf-8") as f:
                lines = f.readlines()
                # f.close()
            lines = [line.split('\n')[0] for line in lines]    # remove the '\n' at the end of each line
            defaultLan = self.defaultLan
            defaultModel = self.model
            defaultPath = lines[2]
            defaultSwVer = lines[3]
            defaultBdVer = lines[4]
            defaultMode = lines[5]

            if len(lines) >= 8:
                self.creator.set(lines[6])
                self.location.set(lines[7])
                self.creatorInfoAcquired = True
                self.configuration = [defaultLan, defaultModel, defaultPath, defaultSwVer, defaultBdVer, defaultMode,
                                      lines[6], lines[7]]
                logger.debug(f"getCreatorInfo, self.creatorInfoAcquired: {self.creatorInfoAcquired}")
                if modifyQ is True:
                    self.popCreator()
            else:
                self.creator.set(self.defaultCreator)
                self.location.set(self.defaultLocation)
                self.creatorInfoAcquired = False
                self.configuration = [defaultLan, defaultModel, defaultPath, defaultSwVer, defaultBdVer, defaultMode,
                                      self.defaultCreator, self.defaultLocation]
                self.popCreator()
                
        except Exception as e:
            print(e)
            print('Create configuration file')
            defaultLan = self.defaultLan
            defaultModel = self.model
            defaultPath = releasePath[:-1]
            defaultSwVer = '2.0'
            defaultBdVer = NyBoard_version
            defaultMode = 'Standard'
            self.configuration = [defaultLan, defaultModel, defaultPath, defaultSwVer, defaultBdVer, defaultMode,
                                  self.defaultCreator, self.defaultLocation]
            self.creatorInfoAcquired = False
            self.popCreator()
        
    def export(self):
        self.getCreatorInfo(False)
        logger.debug(f"export, self.creatorInfoAcquired: {self.creatorInfoAcquired}")
        if not self.creatorInfoAcquired:
            return  # to avoid a bug that the creator window won't open until the file saver is cloesd

        logger.info(f"Creator: {self.creator.get()}")
        logger.info(f"Location: {self.location.get()}")
        files = [('Text Document', '*.md'),
                 ('Python Files', '*.py'),
                 ('All Files', '*.*'),
                 ]
        file = asksaveasfile(filetypes=files, defaultextension='.md')

        if self.activeFrame + 1 == self.totalFrame:
            self.getWidget(self.activeFrame, cSet).config(text='=',  # +txt('Set')
                                                          font='sans 12')
            self.window.update()
            self.activeFrame = 0
        skillData = list()
        loopStructure = list()
        period = self.totalFrame - self.activeFrame
        if self.model == 'Nybble' or self.model == 'Bittle':
            copyFrom = 12
            frameSize = 8
        else:
            copyFrom = 8
            frameSize = 12
        if self.gaitOrBehavior.get() == txt('Behavior'):
            period = -period
            copyFrom = 4
            frameSize = 20
        if self.totalFrame == 1:
            period = 1
            copyFrom = 4
            frameSize = 16
        angleRatio = 1
        startFrame = self.activeFrame
        inv_triggerAxis = {txt(v): k for k, v in triggerAxis.items()}
        for f in range(startFrame, self.totalFrame):
            frame = self.frameList[f]
            self.frameData = copy.deepcopy(frame[2])
            if max(self.frameData[4:20]) > 125 or min(self.frameData[4:20]) < -125:
                angleRatio = 2
            if self.frameData[3] == 1:
                loopStructure.append(f - startFrame)
            if self.getWidget(f, cStep).get() == txt('max') or int(self.getWidget(f, cStep).get())>127:
                self.frameData[20] = 0
            else:
                self.frameData[20] = int(self.getWidget(f, cStep).get())
            self.frameData[21] = max(min(int(self.getWidget(f, cDelay).get()) // 50,127),0)
            self.frameData[22] = int(inv_triggerAxis[self.getWidget(f, cTrig).get()])
            self.frameData[23] = max(min(int(self.getWidget(f, cAngle).get()),127),-128)
            if self.mirror:
                self.mirrorAngles(self.frameData)
            self.updateSliders(self.frameData)
            self.changeButtonState(f)
            self.frameController.update()
            skillData.append(self.frameData[copyFrom: copyFrom + frameSize])
        if period == 1:
            print(self.frameData[4:20])
            send(ports, ['L', self.frameData[4:20], 0.05])
            return
        if angleRatio == 2:
            for r in skillData:
                if frameSize == 8 or frameSize == 12:
                    r = list(map(lambda x: x // angleRatio, r))
                elif frameSize == 20:
                    r[:16] = list(map(lambda x: x // angleRatio, r[:16]))
        if len(loopStructure) < 2:
            loopStructure = [0,0]
        if len(loopStructure) > 2:
            for l in range(1, len(loopStructure) - 1):
                f = loopStructure[l] + startFrame
                frame = self.frameList[f]
                frame[2][3] = 0
                self.getWidget(f, cLoop).deselect()
            self.frameRowScheduler.update()

        print('{')
        print('{:>4},{:>4},{:>4},{:>4},'.format(*[period, 0, 0, angleRatio]))
        if period < 0 and self.gaitOrBehavior.get() == txt('Behavior'):
            print('{:>4},{:>4},{:>4},'.format(*[loopStructure[0], loopStructure[-1], self.loopRepeat.get()]))
        for row in skillData:
            print(('{:>4},' * frameSize).format(*row))
        print('};')

        if file:
#            print(file.name)
            x = datetime.datetime.now()
            fileData = '# ' + file.name.split('/')[-1].split('.')[0] + '\n'
            fileData += 'Note: '+'You may add a short description/instruction here.\n\n'
            fileData += 'Model: ' + self.model + '\n\n'
            fileData += 'Creator: ' + self.creator.get() + '\n\n'
            fileData += 'Location: ' + self.location.get() + '\n\n'
            fileData += 'Date: ' + x.strftime("%b")+' '+x.strftime("%d")+', '+x.strftime("%Y") + '\n\n'
            fileData += '# [Demo](www.youtube.com) You can modify the link in the round brackets\n\n'
            fileData += '# Token\nK\n\n'
            fileData += '# Data\n{\n' + '{:>4},{:>4},{:>4},{:>4},\n'.format(*[period, 0, 0, angleRatio])
            if period < 0 and self.gaitOrBehavior.get() == txt('Behavior'):
                fileData += '{:>4},{:>4},{:>4},\n'.format(*[loopStructure[0], loopStructure[-1], self.loopRepeat.get()])
            logger.debug(f"skillData: {skillData}")
            for row in skillData:
                logger.debug(f"row: {row}")
                logger.debug(f"frameSize: {frameSize}")
                fileData += ('{:>4},' * frameSize).format(*row)
                fileData += '\n'
            fileData += '};'

            # the file in the config directory will be saved automatically
            filePathName = configDir + separation + 'SkillLibrary' + separation + self.model + separation + file.name.split('/')[-1]
            logger.debug(f"fileName is: {filePathName}")

            filePathList = [file.name, filePathName]
            for filePath in filePathList:
                if filePath == filePathName:
                    modelDir = configDir + separation + 'SkillLibrary' + separation + self.model
                    makeDirectory(modelDir)
                try:
                    with open(filePath, 'w+', encoding="utf-8") as f:
                        f.write(fileData)
                        time.sleep(0.1)
                    logger.info(f"save successfully: {filePath}")
                except Exception as e:
                    logger.info(f"save failed:{e}")
                    return False

        if self.gaitOrBehavior.get() == txt('Behavior'):
            skillData.insert(0, [loopStructure[0], loopStructure[-1], int(self.loopRepeat.get())])
        skillData.insert(0, [period, 0, 0, angleRatio])
        flat_list = [item for sublist in skillData for item in sublist]
        print(flat_list)

        send(ports, ['i', 0.1])
        res = send(ports, ['K', flat_list, 0], 0)
        print(res)

    def restartSkillEditor(self):
        for f in self.frameList:
            f[1].destroy()
        self.frameList.clear()
        self.frameData = [0, 0, 0, 0,
                          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                          8, 0, 0, 0, ]
        self.totalFrame = 0
        self.activeFrame = 0
        self.addFrame(0)
        self.vRepeat.set(0)

    #        self.window.update()
    #        self.setPose('calib')

    def indicateEdit(self):
        frame = self.frameList[self.activeFrame]
        if frame[2] != self.frameData:
            self.getWidget(self.activeFrame, cSet).config(text='!'  # + txt('Save')
                                                          , font='sans 14 bold', fg='red')
        #            print('frm',frame[2])
        #            print('dat',self.frameData)
        else:
            self.getWidget(self.activeFrame, cSet).config(text='=',  # +txt('Set')
                                                          font='sans 14 bold', fg='blue')

    def setCheckBox(self, currentRow):
        frame = self.frameList[currentRow]
        if frame[2][3] == 0:
            frame[2][3] = 1
        else:
            frame[2][3] = 0

    def unbindAll(self):
        for i in range(16):
            self.binderValue[i].set(0)
            self.previousBinderValue[i] = 0
            self.changeRadioColor(i, 0)
        self.controllerLabels[1].config(fg='blue')

    def changeRadioColor(self, joint, value):  # -1, 0, 1
        if value:
            self.binderButton[joint * 2 + (1 - value) // 2].configure(background='red')
            self.binderButton[joint * 2 + (value + 1) // 2].configure(background='light blue')
        else:
            self.binderButton[joint * 2].configure(background='light blue')
            self.binderButton[joint * 2 + 1].configure(background='light blue')
        self.binderButton[joint * 2].update()
        self.binderButton[joint * 2 + 1].update()
        if 1 in self.previousBinderValue or -1 in self.previousBinderValue:
            self.controllerLabels[1].config(fg='red')
        else:
            self.controllerLabels[1].config(fg='blue')

    def updateRadio(self, joint):
        if self.previousBinderValue[joint] == self.binderValue[joint].get():
            self.binderValue[joint].set(0)
        self.previousBinderValue[joint] = self.binderValue[joint].get()
        self.changeRadioColor(joint, self.binderValue[joint].get())

    def setAngle(self, idx, value):
        if self.ready == 1:
            value = int(value)
            if self.binderValue[idx].get() == 0:
                self.frameData[4 + idx] = value
                if -126 < value < 126:
                    send(ports, ['I', [idx, value], 0.05])
                else:
                    send(ports, ['i', [idx, value], 0.05])
            else:
                diff = value - self.frameData[4 + idx]
                indexedList = list()
                for i in range(16):
                    if self.binderValue[i].get():
                        self.frameData[4 + i] += diff * self.binderValue[i].get() * self.binderValue[idx].get()
                        indexedList += [i, self.frameData[4 + i]]
                        
                if len(indexedList) > 10:
                    send(ports, ['L', self.frameData[4:20], 0.05])
                elif len(indexedList):
                    send(ports, ['I', indexedList, 0.05])

            self.indicateEdit()
            self.updateSliders(self.frameData)

    def set6Axis(self, i, value):  # a more precise function should be based on inverse kinematics
        value = int(value)
        if self.ready == 1:
            #            send(ports, ['t', [i, value], 0.0])
            if self.originalAngle[0] == 0:
                self.originalAngle[4:20] = copy.deepcopy(self.frameData[4:20])
                self.originalAngle[0] = 1
            positiveGroup = []
            negativeGroup = []
            if i == 0:  # ypr
                positiveGroup = []
                negativeGroup = []
            elif i == 1:  # pitch
                if jointConfig[self.model] == '>>':
                    positiveGroup = [1, 4, 5, 8, 9, 14, 15]
                    negativeGroup = [2, 6, 7, 10, 11, 12, 13]
                else:
                    positiveGroup = [1, 4, 5, 8, 9, 10, 11,]
                    negativeGroup = [6, 7, 12, 13, 14, 15]
            elif i == 2:  # roll
                if jointConfig[self.model] == '>>':
                    positiveGroup = [4, 7, 8, 11, 13, 14]
                    negativeGroup = [0, 5, 6, 9, 10, 12, 15]
                else:
                    positiveGroup = [4, 7, 8, 10, 13, 15]
                    negativeGroup = [0, 2, 5, 6, 9, 11, 12, 14]
            elif i == 3:  # Spinal
                if jointConfig[self.model] == '>>':
                    positiveGroup = [8, 9, 10, 11, 12, 13, 14, 15]
                    negativeGroup = []
                else:
                    positiveGroup = [8, 9, 10, 11, 12, 13, 14, 15]
                    negativeGroup = []

            elif i == 4:  # Height
                if jointConfig[self.model] == '>>':
                    positiveGroup = [12, 13, 14, 15]
                    negativeGroup = [8, 9, 10, 11, ]
                else:
                    positiveGroup = [12, 13, 10, 11, ]
                    negativeGroup = [8, 9, 14, 15]
            elif i == 5:  # Sideway
                if jointConfig[self.model] == '>>':
                    positiveGroup = []
                    negativeGroup = []

            for j in range(16):
                leftQ = (j - 1) % 4 > 1
                frontQ = j % 4 < 2
                upperQ = j / 4 < 3
                factor = 1
                if j > 3:
                    if not upperQ:
                        factor = 2
                    else:
                        factor = 1
                    if i == 1:
                        if jointConfig[self.model] == '>>':
                            if upperQ:
                                if frontQ:
                                    if value < 0:
                                        factor = 0
                                else:
                                    factor *= 2
                                    
                        if jointConfig[self.model] == '><':
                            if upperQ:
                                factor /= 3
                            
                    if i == 2:
                        if (value > 0 and not leftQ) or (value < 0 and leftQ):
                            factor /= 2

                if j in positiveGroup:
                    self.frameData[4 + j] = self.originalAngle[4 + j] + int(value * factor)
                if j in negativeGroup:
                    self.frameData[4 + j] = self.originalAngle[4 + j] - int(value * factor)

            send(ports, ['L', self.frameData[4:20], 0.05])
            self.updateSliders(self.frameData)
            self.indicateEdit()


    def sendCmd(self,event=None):
        if self.ready == 1:
            serialCmd = self.newCmd.get()
            logger.debug(f'serialCmd={serialCmd}')
            if serialCmd != '':
                try:
                    token = serialCmd[0]
                    cmdList = serialCmd[1:].replace(',',' ').split()

                    if len(cmdList) <= 1:
                        send(ports, [serialCmd, 1])
                    else:
                        cmdList = list(map(lambda x:int(x),cmdList))
                        send(ports, [token,cmdList, 1])
                    self.newCmd.set('')
                except Exception as e:
                    logger.info("Exception")
                    print("Illegal input!")
                    raise e

    def setPose(self, pose):
        if self.ready == 1:
            self.getWidget(self.activeFrame, cNote).delete(0, END)
            self.getWidget(self.activeFrame, cNote).insert(0, pose + str(self.activeFrame))
            self.frameData[4:20] = copy.deepcopy(self.postureTable[pose])
            self.originalAngle[0] = 0
            self.updateSliders(self.postureTable[pose])
            self.indicateEdit()
            for i in range(6):
                self.values[16 + i].set(0)
            send(ports,['i',0])
            send(ports, ['k' + pose, 0])
#            if pose == 'rest':
#                send(ports, ['d', 0])

    def setStep(self):
        self.frameData[20] = self.getWidget(self.activeFrame, cStep).get()

    def setDelay(self):
        self.frameData[21] = int(self.getWidget(self.activeFrame, cDelay).get())

    def updateSliders(self, angles):
        for i in range(16):
            self.values[i].set(angles[4 + i])
            self.frameData[4 + i] = angles[4 + i]
    """
    def keepCheckingPort(self, goodPorts):
        allPorts = Communication.Print_Used_Com()
        while self.keepChecking:
            time.sleep(0.01)
            currentPorts = Communication.Print_Used_Com()
            if set(currentPorts) - set(allPorts):
                newPort = list(set(currentPorts) - set(allPorts))
                time.sleep(0.5)
                checkPortList(goodPorts, newPort)
                self.updatePortMenu()
            elif set(allPorts) - set(currentPorts):
                closedPort = list(set(allPorts) - set(currentPorts))
                inv_dict = {v: k for k, v in goodPorts.items()}
                for p in closedPort:
                    if inv_dict.get(p.split('/')[-1], -1) != -1:
                        printH('Removing', p)
                        goodPorts.pop(inv_dict[p.split('/')[-1]])
                self.updatePortMenu()
            allPorts = copy.deepcopy(currentPorts)
    """
    
    def dial(self, i):
        if self.ready == 1:
            global ports
            global goodPorts
            # buttons = self.frameDial.winfo_children()[2:5]
            key = list(dialTable)[i]
            if key == 'Connect':
                if self.keepChecking:
                    send(ports, ['b', [10, 90], 0], 1)
                    closeAllSerial(goodPorts)
                    # self.portMenu.config(state = DISABLED)
                    # self.updatePortMenu()
                    self.keepChecking = False
                    self.frameDial.winfo_children()[1].config(text=txt('Connect'), fg='red')
                    self.dialValue[0].set(False)
                    # for b in buttons:
                    #     b.config(state = DISABLED)
                else:
                    self.dialValue[0].set(True)
                    self.frameDial.winfo_children()[1].update()
                    goodPorts = {}
                    connectPort(goodPorts)
                    
                    printH('***@@@ good ports', goodPorts)
                    #                    self.portMenu.destroy()
                    #                    self.createPortMenu()

                    self.keepChecking = True
                    t = threading.Thread(target=keepCheckingPort, args=(goodPorts,lambda:self.keepChecking,lambda:True,self.updatePortMenu,))
                    t.daemon = True
                    t.start()
                    send(ports, ['b', [10, 90], 0])
                    if len(goodPorts) > 0:
                        self.frameDial.winfo_children()[1].config(text=txt('Connected'), fg='green')
                        # for b in buttons:
                        #     b.config(state = NORMAL)
                    else:
                        self.frameDial.winfo_children()[1].config(text=txt('Listening'), fg='orange')
                # self.frameDial.winfo_children()[1].update()
                self.updatePortMenu()
            elif len(goodPorts) > 0:
                result = send(ports, [dialTable[key], 0])
                if result != -1:
                    state = result[0].replace('\r', '').replace('\n', '')
                    if state == 'k':
                        self.dialValue[i].set(True)
                        self.frameDial.winfo_children()[2].config(fg='green')
                        self.frameDial.winfo_children()[2].select()
                    elif state == 'P':
                        self.dialValue[i].set(False)
                        self.frameDial.winfo_children()[2].config(fg='red')
                        self.frameDial.winfo_children()[2].deselect()
                    elif state == 'g':
                        self.dialValue[i].set(False)
                        self.frameDial.winfo_children()[3].config(fg='red')
                        self.frameDial.winfo_children()[3].deselect()
                    elif state == 'G':
                        self.dialValue[i].set(True)
                        self.frameDial.winfo_children()[3].config(fg='green')
                        self.frameDial.winfo_children()[3].select()
                    elif state == 'z':
                        self.dialValue[i].set(False)
                        self.frameDial.winfo_children()[4].config(fg='red')
                        self.frameDial.winfo_children()[4].deselect()
                    elif state == 'Z':
                        self.dialValue[i].set(True)
                        self.frameDial.winfo_children()[4].config(fg='green')
                        self.frameDial.winfo_children()[4].select()

    def on_closing(self):
        if messagebox.askokcancel(txt('Quit'), txt('Do you want to quit?')):
            self.saveConfigToFile(defaultConfPath)
            self.keepChecking = False  # close the background thread for checking serial port
            self.window.destroy()
            closeAllSerial(goodPorts)
            os._exit(0)
           
if __name__ == '__main__':
    goodPorts = {}
    try:
#        connectPort(goodPorts)
#        ports = goodPorts
        #        time.sleep(2)
        #        if len(goodPorts)>0:
        #            t=threading.Thread(target=keepReadingSerial,args=(goodPorts,))
        #            t.start()
        SkillComposer(model, language)
        closeAllSerial(goodPorts)
        os._exit(0)
    except Exception as e:
        logger.info("Exception")
        closeAllSerial(goodPorts)
        raise e

# unused text codes for references
# import string
#        letters = WORDS#string.ascii_lowercase + string.ascii_uppercase + string.digits
#        vNote.set('note: '+ ''.join(random.choice(letters) for i in range(5)) )#'note')
