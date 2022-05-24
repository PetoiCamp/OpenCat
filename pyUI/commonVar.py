from translate import *
import sys
sys.path.append('../serialMaster/')
resourcePath = './resources/'
sys.path.append(resourcePath)
from ardSerial import *
from tkinter import *
from tkinter import messagebox
from PIL import ImageTk, Image
from idlelib.tooltip import Hovertip
import tkinter.font as tkFont
import threading
import random
from datetime import date


NaJoints = {
    'Nybble': [3, 4, 5, 6, 7],
    'Bittle': [1, 2, 3, 4, 5, 6, 7],
    #    'DoF16' : []
}
scaleNames = [
    'Head Pan', 'Head Tilt', 'Tail Pan', 'N/A',
    'Shoulder', 'Shoulder', 'Shoulder', 'Shoulder',
    'Arm', 'Arm', 'Arm', 'Arm',
    'Knee', 'Knee', 'Knee', 'Knee']
sideNames = ['Left Front', 'Right Front', 'Right Back', 'Left Back']

ports = []
    
def createImage(frame, imgFile, imgW):
    img = Image.open(imgFile)
    ratio = img.size[0] / imgW
    img = img.resize((imgW, round(img.size[1] / ratio)))
    image = ImageTk.PhotoImage(img)
    imageFrame = Label(frame, image=image)
    imageFrame.image = image
    return imageFrame
