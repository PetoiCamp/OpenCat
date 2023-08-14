# Petoi Desktop App 
## The source codes are written with Tinker in Python3 

The major function modules are the [Firmware Uploader](https://docs.petoi.com/desktop-app/firmware-uploader), [Joint Calibrator](https://docs.petoi.com/desktop-app/calibrator), and [Skill Composer](https://docs.petoi.com/desktop-app/skill-composer). 

**UI.py** is the general entry for all the modules:

- UI.py

-> FirmwareUploader.py

-> Calibrator.py

-> SkillComposer.py

![gui](https://github.com/PetoiCamp/NonCodeFiles/blob/master/img/GUI.jpeg)

**translate.py** provides multi-language support for the UI. You may help to translate the UI into your language. 

Before running the app, you need to use the included USB adapter or the Bluetooth dongle to connect to a Petoi robot. 

## Pre-compiled binaries

You can download the [pre-compiled binaries](https://github.com/PetoiCamp/OpenCat/releases).

Windows users:
Double-click the exe file to run the app. Keep all the files in the Win folder when moving the app. 

Mac users:
Double-click the dmg file and drag the app to the Application folder. Go to the Application folder and right-click the Petoi icon. Hold the **shift** key, then click **Open**.


## Run the app from the Terminal 
In the case of compatibility issues, or if you want to modify the source and test, you can also run the code from the Terminal. 

The Terminal is a built-in interface on Mac or Linux machines. The equivalent environment on Windows machines is called the Command-Line Tool (CMD). 
It's recommended that you install [Anaconda](https://www.anaconda.com/) to manage your Python environment. It can also provide the Powershell as a Terminal for older Windows machines. 

Depending on your existing Python configuration, you may need to upgrade to Python3 and install the following libraries:

- pyserial
- pillow

You can install them by entering ```pip3 install pyserial pillow``` in the Terminal or use the package manager in Anaconda. 

To run the code:
1. In the Terminal, use the ```cd``` command to navigate to the ```OpenCat/pyUI/``` folder. You can use the Tab key to auto-complete the path name. 
2. After entering the pyUI/ folder, enter ```ls``` and ensure you can see the UI.py and other python source codes listed. 
3. Enter ```python3 UI.py```.

