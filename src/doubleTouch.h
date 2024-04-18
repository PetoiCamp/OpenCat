byte touchIn[2] = { DIGITAL6, DIGITAL7 };
bool previousTouchState[2] = { false, false };
bool currentTouchState[2];

void touchSetup() {
  pinMode(touchIn[0], INPUT);
  pinMode(touchIn[1], INPUT);
}
void read_doubleTouch() {
  for (byte i = 0; i < 2; i++)
    currentTouchState[i] = digitalRead(touchIn[i]);
  if (currentTouchState[0] || currentTouchState[1]) {
    PT(previousTouchState[0]);
    PT(currentTouchState[0]);
    PT('\t');
    PT(previousTouchState[1]);
    PTL(currentTouchState[1]);
  }
  if (currentTouchState[0] != previousTouchState[0] || currentTouchState[1] != previousTouchState[1]) {
    delay(100);  // read again for concurrent touches
    for (byte i = 0; i < 2; i++)
      currentTouchState[i] = digitalRead(touchIn[i]);
    if (currentTouchState[0] && currentTouchState[1]) {
      beep(20, 50, 50, 3);
      tQueue->addTask('k', "bk", 1500);
      tQueue->addTask('k', "up");
    } else if (currentTouchState[0]) {
      tQueue->addTask('i', "0,90");               //example using 'i' for ASCII commands. Not recommended because of low encoding efficiency. It uses 4 byes to encode 2 numbers
      tQueue->addTask('b', "10,16,12,16,14,16");  //example using 'b' for ASCII commands. Not recommended because of low encoding efficiency.It uses 17 byes to encode 6 numbers
    } else if (currentTouchState[1]) {
      char mel[] = { 17, 16, 19, 16, 21, 16, '~' };  //example using 'B' for Binary commands. it has to end up with '~' because regular 0 can be mistaken as '\0'.
      char mov[] = { 0, -90, '~' };                  //example using 'I' for Binary commands. it has to end up with '~' because regular 0 can be mistaken as '\0'.
      tQueue->addTask('I', mov);                     //the movement starts before the music
      tQueue->addTask('B', mel);
    } else {
      // char mel[]={
      char mel[] = { 15, 16, 14, 16, 12, 16, '~' };
      tQueue->addTask('i', "");
      tQueue->addTask('k', "sit");
      tQueue->addTask('B', mel);
    }
  }
  for (byte i = 0; i < 2; i++)
    previousTouchState[i] = currentTouchState[i];
}
