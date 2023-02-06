#define TOUCH_PIN_L 6
#define TOUCH_PIN_R 7
byte touchIn[2] = { TOUCH_PIN_L, TOUCH_PIN_R };
bool previousTouchState[2] = { false, false };
bool currentTouchState[2];

void read_doubleTouch() {
  for (byte i = 0; i < 2; i++)
    currentTouchState[i] = digitalRead(touchIn[i]);
  // PT(previousTouchState[0]);
  // PT(currentTouchState[0]);
  // PT('\t');
  // PT(previousTouchState[1]);
  // PTL(currentTouchState[1]);
  if (currentTouchState[0] != previousTouchState[0] || currentTouchState[1] != previousTouchState[1]) {
    delay(100); // read again for concurrent touches 
    for (byte i = 0; i < 2; i++)
      currentTouchState[i] = digitalRead(touchIn[i]);
    if (currentTouchState[0] && currentTouchState[1]) {
      beep(20,50,50,3);
      tQueue->push_back(new Task('k', "bk", 1000));
      tQueue->push_back(new Task('k', "balance"));
    } else if (currentTouchState[0]) {
      tQueue->push_back(new Task('i', "0,90"));
      tQueue->push_back(new Task('b', "10,16,12,16,14,16"));
    } else if (currentTouchState[1]) {
      tQueue->push_back(new Task('i', "0,-90"));
      tQueue->push_back(new Task('b', "17,16,19,16,21,16"));
    } else {
      tQueue->push_back(new Task('k', "sit"));
      tQueue->push_back(new Task('b', "15,16,14,16,12,16"));
    }
  }
  for (byte i = 0; i < 2; i++)
    previousTouchState[i] = currentTouchState[i];
}