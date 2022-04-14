#!/usr/bin/python3
# -*- coding: UTF-8 -*-

import string
import random
import sys
sys.path.append("../serialMaster/")
from ardSerial import *
from tkinter import *
from tkinter import messagebox
import tkinter.font as tkFont
from PIL import ImageTk, Image
import copy

#test = True
test = False


headSliderGap = 0
width = 8
centerWidth = 2
height = 8

NaJoints = {'Nybble': [3, 4, 5, 6, 7],
            'Bittle': [1, 2, 3, 4, 5, 6, 7]}

model = 'Nybble'
#model = 'Bittle'


def rgbtohex(r, g, b):
    return f'#{r:02x}{g:02x}{b:02x}'

def printH(head, value):
    print(head,end = ' ')
    print(value)

word_file = "/usr/share/dict/words"
WORDS = open(word_file).read().splitlines()

cLabel, cSet, cSpeed, cDelay, cNote, cDel, cAdd = range(7)

class app:

    def __init__(self):
        self.window = Tk()
        self.sliders = list()
        self.values = list()
        self.ready = 0
        # , slant="italic")
        self.myFont = tkFont.Font(
            family="Times New Roman", size=20, weight="bold")
        self.window.title("Petoi Python UI for OpenCat")
        self.window.geometry("+100+10")
        self.totalFrame = 0
        self.activeFrame = 0
        self.frameList = list()
        self.reference = 0
        self.frameData = [0,0,0,0,
                            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                            0,0,0,0,]
        self.pause = False

        self.frameController = Frame(self.window)
        self.frameController.grid(row=0, column=0, rowspan=9, ipadx=10)
        labelController = Label(self.frameController,
                                text="Joint Controller", font=self.myFont)
        labelController.grid(row=0, column=0, columnspan=width)

        self.frameImu = Frame(self.frameController)
        self.frameImu.grid(row=6, column=3, rowspan=1, columnspan=2, ipady=10)

        yprNames = ['yaw', 'pitch', 'roll']
        for i in range(3):
            label = Label(self.frameImu, text=yprNames[i], bg="Light Blue1")
            value = DoubleVar()
            sliderBar = Scale(self.frameImu, variable=value, orient=HORIZONTAL, borderwidth=2, from_=-60, to=60, length=300, tickinterval=10, resolution=1, command=lambda value, idx=i:  self.setYPR(idx, value))
            sliderBar.set(0)
            label.grid(row=0+i, column=3, columnspan=1)
            sliderBar.grid(row=0+i, column=4, columnspan=centerWidth)
            self.sliders.append(sliderBar)
            self.values.append(value)

        scaleNames = [
            "Head Pan", "Head Tilt", "Tail Pan", "N/A",
            "Shoulder LeftFront", "Shoulder RightFront", "Shoulder RightBack", "Shoulder LeftBack",
            "Arm LeftFront", "Arm RightFront", "Arm RightBack", "Arm LeftBack",
            "Knee LeftFront", "Knee RightFront", "Knee RightBack", "Knee LeftBack"]

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
                LEN = 500

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
                          text='(' + str(i)+')\n'+scaleNames[i])
            value = DoubleVar()
            sliderBar = Scale(self.frameController, state=stt, bg=clr, variable=value, orient=ORI, borderwidth=2, from_=-125*tickDirection, to=125*tickDirection, length=LEN, tickinterval=25, resolution=1,
                              command=lambda value, idx=i:  self.setAngle(idx, value))
            sliderBar.set(0)
            label.grid(row=ROW + 1, column=COL, columnspan=cSPAN, pady=10)
            sliderBar.grid(row=ROW+2, column=COL, rowspan=rSPAN, columnspan=cSPAN, ipadx=2, ipady = 2, padx = 10, pady=(0, PAD))
            
            self.sliders.append(sliderBar)
            self.values.append(value)

        image = Image.open("./jointIndex.jpg").resize((300, 300))
        img = ImageTk.PhotoImage(image)
        panel = Label(self.frameController, image=img)
        panel.image = img
        panel.grid(row=4, column=3, rowspan=2, columnspan=2)

        self.frameDial = Frame(self.window)
        self.frameDial.grid(row=0, column=1, ipady=10)
        labelDial = Label(self.frameDial, text="State Dials", font=self.myFont)
        labelDial.grid(row=0, column=0, columnspan=3)
        dialTable = {'Gyro': 'g', 'Pause': 'p', 'Random': 'z'}
        i = 0
        for key in dialTable:
            button = Button(self.frameDial, text=key, width=6, fg="blue", command=lambda k=key: self.dial(dialTable[k]))
            button.grid(row=i//3 + 1, column=i % 3)
            i += 1

        self.framePosture = Frame(self.window)
        self.framePosture.grid(row=1, column=1, ipady=10)
        labelPosture = Label(self.framePosture, text="Postures", font=self.myFont)
        labelPosture.grid(row=0, column=0, columnspan=3)
        i = 0
        for pose in postureTable:
            button = Button(self.framePosture, text=pose, fg="blue", width=10, command=lambda p=pose:  self.setPose(p))
            button.grid(row=i//3 + 1, column=i % 3)
            i += 1

        self.frameScheduler = Frame(self.window)
        self.frameScheduler.grid(row=2, column=1, rowspan=18, ipady=400)
        labelScheduler = Label(self.frameScheduler, text="Scheduler", font=self.myFont)
        labelScheduler.grid(row=0, column=0, columnspan=3)

        buttonRun = Button(self.frameScheduler, text='Run', width=10, fg="green", command=self.run)
        buttonRun.grid(row=1, column=0)

        buttonExp = Button(self.frameScheduler, text='Pause', width=10, fg="red", command=self.pause)
        buttonExp.grid(row=1, column=1)

        buttonExp = Button(self.frameScheduler, text='Export', width=10, fg="blue", command=self.export)
        buttonExp.grid(row=1, column=2)

        buttonRestart = Button(self.frameScheduler, text='Restart', width=10, fg="blue", command=self.restartScheduler)
        buttonRestart.grid(row=2, column=2)

        buttonUndo = Button(self.frameScheduler, text='Undo', width=10, fg="blue", state=DISABLED, command=self.restartScheduler)
        buttonUndo.grid(row=2, column=0)

        buttonRedo = Button(self.frameScheduler, text='Redo', width=10, fg="blue", state=DISABLED, command=self.restartScheduler)
        buttonRedo.grid(row=2, column=1)

        self.addFrame(0)

        self.ready = 1
        self.window.protocol("WM_DELETE_WINDOW", self.on_closing)
        self.window.mainloop()

    def addFrame(self, currentRow):
#        if not test:
#            currentAngles = wrapper(['j', 0])
        singleFrame = Frame(self.frameScheduler, borderwidth=1, relief=RAISED)
        
        rowLabel = Label(singleFrame, text = str(currentRow), width = 1)
        rowLabel.grid(row=0, column=cLabel)
        
        setButton = Button(singleFrame, text='* Edit',  fg="blue", width=3, command=lambda idx=currentRow: self.setFrame(idx))
        vSpeed = StringVar()
        vSpeed.set("speed")
        Entry(singleFrame, width=4, textvariable=vSpeed, bd=1).grid(row=0, column=cSpeed)
        vDelay = StringVar()
        vDelay.set("delay")
        Entry(singleFrame, width=4, textvariable=vDelay, bd=1).grid(row=0, column=cDelay)
        vNote = StringVar()
        while True:
            note = random.choice(WORDS)
            if len(note) == 5:
                break
        vNote.set('note: ' + note)  # "note")
        color = rgbtohex(random.choice(range(64, 192)), random.choice(range(64, 192)), random.choice(range(64, 192)))
        Entry(singleFrame, width=9, fg=color, textvariable=vNote, bd=1).grid(row=0, column=cNote)

        delButton = Button(singleFrame, text='< Del', fg="red", width=2, command=lambda idx=currentRow: self.delFrame(idx))

        addButton = Button(singleFrame, text='Add v', fg="green", width=2, command=lambda idx=currentRow: self.addFrame(idx + 1))

        setButton.grid(row=0, column=cSet)
        delButton.grid(row=0, column=cDel)
        addButton.grid(row=0, column=cAdd)

        self.updateButtonRow(currentRow, 1)
        if currentRow ==0:
            self.frameList.insert(currentRow, [currentRow, singleFrame, self.frameData])
        else:
            self.frameList.insert(currentRow, [currentRow, singleFrame, self.frameList[currentRow-1][2]])
        singleFrame.grid(row=currentRow + 3, column=0, columnspan=4, ipadx=2, pady=2)
        
        self.changeButtonState(currentRow)
        
    def run(self):
        if self.activeFrame+1 == self.totalFrame:
            self.activeFrame = 0;
        for f in range(self.activeFrame, self.totalFrame):
#            if self.pause:
#                self.pause = False
#                break
            frame = self.frameList[f]
            if(f>0):
                self.changeButtonState(f)
            wrapper(['L',frame[2][4:20],0.5])
            
    def pause(self):
        self.pause = true
            
    def delFrame(self, currentRow):
        self.frameList[currentRow][1].destroy()
        del self.frameList[currentRow]
        self.updateButtonRow(currentRow, -1)
        if self.activeFrame >= currentRow:
            self.activeFrame -= 1

    def updateButtonRow(self, currentRow, shift):
        for f in range(currentRow, len(self.frameList)):
            frame = self.frameList[f]
            frame[0] += shift
            widgets = frame[1].winfo_children()
            widgets[cLabel].configure(text = str(frame[0])) #set
            widgets[cSet].configure(text= '< Set',command=lambda idx=frame[0]: self.setFrame(idx)) #set
            widgets[cDel].configure(command=lambda idx=frame[0]: self.delFrame(idx)) #delete
            widgets[cAdd].configure(command=lambda idx=frame[0]: self.addFrame(idx + 1)) #add
            frame[1].grid(row=frame[0] + 3)
        self.totalFrame += shift


        #    def delFrame(self):
#        if self.ready == 1:

    def getWidget(self, row, idx):
        frame = self.frameList[row]
        widgets = frame[1].winfo_children()
        return widgets[idx]
            
    def changeButtonState(self, currentRow):
        if self.totalFrame > 1:
            self.getWidget(currentRow, cSet).configure(text = '* Edit')
            if currentRow !=self.activeFrame:
                self.getWidget(self.activeFrame, cSet).configure(text = '< Set')
                self.activeFrame = currentRow
            
    def setFrame(self, currentRow):
        #        wrapper['L',pose,0.5]
        frame = self.frameList[currentRow]
        if currentRow != self.activeFrame:
            self.frameData = copy.deepcopy(frame[2])
            self.changeButtonState(currentRow)
            wrapper(['L',self.frameData[4:20],0.5])
            self.updateSliders(self.frameData)
        else:
            frame[2] = copy.deepcopy(self.frameData)
            self.getWidget(self.activeFrame, cSet).configure(text = '* set')

#        widgets = frame[1].winfo_children()
#        print(frame[1].winfo_children())
#        configure(command = lambda pose = jointAngles : self.setFrame(pose))
        
    def export(self):
        self.activeFrame = 0;
        scheduler = []
        for f in range(self.activeFrame, self.totalFrame):
            frame = self.frameList[f]
            scheduler.append(['L',frame[2][4:20],0.1])
        print(scheduler)
        schedulerToSkill(scheduler)
        

    def restartScheduler(self):
        for f in self.frameList:
            f[1].destroy()
        self.frameList.clear()
        self.totalFrame = 0
        self.activeFrame = 0
        self.addFrame(self.totalFrame)

    def setAngle(self, i, value):
        if self.ready == 1:
            self.frameData[4 + i] = value
            if test:
                print(str(i)+', ' + str(value))
            else:
                wrapper(['I', [i, value], 0.0])

    def setYPR(self, i, value):
        if self.ready == 1:
            if test:
                print(str(i)+', ' + str(value))
            else:
                wrapper(['t', [i, value], 0.0])

    def setPose(self, pose):
        if self.ready == 1:
            self.getWidget(self.activeFrame, cNote).delete(0, END)
            self.getWidget(self.activeFrame, cNote).insert(0, pose + str(self.activeFrame))
            if not test:
                wrapper(['k'+pose, 0])
                if pose == 'rest':
                    wrapper(['d', 0])
            self.updateSliders(postureTable[pose])
            
    def updateSliders(self, angles):
        for i in range(3):
#                    self.values[i].set(postureTable[pose][i+1])
            self.values[i].set(0)
            
        for i in range(20):
            if i<16:
                self.values[3 + i].set(angles[i+4])
            self.frameData[i] = angles[i]
                

    def dial(self, value):
        if self.ready == 1:
            if not test:
                wrapper([value, 0])

    def on_closing(self):
        if messagebox.askokcancel("Quit", "Do you want to quit?"):
            if not test:
                closeSerialBehavior()
            self.window.destroy()


if __name__ == "__main__":
    if not test:
        flushSerialOutput(300)
    app()

# unused text codes for references
#        letters = WORDS#string.ascii_lowercase + string.ascii_uppercase + string.digits
#        vNote.set('note: '+ ''.join(random.choice(letters) for i in range(5)) )#"note")
