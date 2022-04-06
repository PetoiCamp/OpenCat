#!/usr/bin/python3
# -*- coding: UTF-8 -*-
from tkinter import *
from PIL import ImageTk, Image
import sys
sys.path.append("../serialMaster/")
from ardSerial import *



headSliderGap = 1
width = 10 + headSliderGap
centerWidth = 4
height = 11
scaleNames = [
    "Head Pan","Head Tilt","Tail Pan","N/A",\
    "Shoulder LeftFront","Shoulder RightFront","Shoulder RightBack","Shoulder LeftBack",\
    "Arm LeftFront","Arm RightFront","Arm RightBack","Arm LeftBack",\
    "Knee LeftFront","Knee RightFront","Knee RightBack","Knee LeftBack"]
yprNames = ['yaw','pitch','roll']
class app:

    def __init__(self):
        self.window =  Tk()
        self.sliders = list()
        self.ready = 0
        
        self.window.geometry("+300+10")
        
        frameController = Frame(self.window)
        frameController.pack(side=BOTTOM,fill = "none",expand=True)
        labelController = Label(frameController,text = "Controller")
        labelController.grid(row = 0, column = 0, columnspan = width)
        
        for i in range(16):
            rSPAN=1
            cSPAN=1
            if i<4:
                tickDirection = 1
                cSPAN=3+centerWidth//2
                if i<2:
                    ROW = 0
                    COL = i%2 * (width//2 + headSliderGap)# + (1-i%2) * (width//2-cSPAN)
                    
                else:
                    ROW = 9
                    COL = (1-i%2) * (width//2 + headSliderGap)# + i%2 * (width//2-cSPAN)
                rSPAN=1
                ORI=HORIZONTAL
                LEN = 500
                
            else:
                tickDirection = -1
                leftQ = (i - 1 ) % 4 > 1
                frontQ = i % 4 < 2
                upperQ = i / 4 < 3
                
                ROW = (i%4//2 +1 )* 2
                if leftQ:
                    COL = (1-leftQ) *(2+centerWidth) + 3 - i // 4
                else:
                    COL = (1-leftQ) *(2+centerWidth) + 1+ i // 4
                ORI=VERTICAL
                LEN = 250
                if ROW ==4:
                    rSPAN=3
            label = Label(frameController, text = str(i)+'\n'+scaleNames[i],bg = "Light Blue1")
            sl =  Scale(frameController, orient=ORI, borderwidth=2, from_=-125*tickDirection, to=125*tickDirection, length = LEN, tickinterval=25, resolution=1,\
                command = lambda value, idx = i :  self.setAngle(idx,value))
            sl.set(0)
            label.grid(row = ROW+1,column = COL,columnspan=cSPAN)
            sl.grid(row = ROW+2,column = COL,rowspan = rSPAN,columnspan=cSPAN)
#            print(str(i)+':\t'+str(ROW)+' '+str(COL))
#            sl.pack()
            self.sliders.append(sl)
        
        

        for i in range(3):
            label = Label(frameController, text = yprNames[i],bg = "Light Blue1")
            sl =  Scale(frameController, orient=HORIZONTAL, borderwidth=2, from_=-60, to=60, length = 300, tickinterval=10, resolution=1,\
                command = lambda value, idx = i :  self.setYPR(idx,value))
            sl.set(0)
            label.grid(row = 6+i,column = 3,columnspan=1)
            sl.grid(row = 6+i,column = 4,columnspan=centerWidth)
            self.sliders.append(sl)
            
            
        image = Image.open("./jointIndex.jpg").resize((400, 400))
        img=ImageTk.PhotoImage(image)
        panel = Label(frameController,image=img)
        panel.image = img
        panel.grid(row=4,column=3,rowspan = 2, columnspan=centerWidth+1)
        
        button = Button(frameController, text='Make Frame',fg = "blue", command = self.move)
        button.grid(row = 3, column = width//2)
        
        self.ready = 1
        self.window.mainloop()

    def setAngle(self,i,value):
        if self.ready == 1:
#            print(str(i)+', '+ str(value))
            wrapper(['I',[i,value],0.0])
    def setYPR(self,i,value):
        if self.ready == 1:
            print(yprNames[i]+'\t'+ str(value))
            wrapper(['t',[i,value],0.0])
    def move(self):
#            wrapper(['I',[0,self.sliders[0].get(),\
#                        1,self.sliders[1].get(),\
#                        2,self.sliders[2].get()],0.5])
        
        print('move')

    def on_closing(self):
        if tkMessageBox.askokcancel("Quit", "Do you want to quit?"):
#            closeSerialBehavior()
            self.window.destroy()
    


if __name__ == "__main__":
    flushSerialOutput(300)
    app()

