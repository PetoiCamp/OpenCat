#!/opt/anaconda3/envs/petoi/bin/python3
# -*- coding: UTF-8 -*-

# Rongzhong Li
# Petoi LLC
# May.22nd, 2022


from FlashUploader import *
from SkillComposer import *
from Calibrator import *
from commonVar import *

language = languageList['English']
apps = ['Firmware Uploader','Joint Calibrator','Skill Composer','Task Scheduler']
def txt(key):
    return language.get(key, textEN[key])
class UI:
    def __init__(self):
        global model
        global language
        try:
            with open("./defaultConfig.txt", "r") as f:
                lines = f.readlines()
                self.defaultPath = lines[0][:-1]
                model = lines[1][:-1]
                self.defaultLan = lines[2][:-1]
                f.close()
                
        except Exception as e:
            print ('Create configuration file')
            self.defaultPath = './release'
            model = 'Bittle'
            self.defaultLan = 'English'
#            raise e
        
        language = languageList[self.defaultLan]
                
        self.window = Tk()
        self.ready = False
        self.window.geometry('+100+10')
        self.window.resizable(False, False)
        self.myFont = tkFont.Font(
        family='Times New Roman', size=20, weight='bold')
        self.window.title(txt('uiTitle'))
        self.createMenu()
        bw = 18
        self.modelLabel = Label(self.window,text = model,font=self.myFont)
        self.modelLabel.grid(row = 0, column = 0,pady = 10)
        for i in range(len(apps)):
            Button(self.window,text = txt(apps[i]),font=self.myFont,width = bw,relief = 'raised',command = lambda app = apps[i]:self.utility(app)).grid(row = 1+i, column = 0,padx=10, pady=(0,10))
        
        self.ready = True
        self.window.protocol('WM_DELETE_WINDOW', self.on_closing)
        self.window.update()
        self.window.mainloop()
        
        
    def createMenu(self):
        self.menubar = Menu(self.window, background='#ff8000', foreground='black', activebackground='white',
                            activeforeground='black')
        file = Menu(self.menubar, tearoff=0, background='#ffcc99', foreground='black')
        for key in NaJoints:
            file.add_command(label=key, command=lambda model=key: self.changeModel(model))
        self.menubar.add_cascade(label=txt('Model'), menu=file)

        lan = Menu(self.menubar, tearoff=0)
        for l in languageList:
            lan.add_command(label=languageList[l]['lanOption'], command=lambda lanChoice=l: self.changeLan(lanChoice))
        self.menubar.add_cascade(label=txt('lanMenu'), menu=lan)

        helpMenu = Menu(self.menubar, tearoff=0)
        helpMenu.add_command(label=txt('About'), command=self.showAbout)
        self.menubar.add_cascade(label=txt('Help'), menu=helpMenu)

        self.window.config(menu=self.menubar)

    def changeModel(self, modelName):
        global model
        model = copy.deepcopy(modelName)
        self.modelLabel.configure(text = model)
        print(model)
        
    def changeLan(self, l):
        global language
        if self.ready and txt('lan') != l:
            self.defaultLan = l
            print(self.defaultLan)
            language = copy.deepcopy(languageList[l])
            self.menubar.destroy()
            self.createMenu()
            self.window.title(txt('uiTitle'))
            for i in range(4):
                self.window.winfo_children()[1+i].config(text = txt(apps[i]))
           
    def utility(self,app):
        with open("./defaultConfig.txt", "w") as f:
            f.write(self.defaultPath+'\n'+model+'\n' + self.defaultLan+'\n')
        self.window.destroy()
        if app == 'Firmware Uploader':
            Uploader(model, language)
        elif app == 'Joint Calibrator':
            Calibrator(model, language)
        elif app =='Skill Composer':
            SkillComposer(model, language)
        elif app == 'Task Scheduler':
            print('schedule')
                
    def showAbout(self):
        messagebox.showinfo('Petoi Controller UI',
                            u'Petoi Controller for OpenCat\nOpen Source on GitHub\nCopyright © Petoi LLC\nwww.petoi.com')
        
    def on_closing(self):
        if messagebox.askokcancel(txt('Quit'), txt('Do you want to quit?')):
            with open("./defaultConfig.txt", "w") as f:
                f.write(self.defaultPath+'\n'+model+'\n' + self.defaultLan+'\n')
            self.window.destroy()
        
if __name__ == '__main__':

    UI()

    
        
