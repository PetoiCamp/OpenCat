#define LIGHT1 A2
#define LIGHT2 A3
//#define PID
//#define advance
#ifdef PID 
double mean_array_d(double array[],int start, int end,double scar)
{
 double mean=0;
 int num=end-start;
 for(int i=start;i<end;i++)
 {
   mean=mean+array[i];
 }
 mean=mean/num;
 return  mean/scar;
}
double sum_norm2(double array1[],double array2[],int n,double scar2)
{
 int i=0;
 int result=0;
 for(i=0;i<n;i++)
 {
   result=result+(array1[i]/scar2)*(array2[i]/scar2);
 }
 return result;
}
double A2_max,A2_min,A3_max,A3_min;

void  L_F(double x[],double y[],int n,double result[2])//the least square method and save to the result[2]
{
 int i=0;
 double k=0,b=0;
 double scar=1;
 double y_mean=0;
 y_mean=mean_array_d(y,0,n,scar);
 double x_mean=0;
 x_mean=mean_array_d(x,0,n,scar);

 double x_hat[3],y_hat[3];
 if(y_mean>=150)
 {
   scar=y_mean/10;
 }
for(i=0;i<3;i++)
 {
   x_hat[i]=x[i]-x_mean;
   y_hat[i]=y[i]-y_mean;
 }
 double xy=0;
 xy=sum_norm2(y_hat,x_hat,n,scar);
 double xx=0;
 xx=sum_norm2(x_hat,x_hat,n,scar);
 k=xy/xx;
 b= y_mean-k*x_mean;
 result[0]=k;
 result[1]=b; 
}
 void calib_light(int angle,double result[2])//get the parameters of the calibration function
 {
  double data1[3];//save the data of A2
  double data2[3];//save the data of A3
  int ang=0;
  int step=1;
  int count=0;
  double A2_read=analogRead(A2);
  double A3_read=analogRead(A3);
   A2_max=A2_read,A2_min=A2_read,A3_max=A3_read,A3_min=A3_read;
  while(1)//step 
  { 
    calibratedPWM(0,ang); 
     A2_read=analogRead(A2);
     A3_read=analogRead(A3);
    if(A2_read<A2_min)
    {
      A2_min=A2_read;
    }
     if(A3_read<A3_min)
    {
      A3_min=A3_read;
    }
    if(A2_read>A2_max)
    {
      A2_max=A2_read;
    }
     if(A3_read>A3_max)
    {
      A3_max=A3_read;
    }
    delay(15);
    if(ang==angle||ang==-angle)
    { 
      step=-step;
    } 
    if(ang==0)
    {
      count++;
    }
    if(count==3)
    {
      break;
    }
    ang=ang+step;
  }
   data1[0]=A2_min;
   data2[0]=A3_min;
   data1[1]=A2_max;
   data2[1]=A3_max;
   data1[2]=0;
   data2[2]=0;
   L_F(data1,data2,3,result);
   
   /* the following code is used for debug 
   Serial.print(data1[0]);
   Serial.print('\t');
   Serial.print(data1[1]);
   Serial.print('\t');
   Serial.print(data1[2]);
   Serial.print('\n');
   Serial.print(data2[0]);
   Serial.print('\t');
   Serial.print(data2[1]);
   Serial.print('\t');
   Serial.print(data2[2]);
   //Serial.print((data2[0]+data2[1])/2);
   Serial.print('\n');
   Serial.print(result[0]);
   Serial.print('\t');
   Serial.print(result[1]);
   Serial.print('\t');
   Serial.print((data2[0]+data2[1])/2);
   Serial.print('\n');
   */
}
double light_correct(int A2_tocorrect, double result[2])
{ 
  double A2_corrected;
  A2_corrected=result[0]*A2_tocorrect+result[1];
  if(A2_corrected<=0)
  {
    A2_corrected=0;
  }
  return A2_corrected;
}


//The following variable is used for the PID
double result[2];
double error=0,lasterror=0;
double integral_error=0,differential_error=0;
double kp=0,ki=0,kd=0;
double output=0;// the output of the controller;
double A2_mean=0,A3_mean=0;
int read_count=24;
double diffA2=0,diffA3=0,lastdiffA2=0;
int pid_angle;
bool disappear_label=0;
double A2_current=0;
double A3_current=0;
void init_lightsensor()// init 
{ 
  for(int i=0;i< read_count;i++)
  {
    A3_mean=A3_mean+analogRead(A3);
    A2_mean=A2_mean+analogRead(A2);
  }
  A2_mean=A2_mean/read_count;
  A2_mean=result[0]*A2_mean+result[1];
  A3_mean=A3_mean/read_count;
}
void setpid_parm(double p,double i,double d)
{
  kp=p;
  ki=i;
  kd=d;
}
void get_error()
{
  
  diffA2=result[0]*analogRead(A2)+result[1]- A2_mean;
  diffA2=max(-150,min(150,diffA2));
  diffA3=analogRead(A3)-A3_mean;
  diffA3=max(-150,min(150,diffA3));
  error=diffA3-diffA2;
  error=110*atan(error/90); 

  lasterror=error;
}
void compute_pid()// get the output;
{

  differential_error=lasterror-error;
  
  #ifdef advance
  if(abs(error)>150)
  {
    setpid_parm(0.1,0.00,0.00);
  }
  else
  {
     setpid_parm(0.18,0.02,0.03);
  }
   if(0<error<10&&(diffA2+diffA3)<20&&(diffA2+diffA3)>5)
  {
    A2_current=result[0]*analogRead(A2)+result[1];
    A3_current=analogRead(A3);
    error*20*sqrt((A2_mean+A3_mean)/(A2_current+A3_current));
  }
  if(0<error<10&&(diffA2+diffA3)<-5)
  {
    A2_current=result[0]*analogRead(A2)+result[1];
    A3_current=analogRead(A3);
    error*=5*sqrt((A2_mean+A3_mean)/(A2_current+A3_current));
  }
  #else
   if(abs(error)>500)
  {
    integral_error=0;
  }
  #endif
  integral_error=integral_error+error;
  integral_error=max(-4500,min(4500,integral_error));
  
  output=kp*error+ki*integral_error+kd*differential_error;
  output=max(-90,min(90,output));
  lasterror=error;
  lastdiffA2=diffA2;
}
void actuator()// the actuator operates based on the value of output 
{ 
  
  pid_angle=int(output);
  calibratedPWM(0,pid_angle);
  if(token == T_SKILL){
    tQueue->addTask(T_SKILL,"sit");
    tQueue->addTask(' '," ");
  }
  
}  
void pid_reset()
{
  if(disappear_label)
  {
    
    calibratedPWM(0,0); 
    error=0;
    lasterror=0;
    integral_error=0;
    differential_error=0;
    disappear_label=0;
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
void light_disappear()
{
  if(error<5&&disappear_label==0)
  {
    
    double label1=analogRead(A2);
    double label2=analogRead(A3);
    //double label2=analogRead(A3);
    double proportion=0;
    if(A3_max>120)
    {
       proportion=0.7;
    }
    if(A3_max<=120)
    {
       proportion=0.8;
    }
    double boundary1=proportion*A2_max+(1-proportion)*A2_min;
    double boundary2=proportion*A3_max+(1-proportion)*A3_min;
    if(label1<boundary1&&label2<boundary2&&label1>A2_min&&label2>A3_min)
    {
      delay(100);
      if(label1<boundary1&&label2<boundary2) // avoid dithering
      {
      disappear_label=1;
      }
    }
  }
}
void doubleLightSetup()// when using the Photoresistors, add this function to the setup function
{
  int angle_test=55;
  Serial.begin(115200);
  calibratedPWM(0,0); 
  calib_light(angle_test,result);
  setpid_parm(0.18,0.02,0.03);
  init_lightsensor();
}
void read_doubleLight()//  when using the Photoresistors, add this function to the loop function
{
  get_error();
  compute_pid();
  actuator();
  light_disappear();
  pid_reset();
  //the following code is used for debugging
  
  /*
  Serial.print(analogRead(A2)*result[0]+result[1]);
  Serial.print('\t');
  Serial.print(analogRead(A3));
  Serial.print('\t');
  Serial.print(A2_max);
  Serial.print('\t');
  Serial.print(A3_max);
  Serial.print('\t');
  Serial.print(error);
  Serial.print('\t');
  Serial.print(output);
  Serial.print('\t');
  Serial.print('\n');
  */
  
}
#else
#define READING_COUNT 30

#define MAX_READING 1024

int meanA = 0, meanB = 0, diffA_B = 0, actualOffset = 0, last = 0;
int tolerance = 20;


void doubleLightSetup() {
  // put your setup code here, to run once:
  for (int i = 0; i < READING_COUNT; i++) {
    int a = analogRead(LIGHT1);
    int b = analogRead(LIGHT2);
    meanA += (a / READING_COUNT);
    meanB += (b / READING_COUNT);
    diffA_B += (a - b) / READING_COUNT;
    delay(1);
  }
}

void read_doubleLight() {
  int a = analogRead(LIGHT1);
  int b = analogRead(LIGHT2);
  a -= meanA;
  b -= meanB;
  int offset = b - a;
  int maxL = max(a, b);
  int clippedOffset = min(max(offset, -60), 60);

  if (maxL < -50) {
    tQueue->addTask('k', "bk", 2000);  // jigs when entering this case for the 2nd time. ???
    tQueue->addTask('k', "up");        // jigs when entering this case for the 2nd time. ???
    PTL(tQueue->size());
  } else if (maxL < 300) {
    actualOffset = (last + clippedOffset) / 2;
    actualOffset = max(min(actualOffset, 90), -90);
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
