# OpenCat

OpenCat is the open-source Arduino and Raspberry Pi-based robotic pet framework developed by Petoi, the maker of futuristic programmable robotic pets.

![](https://github.com/PetoiCamp/NonCodeFiles/blob/master/gif/walk.gif?raw=true)

Inspired by Boston Dynamics' Big Dog, Rongzhong Li started the project in his dorm in 2016. After one year of R&D, he founded Petoi LLC and devoted all his resources to the startup. The goal is to foster collaboration in quadruped robotic research and development of agile and affordable quadruped robot pets, bring STEM concepts to the mass and inspire newcomers (including a lot of kids) to join the robotic AI revolution.

![](https://github.com/PetoiCamp/NonCodeFiles/blob/master/gif/slope.gif?raw=true)

The project is still a complex system only for skilled makers, yet we want to share our work with the community by mass production and bringing down the hardware and software cost. OpenCat has been deployed on Petoi's palm-sized, realistic lifelike robot cat [Nybble](www.igg.me/at/nybble) and robot dog [Bittle](www.igg.me/at/bittle). We've successfully crowdfunded these two robots and shipped thousands of units worldwide.  With our customized Arduino board coordinating all instinctive and sophisticated movements, one can clip on various sensors to bring in perception and inject artificial intelligence capabilities by mounting a Raspberry Pi or other AI chips through wired/wireless connections.

![](https://github.com/PetoiCamp/NonCodeFiles/blob/master/gif/ball.gif?raw=true)

This repository works on both Nybble and Bittle controlled by NyBoard based on ATmega328P. To run the code on our robot models, first change the model and board definition in **OpenCat.h**, then upload **WriteInstinct.ino**.

```
#include "InstinctBittle.h" //activate the correct header file according to your model
//#include "InstinctNybble.h"

//#define NyBoard_V0_1
//#define NyBoard_V0_2
#define NyBoard_V1_0
```

In the serial monitor, set **No line ending** and baudrate as **115200** (or **57600** for NyBoard V0_\*). Enter three capitalized **Y** after the prompts and wait for the MPU to calibrate. Then upload **OpenCat.ino** as the main functional code. 

More detailed documentation can be found at the [Petoi Doc Center](https://docs.petoi.com).

For updates:
* star this repository to receive timely notifications on changes.
* visit www.petoi.com and subscribe to our official newsletters for project announcements.
* follow us on [Twitter](https://twitter.com/petoicamp), [Instagram](https://www.instagram.com/petoicamp/), and [YouTube channel](https://www.youtube.com/c/rongzhongli) for fun videos and community activities.

![](https://github.com/PetoiCamp/NonCodeFiles/blob/master/gif/backflip.gif?raw=true)

The [old repository for OpenCat](https://github.com/PetoiCamp/OpenCat-Old) is too redundant with large image logs and will be obsolete after we add compatibility notes in the documentation.
