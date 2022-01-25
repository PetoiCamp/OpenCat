# OpenCat

OpenCat is the open-source Arduino and Raspberry Pi-based quadruped robotic pet framework developed by Petoi, the maker of futuristic programmable robotic pets.

![](https://github.com/PetoiCamp/NonCodeFiles/blob/master/gif/walk.gif?raw=true)

![](https://github.com/PetoiCamp/NonCodeFiles/blob/master/gif/run.gif?raw=true)

Inspired by Boston Dynamics' Big Dog, Dr. Rongzhong Li started the project in his dorm in 2016. After one year of R&D, he founded Petoi LLC and devoted all his resources to the startup. The goal is to foster collaboration in quadruped(four-legged) robotic research, education, and engineering development of agile and affordable quadruped robot pets, bring STEM concepts to the mass and inspire newcomers (including many kids and adults) to join the robotic AI revolution to create more applications.

![](https://github.com/PetoiCamp/NonCodeFiles/blob/master/gif/slope.gif?raw=true)

The project is still a complex system only for skilled makers, yet we want to share our design and work with the community by mass production and bring down the hardware and software costs. OpenCat has been deployed on Petoi's bionic palm-sized, realistic lifelike [cute robot cat Nybble](https://www.petoi.com/collections/robots/products/petoi-nybble-robot-cat?utm_source=github&utm_medium=code&utm_campaign=nybble) and [high-performance robot dog Bittle](https://www.petoi.com/collections/robots/products/petoi-bittle-robot-dog?utm_source=github&utm_medium=code&utm_campaign=bittle). We now have established a production line and can ship these [affordable robotic kits and accessories](https://www.petoi.com/store?utm_source=github&utm_medium=code&utm_campaign=store) worldwide.

This project provides a base open-source platform to create amazing programmable gaits, locomotion, and deployment of inverse kinematics quadruped robots via C/C++/Python programming languages.  Our users have also deployed [NVIDIA Issac simulations and reinforcement learning on our robots](https://www.youtube.com/playlist?list=PLHMFXft_rV6MWNGyofDzRhpatxZuUZMdg).

![](https://github.com/PetoiCamp/NonCodeFiles/blob/master/gif/stand.gif?raw=true)

![](https://github.com/PetoiCamp/NonCodeFiles/blob/master/gif/NybbleBalance.gif?raw=true)

We've successfully crowdfunded these two mini robot kits and shipped thousands of units worldwide.  With our customized Arduino board and servos coordinating all instinctive and sophisticated movements(walking, running, jumping, backflipping), one can clip on various sensors to bring in perception and inject artificial intelligence capabilities by mounting a Raspberry Pi or other AI chips(such as Nvidia Jetson Nano) through wired/wireless connections.  Please see [Petoi FAQs](https://www.petoi.com/pages/faq?utm_source=github&utm_medium=code&utm_campaign=faq) for more info.

Also, Check out [all of the OpenCat and Petoi robot user showcases](https://www.petoi.camp/forum/showcase).


![](https://github.com/PetoiCamp/NonCodeFiles/blob/master/gif/ball.gif?raw=true)

OpenCat software works on both Nybble and Bittle, controlled by NyBoard based on ATmega328P. To run the code on our robot models, first change the model and board definition in **OpenCat.h**, then upload **WriteInstinct.ino**.

```
#include "InstinctBittle.h" //activate the correct header file according to your model
//#include "InstinctNybble.h"

//#define NyBoard_V0_1
//#define NyBoard_V0_2
#define NyBoard_V1_0
```

Set **No line ending** in the serial monitor and baudrate as **115200** (or **57600** for NyBoard V0_\*). Enter three capitalized **Y** after the prompts and wait for the MPU to calibrate. Then upload **OpenCat.ino** as the main functional code.

More detailed documentation can be found at the [Petoi Doc Center](https://docs.petoi.com).

You can use our mobile app to control the robots as well:
* IOS: [App Store](https://apps.apple.com/us/app/petoi/id1581548095)
* Android: [Google Play](https://play.google.com/store/apps/details?id=com.petoi.petoiapp)

For updates:
* star this repository to receive timely notifications on changes.
* visit www.petoi.com and subscribe to our official newsletters for project announcements. We also host a forum at [petoi.camp](https://www.petoi.com/forum).
* follow us on [Twitter](https://twitter.com/petoicamp), [Instagram](https://www.instagram.com/petoicamp/), and [YouTube channel](https://www.youtube.com/c/rongzhongli) for fun videos and community activities.

![](https://github.com/PetoiCamp/NonCodeFiles/blob/master/gif/backflip.gif?raw=true)

[Advanced tutorials made by users](https://www.youtube.com/playlist?list=PLHMFXft_rV6MWNGyofDzRhpatxZuUZMdg)
[Review, Open-box, and Demos by Users](https://www.youtube.com/playlist?list=PLHMFXft_rV6PSS3Qu5yQ-0iPW-ohu1sM3)

The [old repository for OpenCat](https://github.com/PetoiCamp/OpenCat-Old) is too redundant with large image logs and will be obsolete after adding compatibility notes in the documentation.
