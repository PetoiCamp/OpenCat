#!/opt/anaconda3/envs/petoi/bin/python3
# -*- coding: UTF-8 -*-
from commonVar import *

language = languageList['English']
parName = ['lensFactor','proportion','speed','pan','tilt','frontUpX','backUpX','frontDownX','backDownX','frontUpY','backUpY','frontDownY','backDownY','frontUp','backUp','frontDown','backDown']
lensFactor = 30

def txt(key):
    return language.get(key, textEN[key])
    
class Tuner:
    def __init__(self,model,lan):
        self.calibratorReady = False
        # global ports
        connectPort(goodPorts)
        # ports = goodPorts
        self.model = config.model_
        logger.info(f"The model is: {self.model}")
        global language
        language = lan
        self.winTuner = Tk()
        self.winTuner.title('Petoi R&D Tuner')
        self.winTuner.geometry('+200+30')
        self.winTuner.resizable(False, False)
        self.calibSliders = list()
        self.OSname = self.winTuner.call('tk', 'windowingsystem')
        if self.OSname == 'win32':
            self.calibButtonW = 8
        else:
            self.calibButtonW = 4

        self.calibratorReady = True
        self.setTuner(0, lensFactor)
        pars = self.showPara()
        for i in range(len(parName)):
            ORI = HORIZONTAL
            LEN = 200
            ALIGN = 'we'
            tickDirection = 1

            stt = NORMAL
            if i < 5:
                clr = 'light green'
                minScope = 0
                maxScope = 100
                interval = 20
                if i == 4:
                    minScope = -20
                    maxScope = 20
                    interval = 5
                    if self.model == 'Bittle':
                        stt = DISABLED
                        clr = 'light gray'
                    logger.debug(f"The stt is: {stt}")
            elif i<9:
                clr = 'light yellow'
                minScope = 0
                maxScope = 100
                interval = 20
            elif i<13:
                clr = 'light blue'
                minScope = 0
                maxScope = 100
                interval = 20
            else:
                clr = 'yellow'
                minScope = -100
                maxScope = 100
                interval = 25
            label = Label(self.winTuner,
                          text=parName[i])

            value = DoubleVar()
            sliderBar = Scale(self.winTuner, state=stt, fg='blue', bg=clr, variable=value, orient=ORI,
                              borderwidth=2, relief='flat', width=8, from_=minScope * tickDirection, to=maxScope * tickDirection,
                              length=LEN, tickinterval=interval, resolution=1, repeatdelay=100, repeatinterval=100,
                              command=lambda value, idx=i: self.setTuner(idx, value))
            sliderBar.set(pars[i])
            self.calibSliders.append(sliderBar)
            label.grid(row=i, column=0, pady=2, sticky=ALIGN)
            sliderBar.grid(row=i, column=1,  sticky=ALIGN)

        buttonC = tk.Button(self.winTuner, text=txt('Confirm'), command=self.showPara)
        buttonC.grid(row=i+1, columnspan=2, padx=10, pady=10, sticky=W + E)

        send(goodPorts, ['ksit', 0])
        self.winTuner.update()
        self.winTuner.protocol('WM_DELETE_WINDOW', self.closeTuner)
        self.winTuner.mainloop()


    def showPara(self):
        paraList = self.result[-1].split('\r\n')
        logger.debug(f"The para list is: {paraList}")
        resultStr = paraList[-2].replace('\t', ', ')[:-2]
        logger.info(f"The result string is: {resultStr}")
        pars = [int(x) for x in resultStr.split(',')]
        logger.debug(f"The pars are: {pars}")
        return pars


    def setTuner(self, idx, value):
        if self.calibratorReady:
            value = int(value)
            self.result = send(goodPorts, ['}', [idx, value], 0])
            logger.debug(f"The result list is: {self.result}")

    def closeTuner(self):
        confirm = messagebox.askyesnocancel(title=None, message='Quit?',
                                            default=messagebox.YES)
        if confirm is not None:
            time.sleep(0.1)
            self.calibratorReady = False
            self.calibSliders.clear()
            self.winTuner.destroy()
            
if __name__ == '__main__':
    goodPorts = {}
    try:
        Tuner(model,language)
        closeAllSerial(goodPorts)
        os._exit(0)
    except Exception as e:
        logger.info("Exception")
        closeAllSerial(goodPorts)
        raise e
