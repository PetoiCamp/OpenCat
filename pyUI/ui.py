#!/usr/bin/python3
# -*- coding: UTF-8 -*-

import string
import random
import sys
sys.path.append('../serialMaster/')
from ardSerial import *
from tkinter import *
from tkinter import messagebox
import tkinter.font as tkFont
from PIL import ImageTk, Image
import copy
import threading
from translate import *


width = 8
centerWidth = 2

model = 'Bittle'
postureTable = postureDict[model]

def rgbtohex(r, g, b):
    return f'#{r:02x}{g:02x}{b:02x}'

def printH(head, value):
    print(head,end = ' ')
    print(value)

labelSchedulerHeader = ['Repeat','Loop','Frame', 'Speed', 'Delay(ms)', 'Trig','Angle','Note', 'Del', 'Add']
cLoop,cLabel, cSet, cSpeed, cDelay,cTrig,cAngle, cNote, cDel, cAdd = range(len(labelSchedulerHeader))
frameItemWidth =[2,1,4,3,5,2,4,7,1,1,]

#word_file = '/usr/share/dict/words'
#WORDS = open(word_file).read().splitlines()
animalNames=[
'ant','bat','bear','bee','bird','buffalo','cat','chicken','cow','dog','dolphin','duck','elephant','fish','fox','frog','goose','goat','horse','kangaroo','lion','monkey','owl','ox','penguin','person','pig','rabbit','sheep','tiger','whale','wolf','zebra']
WORDS = animalNames

language = languageList['English']

def txt(key):
    return language.get(key, textEN[key])

class app:

    def __init__(self):
        self.window = Tk()
        self.sliders = list()
        self.values = list()
        self.ready = 0
        # , slant='italic')
        self.myFont = tkFont.Font(
            family='Times New Roman', size=20, weight='bold')
        self.window.title(txt('title'))
        self.window.geometry('+100+10')
        self.frameSize = 16
        self.totalFrame = 0
        self.activeFrame = 0
        self.frameList = list()
        self.frameData = [0,0,0,0,
                            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                            8,0,0,0,]
        self.pause = False
        self.online = True
        self.playStop = False
        self.skill = 'Behavior'
        
        self.createMenu()
        self.createController()
        self.createImage()
        self.createDial()
        self.createPosture()
        self.createScheduler()
        self.createSkillSchedule()
        
        
        if self.online == True:
            flushSerialOutput(300)
        self.ready = 1
        self.window.protocol('WM_DELETE_WINDOW', self.on_closing)
        self.window.update()
        self.window.mainloop()
        
    def createMenu(self):
        self.menubar = Menu(self.window, background='#ff8000', foreground='black', activebackground='white', activeforeground='black')
        file = Menu(self.menubar, tearoff=0, background='#ffcc99', foreground='black')
        file.add_command(label='Nybble',command = lambda:self.changeModel('Nybble'))
        file.add_command(label='Bittle',command = lambda:self.changeModel('Bittle'))
        file.add_command(label='DoF16',command = lambda:self.changeModel('DoF16'))
        file.add_separator()
        file.add_command(label=txt('Exit'), command=self.window.quit)
        self.menubar.add_cascade(label=txt('Model'), menu=file)
        
        lan = Menu(self.menubar, tearoff=0)
        for l in languageList:
            lan.add_command(label=languageList[l]['lanOption'],command = lambda lanChoice = l: self.changLan(lanChoice))
        self.menubar.add_cascade(label=txt('lanMenu'), menu=lan)
        
        help = Menu(self.menubar, tearoff=0)
        help.add_command(label=txt('About'), command=self.about)
        self.menubar.add_cascade(label=txt('Help'), menu=help)

        self.window.config(menu=self.menubar)
        
    def createController(self):
        self.frameController = Frame(self.window)
        self.frameController.grid(row=0, column=0, rowspan=9, ipadx=10)
        labelController = Label(self.frameController,
                                text=txt('Joint Controller'), font=self.myFont)
        labelController.grid(row=0, column=0, columnspan=width)

        self.frameImu = Frame(self.frameController)
        self.frameImu.grid(row=6, column=3, rowspan=1, columnspan=2, ipady=10)

        yprNames = ['yaw', 'pitch', 'roll']
        for i in range(3):
            label = Label(self.frameImu, text=txt(yprNames[i]), bg='Light Blue1')
            value = DoubleVar()
            sliderBar = Scale(self.frameImu, variable=value, orient=HORIZONTAL, borderwidth=2, from_=-60, to=60, length=300, tickinterval=10, resolution=1, command=lambda value, idx=i:  self.setYPR(idx, value))
            sliderBar.set(0)
            label.grid(row=0+i, column=3, columnspan=1)
            sliderBar.grid(row=0+i, column=4, columnspan=centerWidth)
            self.sliders.append(sliderBar)
            self.values.append(value)

        scaleNames = [
            'Head Pan', 'Head Tilt', 'Tail Pan', 'N/A',
            'Shoulder Left Front', 'Shoulder Right Front', 'Shoulder Right Back', 'Shoulder Left Back',
            'Arm Left Front', 'Arm Right Front', 'Arm Right Back', 'Arm Left Back',
            'Knee Left Front', 'Knee Right Front', 'Knee Right Back', 'Knee Left Back']
        NaJoints = {'Nybble': [3, 4, 5, 6, 7],
            'Bittle': [1, 2, 3, 4, 5, 6, 7],
            'DoF16' : []
        }
        for i in range(16):
            PAD = 5
            cSPAN = 1
            if i < 4:
                tickDirection = 1
                cSPAN = 4
                if i < 2:
                    ROW = 0
                else:
                    ROW = 9
                if i > 0 and i < 3:
                    COL = 4
                else:
                    COL = 0
                rSPAN = 1
                ORI = HORIZONTAL
                LEN = 450

            else:
                tickDirection = -1
                leftQ = (i - 1) % 4 > 1
                frontQ = i % 4 < 2
                upperQ = i / 4 < 3

                ROW = (i % 4//2 + 1) * 2
                if leftQ:
                    COL = (1-leftQ) * (1+centerWidth) + 3 - i // 4
                else:
                    COL = (1-leftQ) * (1+centerWidth) + 1 + i // 4
                ORI = VERTICAL
                LEN = 250
                if ROW == 2:
                    PAD = 10

            if i in NaJoints[model]:
                stt = DISABLED
                clr = 'gray'
            else:
                stt = NORMAL
                clr = 'yellow'

            label = Label(self.frameController,
                          text='(' + str(i)+')\n'+txt(scaleNames[i]))
            value = DoubleVar()
            sliderBar = Scale(self.frameController, state=stt, bg=clr, variable=value, orient=ORI, borderwidth=2, from_=-125*tickDirection, to=125*tickDirection, length=LEN, tickinterval=25, resolution=1,
                              command=lambda value, idx=i:  self.setAngle(idx, value))
            sliderBar.set(0)
            label.grid(row=ROW + 1, column=COL, columnspan=cSPAN, pady=2)
            sliderBar.grid(row=ROW+2, column=COL, rowspan=rSPAN, columnspan=cSPAN, ipadx=2, ipady = 2, padx = 2, pady=(0, PAD))
            
            self.sliders.append(sliderBar)
            self.values.append(value)
            
    def createDial(self):
        self.frameDial = Frame(self.window)
        self.frameDial.grid(row=0, column=1, ipady=10)
        labelDial = Label(self.frameDial, text=txt('State Dials'), font=self.myFont)
        labelDial.grid(row=0, column=0, columnspan=4)
        dialTable = {'Connect':'Connect', 'Pause': 'p', 'Gyro': 'g',  'Random': 'z'}
        i = 0

        for key in dialTable:
            if self.online or key == 'Connect':
                dialState = NORMAL
            else:
                dialState = DISABLED
            button = Button(self.frameDial, text=txt(key), width=10, fg='blue', state = dialState, command=lambda k=key: self.dial(dialTable[k]))
            button.grid(row=i//4 + 1, column=i % 4)
            i += 1
    def createPosture(self):
        self.framePosture = Frame(self.window)
        self.framePosture.grid(row=1, column=1, ipady=10)
        labelPosture = Label(self.framePosture, text=txt('Postures'), font=self.myFont)
        labelPosture.grid(row=0, column=0, columnspan=4)
        i = 0
        for pose in postureTable:
            button = Button(self.framePosture, text=pose, fg='blue', width=10, command=lambda p=pose:  self.setPose(p))
            button.grid(row=i//4 + 1, column=i % 4)
            i += 1
            
    def createScheduler(self):
        self.frameScheduler = Frame(self.window)
        self.frameScheduler.grid(row=2, column=1, rowspan=30, ipady=600)
        labelScheduler = Label(self.frameScheduler, text=txt('Scheduler'), font=self.myFont)
        labelScheduler.grid(row=0, column=0, columnspan=4)
        
        self.buttonRun = Button(self.frameScheduler, text=txt('Play'), width=10, fg='green', command=self.playThread)
        self.buttonRun.grid(row=1, column=0)

        buttonExp = Button(self.frameScheduler, text=txt('Import'), width=10, fg='blue', command=self.popImport)
        buttonExp.grid(row=1, column=1)
        
        buttonRestart = Button(self.frameScheduler, text=txt('Restart'), width=10, fg='red', command=self.restartScheduler)
        buttonRestart.grid(row=1, column=2)

        buttonExp = Button(self.frameScheduler, text=txt('Export'), width=10, fg='blue', command=self.export)
        buttonExp.grid(row=1, column=3)
        
        buttonUndo = Button(self.frameScheduler, text=txt('Undo'), width=10, fg='blue', state=DISABLED, command=self.restartScheduler)
        buttonUndo.grid(row=2, column=0)

        buttonRedo = Button(self.frameScheduler, text=txt('Redo'), width=10, fg='blue', state=DISABLED, command=self.restartScheduler)
        buttonRedo.grid(row=2, column=1)
        
    def createSkillSchedule(self):
        self.frameSkillSchedule = Frame(self.frameScheduler) #https://blog.teclado.com/tkinter-scrollable-frames/
        self.frameSkillSchedule.grid(row = 3, column = 0, rowspan = 20, columnspan = 8,ipadx=2, pady=2)
        
        self.vLoop = IntVar()
        self.loopRepeat = Entry(self.frameSkillSchedule, width=frameItemWidth[cLoop], textvariable=self.vLoop, bd=1)
        self.loopRepeat.grid(row=0, column=cLoop)
        
        for i in range(1, len(labelSchedulerHeader)):
            Label(self.frameSkillSchedule,text = txt(labelSchedulerHeader[i]),width = frameItemWidth[i]+2, anchor='w').grid(row = 0, column = i)
        
        canvas = Canvas(self.frameSkillSchedule, width = 515, height = 550)
        scrollbar = Scrollbar(self.frameSkillSchedule, orient='vertical', command=canvas.yview)
        self.scrollable_frame = Frame(canvas)
        
        self.scrollable_frame.bind(
            '<Configure>',
            lambda e: canvas.configure(
                scrollregion=canvas.bbox('all')
            )
        )
        canvas.create_window((0, 0), window=self.scrollable_frame, anchor='nw')
        canvas.configure(yscrollcommand=scrollbar.set)
        
        canvas.grid(row = 1, column = 0, rowspan = 20,columnspan = len(labelSchedulerHeader))
        scrollbar.grid(row = 1, column =len(labelSchedulerHeader),rowspan = 20,sticky = 'ns')
        self.restartScheduler()

    def createImage(self):
        image = Image.open(model+'.jpg')
        ratio = image.size[0]/300
        image=image.resize((300, round(image.size[1]/ratio)))
        
        img = ImageTk.PhotoImage(image)
        self.frameImage = Label(self.frameController, image=img)
        self.frameImage.image = img
        self.frameImage.grid(row=4, column=3, rowspan=2, columnspan=2)
        
    def closeWindow(self):
        print(self.window.winfo_children())
        for w in self.window.winfo_children():
            w.destroy()
        self.window.destroy()
        
    def changLan(self,l):
        global language
        if self.ready and txt('lan') != l:
#            self.window.destroy()
#            self.closeWindow()
            language = languageList[l]
#            app()
#            self.window.update()
            self.window.title(txt('title'))
            self.menubar.destroy()
            self.frameController.destroy()
            self.frameDial.destroy()
            self.frameImage.destroy()
            self.framePosture.destroy()
            self.frameScheduler.destroy()
            self.frameSkillSchedule.destroy()
            self.createMenu()
            self.createController()
            self.createImage()
            self.createDial()
            self.createPosture()
            self.createScheduler()
            self.createSkillSchedule()
        
    def about(self):
        messagebox.showinfo('Petoi Controller UI', 'www.petoi.com')
    
    def changeModel(self,modelName):
        global model
        global postureTable
        if self.ready and modelName != model:
            model = modelName
            postureTable = postureDict[model]
            self.frameController.destroy()
            self.framePosture.destroy()
            self.frameImage.destroy()
            self.createController()
            self.createPosture()
            self.createImage()
        
    def addFrame(self, currentRow):
        singleFrame = Frame(self.scrollable_frame, borderwidth=1, relief=RAISED)
        
        vChecked = BooleanVar()
        loopCheck = Checkbutton(singleFrame, variable = vChecked, onvalue=True, offvalue=False,
            command=lambda idx=currentRow: self.setCheckBox(idx))
        loopCheck.grid(row = 0,column = cLoop)
        
        rowLabel = Label(singleFrame, text = str(currentRow), width = frameItemWidth[cLabel])
        rowLabel.grid(row=0, column=cLabel)
        
        setButton = Button(singleFrame, text = '* '+txt('Set'), font='sans 14 bold', fg='blue', width=frameItemWidth[cSet], command=lambda idx=currentRow: self.setFrame(idx))
        
        vSpeed = StringVar()
        Spinbox(singleFrame, width=frameItemWidth[cSpeed], values = ('1','2','4','8','12','16','32','max'), textvariable = vSpeed, wrap=True).grid(row=0, column=cSpeed)
        
        vDelay = IntVar()
        delayStep = 50
        delayOption = list(range(0,100,50))+list(range(100,1000,100))+list(range(1000,6001,1000))
        Spinbox(singleFrame, width=frameItemWidth[cDelay], values=delayOption, textvariable = vDelay, wrap=True).grid(row=0, column=cDelay)
        
        vTrig = StringVar()
        Spinbox(singleFrame, width=frameItemWidth[cTrig], values=[0,-1,1,-2,2], textvariable = vTrig, wrap=True).grid(row=0, column=cTrig)
        
        vAngle = IntVar()
        Spinbox(singleFrame, width=frameItemWidth[cAngle], from_ = -256, to = 255, textvariable = vAngle, wrap=True).grid(row=0, column=cAngle)
        
        vNote = StringVar()
#        letters = string.ascii_lowercase + string.ascii_uppercase + string.digits
#        vNote.set('note: '+ ''.join(random.choice(letters) for i in range(5)) )
        
        while True:
            note = random.choice(WORDS)
            if len(note) <= 5:
                break
        vNote.set(note + str(currentRow))  # 'note')
        color = rgbtohex(random.choice(range(64, 192)), random.choice(range(64, 192)), random.choice(range(64, 192)))
        Entry(singleFrame, width=frameItemWidth[cNote], fg=color, textvariable=vNote, bd=1).grid(row=0, column=cNote)

        delButton = Button(singleFrame, text='<', fg='red', width=frameItemWidth[cDel], command=lambda idx=currentRow: self.delFrame(idx))

        addButton = Button(singleFrame, text='v', fg='green', width=frameItemWidth[cAdd], command=lambda idx=currentRow: self.addFrame(idx + 1))

        setButton.grid(row=0, column=cSet)
        delButton.grid(row=0, column=cDel)
        addButton.grid(row=0, column=cAdd)

        self.updateButtonCommand(currentRow, 1)
        
        if currentRow ==0:
            newFrameData = copy.deepcopy(self.frameData)
        else:
            newFrameData = copy.deepcopy(self.frameList[currentRow-1][2])
            if self.activeFrame>=currentRow:
                self.activeFrame+=1
        newFrameData[4] = 0# don't add the loop tag
        vSpeed.set('8')
        vDelay.set(0)
        self.frameList.insert(currentRow, [currentRow, singleFrame, newFrameData])
        self.changeButtonState(currentRow)
        singleFrame.grid(row=currentRow + 1, column=0, columnspan=7, ipadx=1, pady=1)
#        singleFrame.update() #any update() after rebuilding the window will cause some problem in the button's function
            
    def pause(self):
        self.pause = true
            
    def delFrame(self, currentRow):
        self.frameList[currentRow][1].destroy()
        del self.frameList[currentRow]
        self.updateButtonCommand(currentRow, -1)
        if self.activeFrame >= currentRow:
            self.activeFrame -= 1

    def getWidget(self, row, idx):
        frame = self.frameList[row]
        widgets = frame[1].winfo_children()
        return widgets[idx]
        
    def updateButtonCommand(self, currentRow, shift):
        for f in range(currentRow, len(self.frameList)):
            frame = self.frameList[f]
            frame[0] += shift
            widgets = frame[1].winfo_children()
            widgets[cLabel].configure(text = str(frame[0])) #set
            widgets[cLoop].configure(command=lambda idx=frame[0]: self.setCheckBox(idx))
            widgets[cSet].configure(command=lambda idx=frame[0]: self.setFrame(idx)) #set
            widgets[cDel].configure(command=lambda idx=frame[0]: self.delFrame(idx)) #delete
            widgets[cAdd].configure(command=lambda idx=frame[0]: self.addFrame(idx + 1)) #add
            frame[1].grid(row=frame[0] + 1)
        self.totalFrame += shift

    def changeButtonState(self, currentRow):
        if self.totalFrame > 1:
            self.getWidget(currentRow, cSet).configure(text = '* '+txt('Set'), font='sans 14 bold')
            if currentRow !=self.activeFrame:
                self.getWidget(self.activeFrame, cSet).configure(text = '< '+txt('Set'), font='sans 12')
                self.activeFrame = currentRow
            
    def setFrame(self, currentRow):
        currentFrame = self.frameList[currentRow]
        if currentRow != self.activeFrame:
            self.frameData = copy.deepcopy(currentFrame[2])
            self.changeButtonState(currentRow)
            self.updateSliders(self.frameData)
            if self.online:
                wrapper(['L',self.frameData[4:20],0])
        else:
            for i in range(20):
                if currentFrame[2][4+i] != self.frameData[4+i]: # the joint that's changed
                    for f in range(currentRow+1, self.totalFrame):
                        frame1 = self.frameList[f-1]
                        frame2 = self.frameList[f]
                        if frame1[2][4+i] == frame2[2][4+i]: # carry over to the next frame
                           frame2[2][4+i] = self.frameData[4+i]
                        else:
                            break
#                currentFrame[2][4+i] = self.frameData[4+i] #doesn't work? copied as reference?
            currentFrame[2] = copy.deepcopy(self.frameData)
            self.getWidget(currentRow, cSet).configure(text = '* '+txt('Set'), font='sans 14 bold')
        
    def close_pop(self,top):
        top.destroy()
        
    def insert_val(self,e):
        e.insert(0, 'Hello World!')
        
    def loadSkillDataText(self, top):
        skillDataString = self.skillText.get('1.0','end')
        top.destroy()
        self.restartScheduler()
        skillData = list(map(int,''.join(skillDataString.split()).split('{')[1].split('}')[0].split(',')[:-1]))
        if skillData[0] < 0:
            header = 7
            loopFrom, loopTo, repeat = skillData[4:7]
            self.vLoop.set(repeat)
            copyFrom = 4
        else:
            header = 4
            if skillData[0] == 1:
                copyFrom = 4
            else:
                copyFrom = 12
        self.frameSize = (len(skillData)-header)//abs(skillData[0])
        for f in range(abs(skillData[0])):
            if f != 0:
                self.addFrame(f)
            frame = self.frameList[f]
            frame[2][copyFrom:copyFrom+self.frameSize] = copy.deepcopy(skillData[header + self.frameSize*f:header+self.frameSize*(f+1)])
            if skillData[0]< 0:
                if f == loopFrom or f == loopTo:
                    self.getWidget(f, cLoop).select()
                    frame[2][4]=1
                else:
                    frame[2][4]=0
#                    print(self.getWidget(f, cLoop).get())
                self.getWidget(f, cSpeed).delete(0, END)
                if frame[2][20] == 0:
                    self.getWidget(f, cSpeed).insert(0,'max')
                else:
                    self.getWidget(f, cSpeed).insert(0,frame[2][20])
                self.getWidget(f, cDelay).delete(0, END)
                self.getWidget(f, cDelay).insert(0,frame[2][21]*50)
                
                self.getWidget(f, cTrig).delete(0, END)
                self.getWidget(f, cAngle).delete(0, END)
                self.getWidget(f, cTrig).insert(0,frame[2][22])
                self.getWidget(f, cAngle).insert(0,frame[2][23])
                
            else:
                self.getWidget(f, cSpeed).delete(0, END)
                self.getWidget(f, cSpeed).insert(0,'max')
            self.activeFrame = f
        
        
    def popImport(self):
       #Create a Toplevel window
        top= Toplevel(self.window)
        top.geometry('900x500')

       #Create an Entry Widget in the Toplevel window
        Button(top,text= txt('Open File'), width = 10, state = DISABLED, command= lambda:insert_val(entry)).grid(row = 0, column = 0)
       #Create a Button Widget in the Toplevel Window
        button= Button(top, text=txt('Cancel'), width = 10, command=lambda:self.close_pop(top)).grid(row =0, column = 1 )
        button= Button(top, text=txt('Ok'), width = 10, command= lambda:self.loadSkillDataText(top)).grid(row =0, column = 2 )
       
        entryFrame = Frame(top)
        entryFrame.grid(row = 1, column = 0, columnspan = 4,padx = 10,pady = 10)
                
        self.skillText= Text(entryFrame, width= 120, spacing1= 2)
        self.skillText.insert('1.0',txt('exampleFormat')
        +'\n\nconst int8_t hi[] PROGMEM ={\n\
    -3,  0, -30, 1,\n\
     1,  2,   3,\n\
     0,-20, -60,   0,   0,   0,   0,   0,  35,  30, 120, 105,  75,  60, -40, -30,     4, 2, 0, 0,\n\
    35, -5, -60,   0,   0,   0,   0,   0, -75,  30, 125,  95,  40,  75, -45, -30,    10, 0, 0, 0,\n\
    40,  0, -35,   0,   0,   0,   0,   0, -60,  30, 125,  95,  60,  75, -45, -30,    10, 0, 0, 0,\n\
};')
        self.skillText.grid(row = 0, column = 0)
        
        scrollY = Scrollbar(entryFrame, width = 20, orient = VERTICAL)
        scrollY.grid(row = 0, column = 1, sticky='ns')
        scrollY.config(command=self.skillText.yview)
        self.skillText.config(yscrollcommand=scrollY.set)
        
#        scrollX = Scrollbar(entryFrame, width = 20, orient = VERTICAL)
#        scrollX.grid(row = 1, column = 0, sticky='ew')
#        scrollX.config(command=self.skillText.xview)
#        self.skillText.config(xscrollcommand=scrollX.set)
        entryFrame.columnconfigure(0, weight=1)
        entryFrame.rowconfigure(0, weight=1)
        
    def playThread(self):
        self.playStop = False
        self.buttonRun.configure(text = txt('Stop'), fg = 'red',command = self.stop)
        self.window.update() #any update() after rebuilding the window will cause some problem in the button's function
        t=threading.Thread(target=self.play)#doesn't work
        t.start()
        
    def play(self):
        if self.activeFrame+1 == self.totalFrame:
            self.getWidget(self.activeFrame, cSet).configure(text = '< '+txt('Set'), font='sans 12')
#            self.window.update() #any update() after rebuilding the window will cause some problem in the button's function
            self.activeFrame = 0;
        for f in range(self.activeFrame, self.totalFrame):
            if self.playStop:
                break
            frame = self.frameList[f]
            self.frameData = copy.deepcopy(frame[2])
            self.updateSliders(self.frameData)
            self.changeButtonState(f)
            self.window.update()
            if self.online:
                wrapper(['L',self.frameData[4:20],0.01])
        self.buttonRun.configure(text = txt('Play'), fg = 'green',command = self.playThread)
        self.playStop = False
        
        
    def stop(self):
        self.buttonRun.configure(text = txt('Play'), fg = 'green',command = self.playThread)
        self.playStop = True
        
    def export(self):
        if self.activeFrame+1 == self.totalFrame:
            self.getWidget(self.activeFrame, cSet).configure(text = '< '+txt('Set'), font='sans 12')
            self.window.update()
            self.activeFrame = 0;
        skillData = list()
        loopStructure = list()
        for f in range(self.activeFrame, self.totalFrame):
            frame = self.frameList[f]
            self.frameData = copy.deepcopy(frame[2])
            if(self.frameData[4]==1):
                loopStructure.append(f)
            if self.getWidget(f, cSpeed).get() == 'max':
                self.frameData[20] = 0
            else:
                self.frameData[20] = int(self.getWidget(f, cSpeed).get())
            self.frameData[21] = int(self.getWidget(f, cDelay).get())//50
            self.frameData[22] = int(self.getWidget(f, cTrig).get())
            self.frameData[23] = int(self.getWidget(f, cAngle).get())
            skillData.append(self.frameData[4:])
            self.updateSliders(self.frameData)
            self.changeButtonState(f)
            self.window.update()
        if len(loopStructure)==0:
            loopStructure = [0]
        print('{')
        print('{:>4},{:>4},{:>4},{:>4},'.format(*[-len(skillData), 0, 0, 1]))
        print('{:>4},{:>4},{:>4},'.format(*[ loopStructure[0], loopStructure[-1], self.loopRepeat.get()]))
        for row in skillData:
            print('{:>4},{:>4},{:>4},{:>4},{:>4},{:>4},{:>4},{:>4},{:>4},{:>4},{:>4},{:>4},{:>4},{:>4},{:>4},{:>4}, {:>4},{:>4},{:>4},{:>4},'.format(*row))
        print('};')
        
        skillData.insert(0,[-len(skillData), 0, 0, 1, loopStructure[0], loopStructure[-1], self.loopRepeat.get()])
        flat_list = [item for sublist in skillData for item in sublist]
        if self.online:
            wrapper(['K',flat_list,1])


    def restartScheduler(self):
        for f in self.frameList:
            f[1].destroy()
        self.frameList.clear()
        self.frameData = [0,0,0,0,
                            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                            8,0,0,0,]
        self.frameSize = 16
        self.totalFrame = 0
        self.activeFrame = 0
        self.addFrame(0)
#        self.window.update()
#        self.setPose('calib')
        
    def indicateEdit(self):
        frame = self.frameList[self.activeFrame]
        if frame[2] != self.frameData:
            self.getWidget(self.activeFrame, cSet).configure(text = '* '+ txt('Save'), font='sans 14 bold')
#            print('frm',frame[2])
#            print('dat',self.frameData)
        else:
            self.getWidget(self.activeFrame, cSet).configure(text = '* '+txt('Set'), font='sans 12')
            
    def setCheckBox(self,currentRow):
        frame = self.frameList[currentRow]
        if frame[2][4] == 0:
            frame[2][4] = 1
        else:
            frame[2][4] = 0
        
    def setAngle(self, i, value):
        if self.ready == 1:
            self.frameData[4 + i] = int(value)
#            print(frame[2])
#            print(self.frameData)
            self.indicateEdit()
            if self.online:
                wrapper(['I', [i, value], 0.0])
            else:
                print(str(i)+', ' + str(value))

    def setYPR(self, i, value):
        if self.ready == 1:
            if self.online:
                wrapper(['t', [i, value], 0.0])
            else:
                print(str(i)+', ' + str(value))

    def setPose(self, pose):
        if self.ready == 1:
            self.getWidget(self.activeFrame, cNote).delete(0, END)
            self.getWidget(self.activeFrame, cNote).insert(0, pose + str(self.activeFrame))
            self.updateSliders(postureTable[pose])
            self.indicateEdit()
            if self.online:
                wrapper(['k'+pose, 0])
                if pose == 'rest':
                    wrapper(['d', 0])
    
    def setSpeed(self):
        self.frameData[20] = self.getWidget(self.activeFrame, cSpeed).get()
#        print(self.frameData)
#        self.indicateEdit()
        
    def setDelay(self):
        self.frameData[21] = int(self.getWidget(self.activeFrame, cDelay).get())
#        print(self.frameData)
#        self.indicateEdit()
                
            
    def updateSliders(self, angles):
        for i in range(3):
#                    self.values[i].set(postureTable[pose][i+1])
            self.values[i].set(0)
            
        for i in range(16):
            self.values[3 + i].set(angles[4+i])
            self.frameData[4+i] = angles[4+i]
                

    def dial(self, value):
#        global self.online
        if self.ready == 1:
            if value == 'Connect':
                state = wrapper(['b', [10,90],0])
                self.online = not self.online
                if state == 'err':
                    self.online = False
                buttons = self.frameDial.winfo_children()[2:]
                if self.online:
                    self.frameDial.winfo_children()[1].configure(text = txt('Disconnect'),fg='red')
                    for b in buttons:
                        b.configure(state = NORMAL)
                else:
                    self.frameDial.winfo_children()[1].configure(text = txt('Connect'),fg = 'blue')
                    for b in buttons:
                        b.configure(state = DISABLED)
            elif self.online == True:
                state = wrapper([value, 0])
                if state == 'p':
                    self.frameDial.winfo_children()[2].configure(text = txt('Pause'),fg = 'green')
                elif state == 'P':
                    self.frameDial.winfo_children()[2].configure(text = txt('Paused'),fg = 'red')
                elif state == 'g':
                    self.frameDial.winfo_children()[3].configure(text = txt('Gyro:OFF'),fg = 'red')
                elif state == 'G':
                    self.frameDial.winfo_children()[3].configure(text = txt('Gyro: ON'),fg = 'green')
                elif state == 'z':
                    self.frameDial.winfo_children()[4].configure(text = txt('Random:OFF'),fg = 'green')
                elif state == 'Z':
                    self.frameDial.winfo_children()[4].configure(text = txt('Random: ON'),fg = 'orange')

    def on_closing(self):
        if messagebox.askokcancel(txt('Quit'), txt('Do you want to quit?')):
            if self.online:
                closeSerialBehavior()
            self.window.destroy()


if __name__ == '__main__':
    app()

# unused text codes for references
#        letters = WORDS#string.ascii_lowercase + string.ascii_uppercase + string.digits
#        vNote.set('note: '+ ''.join(random.choice(letters) for i in range(5)) )#'note')
