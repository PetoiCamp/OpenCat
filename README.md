# OpenCat

OpenCat is the open-source Arduino and Raspberry Pi-based robotic pet framework developed by Petoi, the maker of futuristic programmable robotic pets.

The goal is to foster collaboration in quadruped robotic research and development of agile and affordable quadruped robot pets, and to bring STEM concepts to the mass and inspire newcomers (including a lot of kids) to join the robotic AI revolution.

The project is still a complex system only for skilled makers, yet we want to share our work with the community by mass production and bringing down the hardware and software cost.

OpenCat has been deployed on Petoi's palm-sized, realistic lifelike robot cat Nybble (www.igg.me/at/nybble) and robot dog Bittle (www.igg.me/at/bittle) that can run, walk, and self-balance like a real animal.  We've successfully crowdfunded these two robots and bringing them to the market.  With our customized Arduino board coordinating all instinctive and sophisticated movements, one can clip on various sensors to bring in perception and inject artificial intelligence capabilities by mounting a Raspberry Pi or other AI chips through wired/wireless connections.

This repository works on both Nybble and Bittle controled by NyBoard based on ATmega328P. To run the code on our robot models, first change the model and board definition in **OpenCat.h**, then upload **WriteInstinct.ino**.

```
#include "InstinctBittle.h" //activate the correct header file according to your model
//#include "InstinctNybble.h"

//#define NyBoard_V0_1
//#define NyBoard_V0_2
#define NyBoard_V1_0
```

In the serial monitor, set **No line ending** and baudrate as **115200** (or **57600** for NyBoard V0_\*). Enter three capitalized **Y** after the prompts and wait for the MPU to calibrate. Then upload **OpenCat.ino** as the main functional code. 

For updates:
* star this repository to receive timely notifications on changes.
* visit www.petoi.com and subscribe to our official newsletters for project announcements.
* follow us on [Twitter](https://twitter.com/petoicamp), [Instagram](https://www.instagram.com/petoicamp/), and [YouTube channel](https://www.youtube.com/c/rongzhongli) for fun videos and community activities.

The [old repository for OpenCat](https://github.com/PetoiCamp/OpenCat-Old) is too redundent with large image logs and will be obsolete after we add compatibility notes in the documentation.
