#!/opt/anaconda3/envs/petoi/bin/python3
# -*- coding: UTF-8 -*-
from commonVar import *

language = languageList['English']

def txt(key):
    return language.get(key, textEN[key])
    
class Calibrator:
    def __init__(self,model,lan):
        self.calibratorReady = False
        global ports
        connectPort(goodPorts)
        ports = goodPorts
        self.model = model
        global language
        language = lan
        self.winCalib = Tk()
        self.winCalib.title(txt('calibTitle'))
        self.winCalib.geometry('+200+100')
        self.winCalib.resizable(False, False)
        self.calibSliders = list()
        self.OSname = self.winCalib.call('tk', 'windowingsystem')
        if self.OSname == 'win32':
            self.calibButtonW = 8
        else:
            self.calibButtonW = 6
        self.frameCalibButtons = Frame(self.winCalib)
        self.frameCalibButtons.grid(row=0, column=3, rowspan=13)
        calibButton = Button(self.frameCalibButtons, text=txt('Calibration'), width=self.calibButtonW,command=lambda cmd='c': self.calibFun(cmd))
        standButton = Button(self.frameCalibButtons, text=txt('Stand Up'), width=self.calibButtonW, command=lambda cmd='balance': self.calibFun(cmd))
        restButton = Button(self.frameCalibButtons, text=txt('Rest'), width=self.calibButtonW, command=lambda cmd='d': self.calibFun(cmd))
        saveButton = Button(self.frameCalibButtons, text=txt('Save'), width=self.calibButtonW, command=lambda: send(ports, ['s', 0]))
        abortButton = Button(self.frameCalibButtons, text=txt('Abort'), width=self.calibButtonW, command=lambda: send(ports, ['a', 0]))
        quitButton = Button(self.frameCalibButtons, text=txt('Quit'), width=self.calibButtonW, command=self.closeCalib)
        calibButton.grid(row=6, column=0)
        standButton.grid(row=6, column=1)
        restButton.grid(row=6, column=2)
        saveButton.grid(row=11, column=0)
        abortButton.grid(row=11, column=1)
        quitButton.grid(row=11, column=2)

        imageW = 250
        self.imgWiring = createImage(self.frameCalibButtons, 'resources/' + self.model + 'Wire.jpeg', imageW)
        self.imgWiring.grid(row=0, column=0, rowspan=5, columnspan=3)
        self.imgPosture = createImage(self.frameCalibButtons, 'resources/' + self.model + 'Ruler.jpeg', imageW)
        self.imgPosture.grid(row=7, column=0, rowspan=3, columnspan=3)

        for i in range(16):
            if i < 4:
                tickDirection = 1
                cSPAN = 3
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
                LEN = 200
                ALIGN = 'we'

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
                    COL = 3 + i // 4
                ORI = VERTICAL
                LEN = 150
                ALIGN = 'sw'

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
            label = Label(self.winCalib,
                          text=sideLabel + '(' + str(i) + ')\n' + txt(scaleNames[i]))

            value = DoubleVar()
            sliderBar = Scale(self.winCalib, state=stt, fg='blue', bg=clr, variable=value, orient=ORI,
                              borderwidth=2, relief='flat', width=8, from_=-10 * tickDirection, to=10 * tickDirection,
                              length=LEN, tickinterval=5, resolution=1, repeatdelay=100, repeatinterval=100,
                              command=lambda value, idx=i: self.setCalib(idx, value))
            self.calibSliders.append(sliderBar)
            label.grid(row=ROW, column=COL, columnspan=cSPAN, pady=2, sticky=ALIGN)
            sliderBar.grid(row=ROW + 1, column=COL, rowspan=rSPAN, columnspan=cSPAN, sticky=ALIGN)
        self.calibFun('c')
        self.winCalib.update()
        self.calibratorReady = True
        self.winCalib.protocol('WM_DELETE_WINDOW', self.closeCalib)
        self.winCalib.mainloop()

    def calibFun(self, cmd):
        global ports
        imageW = 250
        self.imgPosture.destroy()
        if cmd == 'c':
            self.imgPosture = createImage(self.frameCalibButtons, 'resources/' + self.model + 'Ruler.jpeg', imageW)
            result = send(ports, ['c', 0])
            if result != -1:
                offsets = result[1]
                #                print(offsets)
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
            send(ports, ['d', 0])
        elif cmd == 'balance':
            self.imgPosture = createImage(self.frameCalibButtons, resourcePath + self.model + 'Stand.jpeg', imageW)
            send(ports, ['kbalance', 0])
        self.imgPosture.grid(row=7, column=0, rowspan=3, columnspan=3)
        self.winCalib.update()

    def setCalib(self, idx, value):
        if self.calibratorReady:
            global ports
            value = int(value)
            send(ports, ['c', [idx, value], 0])

    def closeCalib(self):
        confirm = messagebox.askyesnocancel(title=None, message=txt('Do you want to save the offsets?'),
                                            default=messagebox.YES)
        if confirm is not None:
            global ports
            if confirm:
                send(ports, ['s', 0])
            else:
                send(ports, ['a', 0])
            time.sleep(0.1)
            self.calibratorReady = False
            self.calibSliders.clear()
            self.winCalib.destroy()
            
if __name__ == '__main__':
    goodPorts = {}
    try:
        #        time.sleep(2)
        #        if len(goodPorts)>0:
        #            t=threading.Thread(target=keepReadingSerial,args=(goodPorts,))
        #            t.start()
        Calibrator(model,language)
        closeAllSerial(goodPorts)
        os._exit(0)
    except Exception as e:
        logger.info("Exception")
        closeAllSerial(goodPorts)
        raise e

