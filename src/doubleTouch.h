#define TOUCH_PIN_L 6
#define TOUCH_PIN_R 7
byte touchIn[2] = { TOUCH_PIN_L, TOUCH_PIN_R };
bool previousTouchState[2] = { false, false };
bool currentTouchState[2];

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
      tQueue->push_back(new Task('k', "bk", 1500));
      tQueue->push_back(new Task('k', "up"));
    } else if (currentTouchState[0]) {
      tQueue->push_back(new Task('b', "10,16,12,16,14,16"));  //example using 'b' for ASCII commands. Not recommended because of low encoding efficiency.It uses 17 byes to encode 6 numbers
      tQueue->push_back(new Task('i', "0,90"));               //example using 'i' for ASCII commands. Not recommended because of low encoding efficiency. It uses 4 byes to encode 2 numbers
                                                              //the movement starts after the music
    } else if (currentTouchState[1]) {
      char mel[] = { 17, 16, 19, 16, 21, 16, '~' };  //example using 'B' for Binary commands. it has to end up with '~' because regular 0 can be mistaken as '\0'.
      char mov[] = { 0, -90, '~' };                  //example using 'I' for Binary commands. it has to end up with '~' because regular 0 can be mistaken as '\0'.
      tQueue->push_back(new Task('I', mov));         //the movement starts before the music
      tQueue->push_back(new Task('B', mel));
    } else {
      // char mel[]={
      char mel[] = { 15, 16, 14, 16, 12, 16, '~' };
      tQueue->push_back(new Task('i', ""));
      tQueue->push_back(new Task('k', "sit"));
      tQueue->push_back(new Task('B', mel));
    }
  }
  for (byte i = 0; i < 2; i++)
    previousTouchState[i] = currentTouchState[i];
}
