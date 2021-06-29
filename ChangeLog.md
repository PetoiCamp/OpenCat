
# Change Log


## Jun 29, 2021
* OpenCat.ino will print out the model name (Bittle/Nybble) at booting up.
* Moved #define I2C_EEPROM to the begining OpenCat.h.

## Jun 7, 2021

* Adjusted the threshold voltage so that Bittle will keep beeping when the battery goes under 6.5V, a few moments before the battery power shuts off;
* Adjusted the gaits so that the elbow won't hit the body in the accelerated phase. Removed the running gait on the IR remote key "2" and replaced it with a faster trot. The original "check around" behavior on key "3" is assigned to key "2";
* Assigned a "play dead" trick to IR remote key "3". Bittle will fall on its back then roll back (if the gyro is activated);
* Removed the behavior when the robot is tilted at a large angle to avoid an occasional bug. Will put the behavior back if the bug can be fixed; 
* Added an auto detection code for a new sound&light sensor connected to the analog Grove pin. Some new automated behaviors are being developed. The code block won't be active if the sensor is not connected to the Analog Grove socket;


