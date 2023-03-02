/*
   Choose communication mode define here:
      I2C_MODE    : I2C mode, default pin: MU_SDA <==> ARDUINO_SDA, MU_SCL <==> ARDUINO_SCL
      SERIAL_MODE : Serial mode, default pin: MU_TX <==> ARDUINO_PIN3, MU_RX <==> ARDUINO_PIN2
*/
#define I2C_MODE
//#define SERIAL_MODE

/*
   Choose MU address here: 0x60, 0x61, 0x62, 0x63
          default address: 0x60
*/
#define MU_ADDRESS 0x50  //in later versions we set the I2C device to 0x50, 0x51, 0x52, 0x53
#define ALT_MU_ADDRESS 0x60

#include <MuVisionSensor.h>  //you need to download the library https://github.com/mu-opensource/MuVisionSensor3 into your ~/Documents/Arduino/libraries/
#ifdef I2C_MODE
#include <Wire.h>
#endif
#ifdef SERIAL_MODE
#include <SoftwareSerial.h>
#define TX_PIN 2
#define RX_PIN 3
SoftwareSerial mySerial(RX_PIN, TX_PIN);
#endif

MuVisionSensor *Mu;



int xCoord, yCoord;              //the x y returned by the sensor
int xDiff, yDiff;                //the scaled distance from the center of the frame
int currentX = 0, currentY = 0;  //the current x y of the camera's direction in the world coordinate
int imgRange = 100;              //the frame size 0~100 on X and Y direction
int skip = 1;                    //, counter; //an efforts to reduce motion frequency without using delay. set skip >1 to take effect
int i2cdelay = 3;
long noResultTime = 0;
MuVisionType object[] = { VISION_BODY_DETECT, VISION_BALL_DETECT };
String objectName[] = { "body", "ball" };
int objectIdx = 0;
bool firstConnection = true;
void cameraSetup() {
  // put your setup code here, to run once:
  uint8_t err;
  // initialized MU on the I2C port
  do {
    MuVisionSensor *Mu0 = new MuVisionSensor(MU_ADDRESS);
    err = Mu0->begin(&Wire);

    if (err == MU_OK) {
      PTLF("MU initialized at 0x50");
      Mu = Mu0;
    } else {
      if (firstConnection) {
        firstConnection = false;
        PTLF("Failed to initialize the camera!");
        PTLF("Set the four dial switches on the camera as v ^ v v (the second switch dialed up to I2C)");
        PTLF("Then connect the camera to the I2C Grove socket with SDA and SCL pins!");
        PTLF("The battery should be turned on to drive the servos");
      }
      delete Mu0;
      MuVisionSensor *Mu1 = new MuVisionSensor(ALT_MU_ADDRESS);
      err = Mu1->begin(&Wire);
      if (err == MU_OK) {
        PTLF("MU initialized at 0x60");
        Mu = Mu1;
      } else {
        delete Mu1;
      }
    }
    delay(1000);
  } while (err != MU_OK);
  //  shutServos();
  //  counter = 0;
  //  motion.loadBySkillName("rest");
  //  transform(motion.dutyAngles);

  (*Mu).VisionBegin(object[objectIdx]);
  noResultTime = millis();
}

void read_camera() {
  // read result
  bool objectDetected[] = { false, false };
  transformSpeed = 2;
  for (int test = 0; test < sizeof(object) / 2; test++)                                                   //switch between ball and body
    if (test == objectIdx && (objectDetected[objectIdx] = (*Mu).GetValue(object[objectIdx], kStatus))) {  // update vision result and get status, 0: undetected, other:
      //      PTL(objectName[objectIdx]);
      noResultTime = millis();  //update the timer
      xCoord = (int)(*Mu).GetValue(object[objectIdx], kXValue);
      yCoord = (int)(*Mu).GetValue(object[objectIdx], kYValue);

      //      PT(xCoord);       // get vision result: x axes value
      //      PT('\t');
      //      PTL(yCoord);       // get vision result: y axes value
      //      PT("width = ");

      //      if (objectIdx == 1)
      //        switch ((*Mu).GetValue(VISION_BALL_DETECT, kLabel)) {              // get vision result: label value
      //          case MU_BALL_TABLE_TENNIS:
      //            PTLF("table tennis");
      //            break;
      //          case MU_BALL_TENNIS:
      //            PTLF("tennis");
      //            break;
      //          default:
      //            PTLF("unknow ball type");
      //            break;
      //        }
      //      delay(i2cdelay);
    }

  if (objectDetected[0] || objectDetected[1]) {
    //    if (!(counter % skip)) {
    xDiff = max(min((xCoord - imgRange / 2) / 3, 32), -32);
    yDiff = max(min((yCoord - imgRange / 2) / 3, 24), -24);
    if (abs(xDiff) > 2 || abs(yDiff) > 2) {

      currentX = max(min(currentX - xDiff, 90), -90);
      currentY = max(min(currentY - yDiff, 75), -75);


      int allParameter[DOF] = { currentX, 0, 0, 0,
                                0, 0, 0, 0,
                                60 - currentY / 2 + currentX / 6, 60 - currentY / 2 - currentX / 6, 90 + currentY / 3 - currentX / 8, 90 + currentY / 3 + currentX / 8,
                                10 + currentY / 1.2 - currentX / 2, 10 + currentY / 1.2 + currentX / 2, -30 - currentY / 3 + currentX / 4, -30 - currentY / 3 - currentX / 4 };
      //      transform(a, 4);
      cmdLen = DOF;
      token = T_LISTED_BIN;
      for (byte i = 0; i < cmdLen; i++)
        newCmd[i] = (int8_t)min(max(allParameter[i], -125), 125);
      newCmd[cmdLen] = '~';
      transformSpeed = 16;
      newCmdIdx = 6;
      //      printList(newCmd);
      //    }
    }
  } else if (millis() - noResultTime > 2000) {  // if no object is detected for 2 seconds, switch object
    objectIdx = (objectIdx + 1) % (sizeof(object) / 2);
    (*Mu).VisionBegin(object[objectIdx]);
    PTL(objectName[objectIdx]);
    noResultTime = millis();
  }
  //    PT("fps = ");
  //    PTL(1000/(millis()-time_start));
  //    PTL();
}
