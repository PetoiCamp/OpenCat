#include <Wire.h>
int xCoord, yCoord, width, widthCounter;  //the x y returned by the sensor
int xDiff, yDiff;                         //the scaled distance from the center of the frame
int currentX = 0, currentY = 0;           //the current x y of the camera's direction in the world coordinate
int imgRangeX = 100;                      //the frame size 0~100 on X and Y direction
int imgRangeY = 100;

int8_t lensFactor = 30;
int8_t proportion = 11;
int8_t pan = 10;
int8_t sp = 4;

int8_t frontUpX = 60;
int8_t backUpX = 80;
int8_t frontDownX = 20;
int8_t backDownX = 80;
int8_t frontUpY = 20;
int8_t backUpY = 30;
int8_t frontDownY = 12;
int8_t backDownY = 30;

int8_t frontUp = 60;
int8_t backUp = 90;
int8_t frontDown = 10;
int8_t backDown = -30;

int8_t **par = new int8_t *[16];

#define MU_CAMERA
// #define SENTRY1_CAMERA
// #define TALL_TARGET

#ifdef MU_CAMERA
void muCameraSetup();
void read_MuCamera();
#endif

#ifdef SENTRY1_CAMERA
void sentry1CameraSetup();
void read_Sentry1Camera();
#endif

void cameraSetup() {
  par[0] = &lensFactor;
  par[1] = &proportion;
  par[2] = &pan;
  par[3] = &sp;
  par[4] = &frontUpX;
  par[5] = &backUpX;
  par[6] = &frontDownX;
  par[7] = &backDownX;
  par[8] = &frontUpY;
  par[9] = &backUpY;
  par[10] = &frontDownY;
  par[11] = &backDownY;
  par[12] = &frontUp;
  par[13] = &backUp;
  par[14] = &frontDown;
  par[15] = &backDown;
  transformSpeed = 0;
  widthCounter = 0;
#ifdef MU_CAMERA
  muCameraSetup();
#endif
#ifdef SENTRY1_CAMERA
  lensFactor = 10;
  proportion = 20;
  sentry1CameraSetup();
#endif
  fps = 0;
  loopTimer = millis();
  tQueue->addTask('k', "sit");
}
void showRecognitionResult(int xCoord, int yCoord, int width, int height = -1) {
  PT(xCoord);  // get vision result: x axes value
  PT('\t');
  PT(yCoord);  // get vision result: y axes value
  PT('\t');
  PT("size = ");
  PT(width);
  if (height >= 0) {
    PT('\t');
    PT(height);
  }
  PTL();
}

// #define WALK  //let the robot move its body to follow people rather than sitting at the original position \
              // it works the best on the table so the robot doesn't need to loop upward.
// #define ROTATE
void cameraBehavior(int xCoord, int yCoord, int width) {
  // showRecognitionResult(xCoord, yCoord, width);
#ifdef WALK
  if (width > 45 && width != 52)  //52 maybe a noise signal
    widthCounter++;
  else
    widthCounter = 0;
  if (width < 25 && width != 16) {                                                                      //16 maybe a noise signal
    tQueue->addTask('k', currentX < -15 ? "wkR" : (currentX > 15 ? "wkL" : "wkF"), (50 - width) * 50);  //walk towards you
    tQueue->addTask('k', "sit");
    tQueue->addTask('i', "");
    currentX = 0;
  } else if (widthCounter > 2) {
    tQueue->addTask('k', "bk", 1000);  //the robot will walk backward if you get too close!
    tQueue->addTask('k', "sit");
    tQueue->addTask('i', "");
    widthCounter = 0;
    currentX = 0;
  } else
#endif
  {
    xDiff = (xCoord - imgRangeX / 2.0);  //atan((xCoord - imgRangeX / 2.0) / (imgRangeX / 2.0)) * degPerRad;//almost the same
    yDiff = (yCoord - imgRangeY / 2.0);  //atan((yCoord - imgRangeY / 2.0) / (imgRangeX / 2.0)) * degPerRad;
    if (abs(xDiff) > 1 || abs(yDiff) > 1) {
      xDiff = xDiff / (lensFactor / 10.0);
      yDiff = yDiff / (lensFactor / 10.0);
      currentX = max(min(currentX - xDiff, 125), -125) / (proportion / 10.0);
      currentY = max(min(currentY - yDiff, 125), -125) / (proportion / 10.0);

      // PT('\t');
      // PT(currentX);
      // PT('\t');
      // PTL(currentY);

      // if (abs(currentX) < 60) {
      int8_t base[] = { 0, 0, 0, 0,
                        0, 0, 0, 0,
                        frontUp, frontUp, backUp, backUp,
                        -frontDown, -frontDown, backDown, backDown };
      int8_t feedBackArray[][2] = {
        { pan, 0 },
        { 0, 0 },
        { 0, 0 },
        { 0, 0 },
        { 0, 0 },
        { 0, 0 },
        { 0, 0 },
        { 0, 0 },
        { frontUpX, -frontUpY },
        { -frontUpX, -frontUpY },
        { -backUpX, backUpY },
        { backUpX, backUpY },
        { -frontDownX, frontDownY },
        { frontDownX, frontDownY },
        { backDownX, -backDownY },
        { -backDownX, -backDownY },
      };
      transformSpeed = sp;
      for (int j = 0; j < DOF; j++) {
        int i = j;
        float adj = float(base[i])
                    + (feedBackArray[i][0] ? currentX * 10.0 / feedBackArray[i][0] : 0)
                    + (feedBackArray[i][1] ? currentY * 10.0 / feedBackArray[i][1] : 0);
        newCmd[j] = min(125, max(-125, adj));

        // PT(adj);
        // PT('\t');
        // PT(int8_t(newCmd[j]));
        // PTF(",\t");
      }
      PTL();
      cmdLen = DOF;
      token = T_LISTED_BIN;

      newCmd[cmdLen] = '~';
      newCmdIdx = 6;
      //      printList(newCmd);}
      // }
#ifdef ROTATE
      else {
        tQueue->addTask('k', (currentX < 0 ? "vtR" : "vtL"), abs(currentX) * 40);  //spin its body to follow you
        tQueue->addTask('k', "sit");
        tQueue->addTask('i', "");
        currentX = 0;
      }
#endif
    }
  }
}

void read_camera() {
#ifdef MU_CAMERA
  read_MuCamera();
#endif
#ifdef SENTRY1_CAMERA
  read_Sentry1Camera();
#endif
}

#ifdef MU_CAMERA
// You need to install https://github.com/mu-opensource/MuVisionSensor3 as a zip library in Arduino IDE.
// Set the four dial switches on the camera as **v ^ v v** (the second switch dialed up to I2C) and connect the camera module to the I2C grove on NyBoard.
// The battery should be turned on to drive the servos.

// You can use these 3D printed structures to attach the camera module.
// https://github.com/PetoiCamp/NonCodeFiles/blob/master/stl/MuIntelligentCamera_mount.stl
// https://github.com/PetoiCamp/NonCodeFiles/blob/master/stl/bone.stl
// After uploading the code, you may need to press the reset buttons on the module and then the NyBoard.
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
#define TX_PIN 6
#define RX_PIN 7
SoftwareSerial mySerial(RX_PIN, TX_PIN);
#endif

MuVisionSensor *Mu;
int skip = 1;  //, counter; //an efforts to reduce motion frequency without using delay. set skip >1 to take effect
int i2cdelay = 3;
long noResultTime = 0;
MuVisionType object[] = { VISION_BODY_DETECT, VISION_BALL_DETECT };
String objectName[] = { "body", "ball" };
int objectIdx = 0;
int lastBallType;
bool firstConnection = true;

void muCameraSetup() {
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

  (*Mu).VisionBegin(object[objectIdx]);
  noResultTime = millis();
}

void read_MuCamera() {
  if ((*Mu).GetValue(object[objectIdx], kStatus)) {  // update vision result and get status, 0: undetected, other:
    //      PTL(objectName[objectIdx]);
    noResultTime = millis();  //update the timer
    xCoord = (int)(*Mu).GetValue(object[objectIdx], kXValue);
    yCoord = (int)(*Mu).GetValue(object[objectIdx], kYValue);
    width = (int)(*Mu).GetValue(object[objectIdx], kWidthValue);
    // height = (int)(*Mu).GetValue(VISION_BODY_DETECT, kHeightValue);
    //-------ball------
    // if (objectIdx == 1) {
    //   int ballType = (*Mu).GetValue(object[objectIdx], kLabel);
    //   if (lastBallType != ballType) {
    //     switch ((*Mu).GetValue(object[objectIdx], kLabel)) {  // get vision result: label value
    //       case MU_BALL_TABLE_TENNIS:
    //         PTLF("table tennis");
    //         break;
    //       case MU_BALL_TENNIS:
    //         PTLF("tennis");
    //         break;
    //       default:
    //         PTLF("unknow ball type");
    //         break;
    //     }
    //     lastBallType = ballType;
    //   }
    // }
    //-------ball------
    cameraBehavior(xCoord, yCoord, width);
    // FPS();
  }
  //  else if (millis() - noResultTime > 2000) {  // if no object is detected for 2 seconds, switch object
  //   (*Mu).VisionEnd(object[objectIdx]);
  //   objectIdx = (objectIdx + 1) % (sizeof(object) / 2);
  //   (*Mu).VisionBegin(object[objectIdx]);
  //   PTL(objectName[objectIdx]);
  //   noResultTime = millis();
  // }
}
#endif

#ifdef SENTRY1_CAMERA

#define SENTRY_ADDR 0x60  // 0x61 0x62 0x63

char writeRegData(char reg_addr, char reg_data) {
  Wire.beginTransmission(SENTRY_ADDR);
  Wire.write(reg_addr);
  Wire.write(reg_data);
  Wire.endTransmission();
  delay(1);
}

char readRegData(char reg_addr) {
  Wire.beginTransmission(SENTRY_ADDR);
  Wire.write(reg_addr);  // read label
  Wire.endTransmission();

  Wire.requestFrom(SENTRY_ADDR, 1);  // request 1 byte from slave device
  char ret = 0;
  while (Wire.available()) {
    ret = Wire.read();  // receive a byte
  }
  return ret;
  delay(1);
}

void sentry1CameraSetup() {
  // Wire.begin();  // join i2c bus (address optional for master)
  delay(2000);  // wait for sentry1 startup, not necessary
  PTLF("Setup sentry1");
  writeRegData(0x20, 0x07);  // set vision id: 7 (body for Sentry1)
  writeRegData(0x21, 0x01);  // enable vision
  writeRegData(0x22, 0x10);  // set vision level: 0x10=Sensitive/Speed 0x20=balance(default if not set) 0x30=accurate
  delay(1000);
  PTLF("Sentry1 ready");
  lensFactor = 30;
  proportion = 20;
  // pan = 15;
  sp = 1;
}

void read_Sentry1Camera() {
  //  Serial.println("loop...");
  char label = readRegData(0x89);  // read label/ value Low Byte, 1=detected, 0=undetected
  //  Serial.print("label: ");
  //  Serial.println(label);//
  if (label == 1) {              // have detected
    xCoord = readRegData(0x81);  // read x value Low Byte, 0~100, (LB is enought for sentry 1)
    yCoord = readRegData(0x83);  // read y value Low Byte, 0~100
    width = readRegData(0x85);   // read width value Low Byte, 0~100
                                 //  char height = readRegData(0x87);  // read height value Low Byte, 0~100, not necessary if already have read width
                                 // do something ......
    cameraBehavior(xCoord, yCoord, width);
    FPS();
    delay(10);
  }
  // do something or delay some time
}
#endif
