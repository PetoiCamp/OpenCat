# OpenCat

OpenCat is the open-source Arduino and Raspberry Pi-based quadruped robotic pet framework developed by Petoi, the maker of futuristic programmable robotic pets.

![](https://github.com/PetoiCamp/NonCodeFiles/blob/master/gif/walk.gif?raw=true)

![](https://github.com/PetoiCamp/NonCodeFiles/blob/master/gif/run.gif?raw=true)

Inspired by Boston Dynamics' Big Dog, Dr. Rongzhong Li started the project in his dorm in 2016. After one year of R&D, he founded Petoi LLC and devoted all his resources to the startup. The goal is to foster collaboration in quadruped(four-legged) robotic research, education, and engineering development of agile and affordable quadruped robot pets, bring STEM concepts to the mass and inspire newcomers (including many kids and adults) to join the robotic AI revolution to create more applications.

![](https://github.com/PetoiCamp/NonCodeFiles/blob/master/gif/slope.gif?raw=true)

The project is still a complex system only for skilled makers, yet we want to share our design and work with the community by mass production and bring down the hardware and software costs. OpenCat has been deployed on Petoi's bionic palm-sized, realistic lifelike [cute robot cat Nybble](https://www.petoi.com/collections/robots/products/petoi-nybble-robot-cat?utm_source=github&utm_medium=code&utm_campaign=nybble) and [high-performance robot dog Bittle](https://www.petoi.com/collections/robots/products/petoi-bittle-robot-dog?utm_source=github&utm_medium=code&utm_campaign=bittle). We now have established a production line and can ship these [affordable robotic kits and accessories](https://www.petoi.com/store?utm_source=github&utm_medium=code&utm_campaign=store) worldwide.

This project provides a base open-source platform to create amazing programmable gaits, locomotion, and deployment of inverse kinematics quadruped robots and bring simulations to the real world via C/C++/Python programming languages.  Our users have deployed [NVIDIA Issac simulations and reinforcement learning on our robots](https://www.youtube.com/playlist?list=PLHMFXft_rV6MWNGyofDzRhpatxZuUZMdg). Our users have also successfully deployed OpenCat on their DIY 3D-print robot pets.

![](https://github.com/PetoiCamp/NonCodeFiles/blob/master/gif/stand.gif?raw=true)

![](https://github.com/PetoiCamp/NonCodeFiles/blob/master/gif/NybbleBalance.gif?raw=true)

We've successfully crowdfunded these two mini robot kits and shipped thousands of units worldwide.  With our customized Arduino board and servos coordinating all instinctive and sophisticated movements(walking, running, jumping, backflipping), one can clip on various sensors to bring in perception and inject artificial intelligence capabilities by mounting a Raspberry Pi or other AI chips(such as Nvidia Jetson Nano) through wired/wireless connections.  Please see [Petoi FAQs](https://www.petoi.com/pages/faq?utm_source=github&utm_medium=code&utm_campaign=faq) for more info.

Also, Check out [all of the OpenCat and Petoi robot user showcases](https://www.petoi.camp/forum/showcase).


![](https://github.com/PetoiCamp/NonCodeFiles/blob/master/gif/ball.gif?raw=true)

## Setup  Process:

OpenCat software works on both Nybble and Bittle, controlled by NyBoard based on ATmega328P. More detailed documentation can be found at the [Petoi Doc Center](https://docs.petoi.com).

To configure the board:

1. Open the file OpenCat.ino, select your robot and board version.
```cpp
#define BITTLE    //Petoi 9 DOF robot dog: 1 on head + 8 on leg
#define NyBoard_V1_0
```

2. Comment out ```#define MAIN_SKETCH``` and upload. Upload and follow the serial prompts to proceed
```cpp
// #define MAIN_SKETCH
```

3. If you activate ```#define AUTO_INIT```, the program will automatically set up without prompts.

4. Plug the USB uploader to the NyBoard and install the driver if no USB port is found under Arduino -> Tools -> Port (add pic)

5. Press the upload button (add pic)

6. Open the serial monitor (add pic)

Set the serial monitor as **no line ending** and **115200** baud rate (add pic)
The serial prompts:
```
Reset joint offsets? (Y/n)
Y    (Input ‘Y’ and hit enter, if you have never calibrated the joint servos or want to redo calibration)

Calibrate the IMU? (Y/n): 
Y    (Input ‘Y’ and hit enter, if you have never calibrated the IMU (Inertial Measurement Unit) or want to redo calibration)
```

Put the robot flat on the table and don't touch it. The robot will read hundreds of sensor data and save the offsets. It will beep when the calibration finishes. 

When the serial monitor prints "Ready!", you can close the serial monitor to do the next step.

7. Uncomment ```#define MAIN_SKETCH``` to make it active. Then upload.
```cpp
#define MAIN_SKETCH
```

This time the code becomes the normal program for the major functionalities. 

When the serial monitor prints "Ready!", you can close the serial monitor to do the next step.

8. If you have never calibrated the joints’ offsets or reset the offsets in Step2, you need to do calibrate them. If you boot up the robot with one side up, it will enter the calibration state automatically for you to install the legs. Otherwise, it will enter the normal rest state. 

9. You can use the serial monitor to calibrate it directly. Or you may plug in the Bluetooth dongle, and use the Petoi app (on Android/iOS) for a more user-friendly interface. The mobile app is available on:

* IOS: [App Store](https://apps.apple.com/us/app/petoi/id1581548095)
* Android: [Google Play](https://play.google.com/store/apps/details?id=com.petoi.petoiapp)

You can refer to the calibration section in the user manual (https://bittle.petoi.com/6-calibration) and Guide for the Petoi App(https://docs.petoi.com/app-guide).

10. you can use the infrared remote or other applications (such as the Petoi App, python, serial monitor ... etc.) to play with the robot (https://bittle.petoi.com/7-play-with-bittle).

For updates:
* star this repository to receive timely notifications on changes.
* visit www.petoi.com and subscribe to our official newsletters for project announcements. We also host a forum at [petoi.camp](https://www.petoi.com/forum).
* follow us on [Twitter](https://twitter.com/petoicamp), [Instagram](https://www.instagram.com/petoicamp/), and [YouTube channel](https://www.youtube.com/c/rongzhongli) for fun videos and community activities.

![](https://github.com/PetoiCamp/NonCodeFiles/blob/master/gif/backflip.gif?raw=true)

[Advanced tutorials made by users](https://www.youtube.com/playlist?list=PLHMFXft_rV6MWNGyofDzRhpatxZuUZMdg)

[Review, open-box, and demos by users](https://www.youtube.com/playlist?list=PLHMFXft_rV6PSS3Qu5yQ-0iPW-ohu1sM3)

The [old repository for OpenCat](https://github.com/PetoiCamp/OpenCat-Old) is too redundant with large image logs and will be obsolete after adding compatibility notes in the documentation.
