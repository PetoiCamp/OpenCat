from translate import *
import platform
import sys
if platform.system() == "Windows":    # for Windows
    sys.path.append('..\\serialMaster\\')
    resourcePath = '.\\resources\\'
    releasePath = '.\\release\\'
elif platform.system() == "Linux":    # for Linux
    sys.path.append('/usr/share/petoi-opencat/serialMaster/')
    resourcePath = '/usr/share/petoi-opencat/resources/'
    releasePath = '/usr/share/petoi-opencat/release/'
else:
    sys.path.append('../serialMaster/')
    resourcePath = './resources/'
    releasePath = './release/'
sys.path.append(resourcePath)

from ardSerial import *
from tkinter import *
from tkinter import messagebox
from PIL import ImageTk, Image
import tkinter.font as tkFont
import threading
import random
import datetime
import os

NyBoard_version = 'NyBoard_V1_2'
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
    'Bittle X': [1, 2, 3, 4, 5, 6, 7],
    'DoF16' : []
}
scaleNames = [
    'Head Pan', 'Head Tilt', 'Tail Pan', 'N/A',
    'Shoulder', 'Shoulder', 'Shoulder', 'Shoulder',
    'Arm', 'Arm', 'Arm', 'Arm',
    'Knee', 'Knee', 'Knee', 'Knee']
sideNames = ['Left Front', 'Right Front', 'Right Back', 'Left Back']

ports = []

def makeDirectory(path):
    # delete spaces in the path string
    path = path.strip()
    # delete the '\' at the end of path string
    path = path.rstrip("\\").rstrip("/")
    # path = path.rstrip("/")
	
    # check whether the path exists
    isExists = os.path.exists(path)
    
    if not isExists:
        # Create the directory if it does not exist
        os.makedirs(path)
        print(path + ' creat successfully')
        return True
    else:
        # If the directory exists, it will not be created and prompt that the directory already exists.
        print(path + ' already exists')
        return False

if platform.system() == "Windows":    # for Windows
    separation = '\\'
    homeDri = os.getenv('HOMEDRIVE') 
    homePath = os.getenv('HomePath') 
    configDir = homeDri + homePath
else:  # for Linux & macOS
    separation = '/'
    home = os.getenv('HOME') 
    configDir = home 
configDir = configDir + separation +'.config' + separation +'Petoi'
makeDirectory(configDir)
defaultConfPath = configDir + separation + 'defaultConfig.txt'
print(defaultConfPath)

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
