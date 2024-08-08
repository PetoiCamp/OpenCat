#!/usr/bin/python3
# -*- coding: UTF-8 -*-
from commonVar import *

language = languageList['English']

BittleRSet = {
    "imageW": 360,       # The width of image
    "sliderW": 220,      # The width of the slider rail corresponding to joint numbers 0 to 3
    "rowJoint1": 2,      # The row number of the label with joint number 2 and 3
    "sliderLen": 260,    # The length of the slider rail corresponding to joint numbers 4 to 15
    "rowJoint2": 4       # The row number of the label with joint number 4 or 15 is located
}

RegularSet = {
    "imageW": 250,
    "sliderW": 200,
    "rowJoint1": 11,
    "sliderLen": 150,
    "rowJoint2": 2
}

paraemterSet = {
    "Regular": RegularSet,
    "BittleR": BittleRSet
}

def txt(key):
    return language.get(key, textEN[key])
    
class Calibrator:
    def __init__(self,model,lan):
        self.calibratorReady = False
        global language
        language = lan
#        global goodPorts
        connectPort(goodPorts)
        start = time.time()
        while config.model_ == '':
            if time.time() - start > 5:
                config.model_ = model
                config.version_ = "N_210101"
                print('Use the model set in the UI interface.')
            time.sleep(0.01)
        self.configName = config.model_
        self.boardVersion = config.version_
        config.model_ = config.model_.replace(' ', '')
        if 'Bittle' in config.model_ and config.model_!= 'BittleR':
            self.model = 'Bittle'
        else:
            self.model = config.model_

        self.winCalib = Tk()
        self.winCalib.title(txt('calibTitle'))
        self.winCalib.geometry('+200+100')
        self.winCalib.resizable(False, False)
        self.calibSliders = list()
        self.OSname = self.winCalib.call('tk', 'windowingsystem')
        if self.OSname == 'win32':
            self.winCalib.iconbitmap(resourcePath + 'Petoi.ico')
            self.calibButtonW = 8
        else:
            self.calibButtonW = 4
        self.frameCalibButtons = Frame(self.winCalib)
        self.frameCalibButtons.grid(row=0, column=3, rowspan=13)
        calibButton = Button(self.frameCalibButtons, text=txt('Calibrate'), fg = 'blue', width=self.calibButtonW,command=lambda cmd='c': self.calibFun(cmd))
        standButton = Button(self.frameCalibButtons, text=txt('Stand Up'), fg = 'blue', width=self.calibButtonW, command=lambda cmd='balance': self.calibFun(cmd))
        restButton = Button(self.frameCalibButtons, text=txt('Rest'),fg = 'blue', width=self.calibButtonW, command=lambda cmd='d': self.calibFun(cmd))
        walkButton = Button(self.frameCalibButtons, text=txt('Walk'),fg = 'blue', width=self.calibButtonW, command=lambda cmd='walk': self.calibFun(cmd))
        saveButton = Button(self.frameCalibButtons, text=txt('Save'),fg = 'blue', width=self.calibButtonW, command=lambda: send(goodPorts, ['s', 0]))
        abortButton = Button(self.frameCalibButtons, text=txt('Abort'),fg = 'blue', width=self.calibButtonW, command=lambda: send(goodPorts, ['a', 0]))
#        quitButton = Button(self.frameCalibButtons, text=txt('Quit'),fg = 'blue', width=self.calibButtonW, command=self.closeCalib)
        calibButton.grid(row=6, column=0)
        restButton.grid(row=6, column=1)
        standButton.grid(row=6, column=2)
        walkButton.grid(row=11, column=0)
        saveButton.grid(row=11, column=1)
        abortButton.grid(row=11, column=2)
#        quitButton.grid(row=11, column=2)

        if self.model == 'BittleR':
            self.paraemterSet = paraemterSet['BittleR']
            scaleNames = BittleRScaleNames
        else:
            self.paraemterSet = paraemterSet['Regular']
            scaleNames = RegularScaleNames

        if "B" in self.boardVersion and config.model_ = 'BittleR':
            self.imgWiring = createImage(self.frameCalibButtons,
                                         resourcePath + config.model_ + self.boardVersion[1] + 'Wire.jpeg',
                                         self.paraemterSet['imageW'])
        else:
            self.imgWiring = createImage(self.frameCalibButtons,
                                         resourcePath + config.model_ + 'Wire.jpeg',
                                         self.paraemterSet['imageW'])

        self.imgWiring.grid(row=0, column=0, rowspan=5, columnspan=3)
        Hovertip(self.imgWiring, txt('tipImgWiring'))

        self.imgPosture = createImage(self.frameCalibButtons, resourcePath + self.model + 'Ruler.jpeg', self.paraemterSet['imageW'])
        self.imgPosture.grid(row=7, column=0, rowspan=3, columnspan=3)

        for i in range(16):
            if i < 4:
                tickDirection = 1
                cSPAN = 3
                if i < 2:
                    ROW = 0
                else:
                    ROW = self.paraemterSet['rowJoint1']

                if 0 < i < 3:
                    COL = 4
                else:
                    COL = 0
                rSPAN = 1
                ORI = HORIZONTAL
                LEN = self.paraemterSet['sliderW']
                ALIGN = 'we'

            else:
                tickDirection = -1
                leftQ = (i - 1) % 4 > 1
                frontQ = i % 4 < 2
                upperQ = i / 4 < 3

                rSPAN = 3
                ROW = self.paraemterSet['rowJoint2'] + (1 - frontQ) * (rSPAN + 2)

                if leftQ:
                    COL = 3 - i // 4
                else:
                    COL = 3 + i // 4
                ORI = VERTICAL
                LEN = self.paraemterSet['sliderLen']
                ALIGN = 'sw'
            stt = NORMAL
            if i in NaJoints[self.model]:
                clr = 'light yellow'
            else:
                clr = 'yellow'
            if i in range(8, 12):
                sideLabel = txt(sideNames[i % 8]) + '\n'
            else:
                sideLabel = ''
            label = Label(self.winCalib,
                          text=sideLabel + '(' + str(i) + ')\n' + txt(scaleNames[i]))

            value = DoubleVar()
            sliderBar = Scale(self.winCalib, state=stt, fg='blue', bg=clr, variable=value, orient=ORI,
                              borderwidth=2, relief='flat', width=8, from_=-25 * tickDirection, to=25 * tickDirection,
                              length=LEN, tickinterval=10, resolution=1, repeatdelay=100, repeatinterval=100,
                              command=lambda value, idx=i: self.setCalib(idx, value))
            self.calibSliders.append(sliderBar)
            label.grid(row=ROW, column=COL, columnspan=cSPAN, pady=2, sticky=ALIGN)
            sliderBar.grid(row=ROW + 1, column=COL, rowspan=rSPAN, columnspan=cSPAN, sticky=ALIGN)
        time.sleep(3) # wait for the robot to reboot
        self.calibFun('c')
        self.winCalib.update()
        self.calibratorReady = True
        self.winCalib.protocol('WM_DELETE_WINDOW', self.closeCalib)
        self.winCalib.focus_force()    # force the main interface to get focus
        self.winCalib.mainloop()

    def calibFun(self, cmd):
#        global ports
        imageW = self.paraemterSet['imageW']

        self.imgPosture.destroy()
        if cmd == 'c':
            self.imgPosture = createImage(self.frameCalibButtons, resourcePath + self.model + 'Ruler.jpeg', imageW)
            result = send(goodPorts, ['c', 0])
            if result != -1:
                offsets = result[1]
                printH('re',result)
                printH('of',offsets)
                idx = offsets.find(',')
                l1 = offsets[:idx].split()[-1]
                offsets = ''.join(offsets[idx + 1:].split()).split(',')[:15]
                offsets.insert(0, l1)
            #                print(offsets)
            #                print(len(offsets))
            else:
                offsets = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ]
            for i in range(16):
                self.calibSliders[i].set(offsets[i])

        elif cmd == 'd':
            self.imgPosture = createImage(self.frameCalibButtons, resourcePath + self.model + 'Rest.jpeg', imageW)
            send(goodPorts, ['d', 0])
        elif cmd == 'balance':
            self.imgPosture = createImage(self.frameCalibButtons, resourcePath + self.model + 'Stand.jpeg', imageW)
            send(goodPorts, ['kbalance', 0])
        elif cmd == 'walk':
            self.imgPosture = createImage(self.frameCalibButtons, resourcePath + self.model + 'Walk.jpeg', imageW)
            send(goodPorts, ['kwkF', 0])
        self.imgPosture.grid(row=7, column=0, rowspan=3, columnspan=3)
        Hovertip(self.imgPosture, txt('tipImgPosture'))
        self.winCalib.update()

    def setCalib(self, idx, value):
        if self.calibratorReady:
#            global ports
            value = int(value)
            send(goodPorts, ['c', [idx, value], 0])

    def closeCalib(self):
        confirm = messagebox.askyesnocancel(title=None, message=txt('Do you want to save the offsets?'),
                                            default=messagebox.YES)
        if confirm is not None:
#            global ports
            if confirm:
                send(goodPorts, ['s', 0])
            else:
                send(goodPorts, ['a', 0])
            time.sleep(0.1)
            self.calibratorReady = False
            self.calibSliders.clear()
            self.winCalib.destroy()
            closeAllSerial(goodPorts)
            os._exit(0)
            
if __name__ == '__main__':
    goodPorts = {}
    try:
        #        time.sleep(2)
        #        if len(goodPorts)>0:
        #            t=threading.Thread(target=keepReadingSerial,args=(goodPorts,))
        #            t.start()
        model = "Bittle"
        Calibrator(model,language)
        closeAllSerial(goodPorts)
        os._exit(0)
    except Exception as e:
        logger.info("Exception")
        closeAllSerial(goodPorts)
        raise e

