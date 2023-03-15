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



int xCoord, yCoord, width, widthCounter;  //the x y returned by the sensor
int xDiff, yDiff;                         //the scaled distance from the center of the frame
int currentX = 0, currentY = 0;           //the current x y of the camera's direction in the world coordinate
int imgRangeX = 100;                      //the frame size 0~100 on X and Y direction
int imgRangeY = 100;

int skip = 1;  //, counter; //an efforts to reduce motion frequency without using delay. set skip >1 to take effect
int i2cdelay = 3;
long noResultTime = 0;
MuVisionType object[] = { VISION_BODY_DETECT, VISION_BALL_DETECT };
String objectName[] = { "body", "ball" };
int objectIdx = 0;
int lastBallType;
bool firstConnection = true;

int f = 3;
int p = 1;
int sp = 4;
int u1 = 6;
int u2 = 8;
int d1 = 2;
int d2 = 8;


void cameraSetup() {
  par[0] = &f;
  par[1] = &p;
  par[2] = &sp;
  par[3] = &u1;
  par[4] = &u2;
  par[5] = &d1;
  par[6] = &d2;
  transformSpeed = 0;
  widthCounter = 0;

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
  if ((*Mu).GetValue(object[objectIdx], kStatus)) {  // update vision result and get status, 0: undetected, other:
    //      PTL(objectName[objectIdx]);
    noResultTime = millis();  //update the timer
    width = (int)(*Mu).GetValue(object[objectIdx], kWidthValue);
    // height = (int)(*Mu).GetValue(VISION_BODY_DETECT, kHeightValue);
    //  PT("size = ");
    // PT(width);
    // PT('\t');
    // PTL(height);

    //  PT(xCoord);       // get vision result: x axes value
    //  PT('\t');
    //  PTL(yCoord);       // get vision result: y axes value

    if (objectIdx == 1) {
      int ballType = (*Mu).GetValue(object[objectIdx], kLabel);
      if (lastBallType != ballType) {
        switch ((*Mu).GetValue(object[objectIdx], kLabel)) {  // get vision result: label value
          case MU_BALL_TABLE_TENNIS:
            PTLF("table tennis");
            break;
          case MU_BALL_TENNIS:
            PTLF("tennis");
            break;
          default:
            PTLF("unknow ball type");
            break;
        }
        lastBallType = ballType;
      }
    }
    delay(i2cdelay);
    if (width > 50)
      widthCounter++;
    else
      widthCounter = 0;
    if (width < 20) {
      tQueue->addTask('k', currentX < -15 ? "wkR" : (currentX > 15 ? "wkL" : "wkF"), (50 - width) * 80);
      tQueue->addTask('k', "sit");
      tQueue->addTask('i', "");
      currentX = 0;
    } else if (widthCounter > 5) {
      tQueue->addTask('k', "bk", 1000);
      tQueue->addTask('k', "sit");
      tQueue->addTask('i', "");
      widthCounter = 0;
      currentX = 0;
    } else {
      xCoord = (int)(*Mu).GetValue(object[objectIdx], kXValue);
      yCoord = (int)(*Mu).GetValue(object[objectIdx], kYValue);
      xDiff = (xCoord - imgRangeX / 2) / f;  //max(min((xCoord - imgRangeX / 2) / 3, 32), -32);
      yDiff = (yCoord - imgRangeY / 2) / f;  //max(min((yCoord - imgRangeY / 2) / 3, 24), -24);
      currentX = max(min(currentX - xDiff, 90), -90) / p;
      currentY = max(min(currentY - yDiff, 75), -75) / p;
      if (abs(xDiff) > 1 || abs(yDiff) > 1) {
        if (abs(currentX) < 45) {
          int allParameter[DOF] = { currentX, 0, 0, 0,
                                    0, 0, 0, 0,
                                    75 - currentY / 2 + currentX / u1, 75 - currentY / 2 - currentX / u1, 90 + currentY / 3 - currentX / u2, 90 + currentY / 3 + currentX / u2,
                                    int(10 + currentY / 1.2 - currentX / d1), int(10 + currentY / 1.2 + currentX / d1), -30 - currentY / 3 + currentX / d2, -30 - currentY / 3 - currentX / d2 };
          //      transform(a, 4);
          cmdLen = DOF;
          token = T_LISTED_BIN;
          for (byte i = 0; i < cmdLen; i++)
            newCmd[i] = (int8_t)min(max(allParameter[i], -125), 125);
          newCmd[cmdLen] = '~';
          newCmdIdx = 6;
          //      printList(newCmd);}
        }
        else {
          tQueue->addTask('k', (currentX < 0 ? "vtR" : "vtL"), 2000);
          tQueue->addTask('k', "sit");
          tQueue->addTask('i', "");
          currentX = 0;
        }
      }
    }
    FPS();
  } else if (millis() - noResultTime > 2000) {  // if no object is detected for 2 seconds, switch object
    (*Mu).VisionEnd(object[objectIdx]);
    objectIdx = (objectIdx + 1) % (sizeof(object) / 2);
    (*Mu).VisionBegin(object[objectIdx]);
    PTL(objectName[objectIdx]);
    noResultTime = millis();
  }
}