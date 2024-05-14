/*
  This program uses a simple double light sensor to make the robot follow the light source
  The PID mode will scan the environmental lighting conditions (look around) during booting up. 
  Make sure you are not using the targeting light during the process.

  Zehui Zhao, Rongzhong Li
  Petoi LLC
  Aug, 18, 2023
*/

// Without the following macros, the program will behave based on the simple feedback reaction
// #define PID //use the standard PID to make the robot track the light source
#define ADVANCED_PID  //use some compensation to make the robot adapt to different lighting conditions

//-------
#define MAX_READING 1024.0
#define BASE_RANGE 1024.0
#define ANALOG1 A2
#define ANALOG2 A3

#ifdef ADVANCED_PID
#define PID
#endif

#define BOOTING_SCAN_RANGE 55.0  // the scan range when the robot evaluate the environmental lighting condition
#define MAX_PANNING 90.0
#define CLIP_PANNING 60.0
#define MIN_DIFFERENCE 5.0           // if the difference between two sensors is smaller than MIN_DIFFERENCE, we consider the light source is removed
#define THRESHOLD_DARK_BRIGHT 120.0  //if the reading is lower than THRESHOLD_DARK_BRIGHT, it's considered as dark condition
#define MAX_DIFF 150.0               //constrain the error in PID to avoid jumping

double rate = 1.0 * MAX_READING / BASE_RANGE;

#ifdef PID
double mean_array_d(double array[], int start, int end, double scar) {
  double mean = 0;
  int num = end - start;
  for (int i = start; i < end; i++) {
    mean = mean + array[i];
  }
  mean = mean / num;
  return mean / scar;
}
double sum_norm2(double array1[], double array2[], int n, double scar2) {
  int i = 0;
  int result = 0;
  for (i = 0; i < n; i++) {
    result = result + (array1[i] / scar2) * (array2[i] / scar2);
  }
  return result;
}
double ANALOG1_max, ANALOG1_min, ANALOG2_max, ANALOG2_min;

void L_F(double x[], double y[], int n, double result[2])  //the least square method and save to the result[2]
{
  int i = 0;
  double k = 0, b = 0;
  double scar = 1;
  double y_mean = 0;
  y_mean = mean_array_d(y, 0, n, scar);
  double x_mean = 0;
  x_mean = mean_array_d(x, 0, n, scar);

  double x_hat[3], y_hat[3];
  if (y_mean >= 150) {
    scar = y_mean / 10;
  }
  for (i = 0; i < 3; i++) {
    x_hat[i] = x[i] - x_mean;
    y_hat[i] = y[i] - y_mean;
  }
  double xy = 0;
  xy = sum_norm2(y_hat, x_hat, n, scar);
  double xx = 0;
  xx = sum_norm2(x_hat, x_hat, n, scar);
  k = xx == 0 ? 1 : xy / xx;
  b = xx == 0 ? 0 : y_mean - k * x_mean;

  if (k > 1.3 || k < 0.75) {  // assume a reasonable ratio is 4:3 or 3:4
    PTLF("The consistency of the two sensors are too bad to be trusted! Consider checking the sensors and avoid direct lighting on one of the sensors.");
    k = 1;
    b = 0;
  }
  result[0] = k;
  result[1] = b;
}
void calib_light(int angle, double result[2])  //get the parameters of the calibration function
{
  double data1[3];       //save the data of ANALOG1
  double datANALOG1[3];  //save the data of ANALOG2
  int ang = 0;
  int step = 1;
  int count = 0;
  double ANALOG1_read = analogRead(ANALOG1) / rate;
  double ANALOG2_read = analogRead(ANALOG2) / rate;
  ANALOG1_max = ANALOG1_read, ANALOG1_min = ANALOG1_read, ANALOG2_max = ANALOG2_read, ANALOG2_min = ANALOG2_read;
  while (1)  //step
  {
    calibratedPWM(0, ang);
    ANALOG1_read = analogRead(ANALOG1) / rate;
    ANALOG2_read = analogRead(ANALOG2) / rate;
    if (ANALOG1_read < ANALOG1_min) {
      ANALOG1_min = ANALOG1_read;
    }
    if (ANALOG2_read < ANALOG2_min) {
      ANALOG2_min = ANALOG2_read;
    }
    if (ANALOG1_read > ANALOG1_max) {
      ANALOG1_max = ANALOG1_read;
    }
    if (ANALOG2_read > ANALOG2_max) {
      ANALOG2_max = ANALOG2_read;
    }
    delay(15);
    if (ang == angle || ang == -angle) {
      step = -step;
    }
    if (ang == 0) {
      count++;
    }
    if (count == 3) {
      break;
    }
    ang = ang + step;
  }
  data1[0] = ANALOG1_min;
  datANALOG1[0] = ANALOG2_min;
  data1[1] = ANALOG1_max;
  datANALOG1[1] = ANALOG2_max;
  data1[2] = 0;
  datANALOG1[2] = 0;
  L_F(data1, datANALOG1, 3, result);

  /* the following code is used for debug 
   Serial.print(data1[0]);
   Serial.print('\t');
   Serial.print(data1[1]);
   Serial.print('\t');
   Serial.print(data1[2]);
   Serial.print('\n');
   Serial.print(datANALOG1[0]);
   Serial.print('\t');
   Serial.print(datANALOG1[1]);
   Serial.print('\t');
   Serial.print(datANALOG1[2]);
   //Serial.print((datANALOG1[0]+datANALOG1[1])/2);
   Serial.print('\n');
   Serial.print(result[0]);
   Serial.print('\t');
   Serial.print(result[1]);
   Serial.print('\t');
   Serial.print((datANALOG1[0]+datANALOG1[1])/2);
   Serial.print('\n');
   */
}
double light_correct(int ANALOG1_tocorrect, double result[2]) {
  double ANALOG1_corrected;
  ANALOG1_corrected = result[0] * ANALOG1_tocorrect + result[1];
  if (ANALOG1_corrected <= 0) {
    ANALOG1_corrected = 0;
  }
  return ANALOG1_corrected;
}


//The following variable is used for the PID
double result[2];
double error = 0, lasterror = 0;
double integral_error = 0, differential_error = 0;
double kp = 0, ki = 0, kd = 0;
double output = 0;  // the output of the controller;
double ANALOG1_mean = 0, ANALOG2_mean = 0;
int read_count = 24;
double diffANALOG1 = 0, diffANALOG2 = 0, lastdiffANALOG1 = 0;
int pid_angle;
bool lightRemovedQ = 0;
double ANALOG1_current = 0;
double ANALOG2_current = 0;
void init_lightsensor()  // init
{
  for (int i = 0; i < read_count; i++) {
    ANALOG2_mean = ANALOG2_mean + analogRead(ANALOG2) / rate;
    ANALOG1_mean = ANALOG1_mean + analogRead(ANALOG1) / rate;
  }
  ANALOG1_mean = ANALOG1_mean / read_count;
  ANALOG1_mean = result[0] * ANALOG1_mean + result[1];
  ANALOG2_mean = ANALOG2_mean / read_count;
}
void setpid_parm(double p, double i, double d) {
  kp = p;
  ki = i;
  kd = d;
}
void get_error() {

  diffANALOG1 = result[0] * analogRead(ANALOG1) / rate + result[1] - ANALOG1_mean;
  diffANALOG1 = max(-MAX_DIFF, min(MAX_DIFF, diffANALOG1));
  diffANALOG2 = analogRead(ANALOG2) / rate - ANALOG2_mean;
  diffANALOG2 = max(-MAX_DIFF, min(MAX_DIFF, diffANALOG2));
  error = diffANALOG2 - diffANALOG1;
  error = 110 * atan(error / 90);  // the 110 and 90 are the fitting constants to rescale the error
}
void compute_pid()  // get the output;
{

  differential_error = lasterror - error;

#ifdef ADVANCED_PID
  if (abs(error) > 150) {
    setpid_parm(0.1, 0.00, 0.00);
  } else {
    setpid_parm(0.18, 0.02, 0.03);
  }
  if (0 < error < 10 && (diffANALOG1 + diffANALOG2) < 20 && (diffANALOG1 + diffANALOG2) > 5) {
    ANALOG1_current = result[0] * analogRead(ANALOG1) / rate + result[1];
    ANALOG2_current = analogRead(ANALOG2) / rate;
    error *= 5 * sqrt((ANALOG1_mean + ANALOG2_mean) / (ANALOG1_current + ANALOG2_current));
  }
  if (0 < error < 10 && (diffANALOG1 + diffANALOG2) < -5) {
    ANALOG1_current = result[0] * analogRead(ANALOG1) / rate + result[1];
    ANALOG2_current = analogRead(ANALOG2) / rate;
    error *= 3 * sqrt((ANALOG1_mean + ANALOG2_mean) / (ANALOG1_current + ANALOG2_current));
  }
#else
  if (abs(error) > 500) {
    integral_error = 0;
  }
#endif
  integral_error = integral_error + error;
  integral_error = max(-4500.0, min(4500.0, integral_error));

  output = kp * error + ki * integral_error + kd * differential_error;
  output = max(-MAX_PANNING, min(MAX_PANNING, output));
  lasterror = error;
  lastdiffANALOG1 = diffANALOG1;
}
void actuator()  // the actuator operates based on the value of output
{
  pid_angle = int(output);
  calibratedPWM(0, pid_angle);
  if (token == T_SKILL) {
    tQueue->addTask(T_SKILL, "sit");
    tQueue->addTask(' ', " ");
  }
}
void pid_reset() {
  if (lightRemovedQ) {
    calibratedPWM(0, 0);
    error = 0;
    lasterror = 0;
    integral_error = 0;
    differential_error = 0;
    lightRemovedQ = 0;
  }
}
/*
void reset_itself()// can reset the dog's head withoud using the intensity of the ambient light
//but it destabilizes the PID system in some degree
//so currently it is not be used until it is improved and well tested;                  
{
  if(abs(error)<5)
  {
    while(1)
    { 
      double lasterror=error;
      double label1=0;
      double label2=0;
      if(pid_angle>0)
      {
        pid_angle=pid_angle-1;
        label1=1;
      }
      if(pid_angle<0)
      {
        pid_angle=pid_angle+1;
        label2=1;
      }
      calibratedPWM(0,pid_angle);
      get_error();
      delay(20);
      //calibratedPWM(0,pid_angle);
      if(abs(error)>10)
      {
        break;
      }
      if(abs(pid_angle)<10)
      {
        break;
      }
    }
  }
}
*/
void light_disappear() {
  if (error < MIN_DIFFERENCE && lightRemovedQ == 0) {

    double label1 = analogRead(ANALOG1) / rate;
    double label2 = analogRead(ANALOG2) / rate;
    double proportion = 0;
    if (ANALOG2_max > THRESHOLD_DARK_BRIGHT) {
      proportion = 0.7;
    }
    if (ANALOG2_max <= THRESHOLD_DARK_BRIGHT) {
      proportion = 0.8;
    }
    double boundary1 = proportion * ANALOG1_max + (1 - proportion) * ANALOG1_min;
    double boundary2 = proportion * ANALOG2_max + (1 - proportion) * ANALOG2_min;
    if (label1 < boundary1 && label2 < boundary2 && label1 > ANALOG1_min && label2 > ANALOG2_min) {  // it also works if you cover one on the sensors
      delay(100);
      if (label1 < boundary1 && label2 < boundary2)  // avoid shaking
      {
        lightRemovedQ = 1;  //the light source has been removed
      }
    }
  }
}
void doubleLightSetup()  // when using the Photoresistors, add this function to the setup function
{
  int angle_test = BOOTING_SCAN_RANGE;
  Serial.begin(115200);
  calibratedPWM(0, 0);
  calib_light(angle_test, result);
  setpid_parm(0.18, 0.02, 0.03);
  init_lightsensor();
}
void read_doubleLight()  //  when using the Photoresistors, add this function to the loop function
{
  get_error();
  compute_pid();
  actuator();
  light_disappear();
  pid_reset();
  //the following code is used for debugging

  /*
  Serial.print(analogRead(ANALOG1)/rate*result[0]+result[1]);
  Serial.print('\t');
  Serial.print(analogRead(ANALOG2)/rate);
  Serial.print('\t');
  Serial.print(ANALOG1_max);
  Serial.print('\t');
  Serial.print(ANALOG2_max);
  Serial.print('\t');
  Serial.print(error);
  Serial.print('\t');
  Serial.print(output);
  Serial.print('\t');
  Serial.print('\n');
  */
}

#else  // simple feedback reaction

#define READING_COUNT 30

int meanA = 0, meanB = 0, diffA_B = 0, actualOffset = 0, last = 0;
int tolerance = 20;

void doubleLightSetup() {
  // put your setup code here, to run once:
  for (int i = 0; i < READING_COUNT; i++) {
    int a = analogRead(ANALOG1);
    int b = analogRead(ANALOG2);
    meanA += (a / READING_COUNT);
    meanB += (b / READING_COUNT);
    diffA_B += (a - b) / READING_COUNT;
    delay(1);
  }
}

void read_doubleLight() {
  int a = analogRead(ANALOG1);
  int b = analogRead(ANALOG2);
  a -= meanA;
  b -= meanB;
  int offset = b - a;
  int maxL = max(a, b);
  int clippedOffset = min(max(offset, -CLIP_PANNING), CLIP_PANNING);

  if (maxL < -5) {
    tQueue->addTask('k', "bk", 2000);  // jigs when entering this case for the 2nd time. ???
    tQueue->addTask('k', "up");        // jigs when entering this case for the 2nd time. ???
    PTL(tQueue->size());
  } else if (maxL < 300) {
    actualOffset = (last + clippedOffset) / 2;
    actualOffset = max(min(actualOffset, MAX_PANNING), -MAX_PANNING);
    token = T_INDEXED_SIMULTANEOUS_BIN;
    newCmd[0] = 0;
    newCmd[1] = actualOffset;
    last = actualOffset;
    cmdLen = 2;
    newCmdIdx = 5;
    PTL(actualOffset);
  }
}
#endif
