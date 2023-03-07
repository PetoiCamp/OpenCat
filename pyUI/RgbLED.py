#!/usr/bin/python3
# -*- coding: UTF-8 -*-

from commonVar import *
from tkinter.colorchooser import askcolor

root = Tk()
root.title('Eye Color Setter')
root.geometry('+400+200')

face = Frame(root).grid(row = 0,column = 0)


def change_color(i):
    colors = askcolor(title="Tkinter Color Chooser")
    print(list(colors[0])+[1,1])
    canvas.itemconfig(eyes[i], fill=colors[1])
    if colorBinderValue.get():
        canvas.itemconfig(eyes[1-i], fill=colors[1])
    
dia = 100
crd = [10,10]
gap = 35
btShift = [70,25]
width = dia*2 + gap + 2*crd[0]
root.geometry(str(width)+'x170')

canvas = Canvas(face,height=120)
canvas.grid(row = 0,column = 0)
eyeR = canvas.create_oval(crd[0], crd[1], crd[0]+dia, crd[1]+dia, outline="#000",
            fill="#606060", width=2)
eyeL = canvas.create_oval(crd[0]+dia+gap, crd[1], crd[0]+2*dia+gap, crd[1]+dia, outline="#000",
            fill="#606060", width=2)
eyes = [eyeR, eyeL]
btR = Button(face, text='Color',
    command=lambda:change_color(0))
btR.place(x=crd[0]+dia/2-btShift[0]/2,y=crd[1]+dia/2-btShift[1]/2)
btL = Button(face,text='Color',
    command=lambda:change_color(1))
btL.place(x=crd[0]+dia*3/2+gap-btShift[0]/2,y=crd[0]+dia/2-btShift[1]/2)
colorBinderValue = BooleanVar()
colorBinder = Checkbutton(face, text='<>', indicator=0, width=2,
                                 variable=colorBinderValue,onvalue=True, offvalue=False)
colorBinder.place(x=crd[0]+dia+5,y=crd[1]+dia/2-btShift[1]/2)

btnsEff = Frame(face)
btnsEff.grid(row = 1,column = 0,ipadx = 15)
ledEffects = ['Breath','Rotate','Flash']
for e in range(len(ledEffects)):
    Button(btnsEff,text=ledEffects[e],width = 4,).grid(row = 0,column = e)

#Button(btnsEff,text='None').grid(row = 0,column = 3)
root.mainloop()

