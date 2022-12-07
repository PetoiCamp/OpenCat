from translate import *
import sys
sys.path.append('../serialMaster/')
resourcePath = './resources/'
sys.path.append(resourcePath)
from ardSerial import *
from tkinter import *
from tkinter import messagebox
from PIL import ImageTk, Image
import tkinter.font as tkFont
import threading
import random
from datetime import date

verNumber = sys.version.split('(')[0].split()[0]
verNumber = verNumber.split('.')
print(verNumber)
#verNumber = [2,1,1] #for testing
supportHoverTip = True
if int(verNumber[0])<3 or int(verNumber[1])<7:
    print("Please upgrade your Python to 3.7.1 or above!")
    root = Tk()
    root.overrideredirect(1)
    root.withdraw()
    messagebox.showwarning(title='Warning', message='Please upgrade your Python\nto 3.7.1 or above\nto show hovertips!')
    root.destroy()
    supportHoverTip = False
#    exit(0)
    
try:
    from idlelib.tooltip import Hovertip
except Exception as e:
    logger.info("Cannot import hovertip!")
    raise e
    

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

def tip(item, note):
    if supportHoverTip:
        Hovertip(item,note)
#    else:
#        print(note)
