#define PIR_PIN 8
bool previousPIR = false;

void createPirTask() {  //this is an example task
  tQueue->addTask('k', "sit");
  tQueue->addTask('m', "0 -60 0 60", 2000);
  char music[] = { 14, 8, 18, 16, 16, 16, 13, 16, 9, 16, 11, 16, 0, 4, 13, 8, 9, 4, '~' };
  tQueue->addTask('B', music);
  tQueue->addTask('k', "str", 2000);
  tQueue->addTask('k', "up");
}

void read_PIR() {
  bool currentPIR = digitalRead(PIR_PIN);
  PTL(currentPIR);
  if (currentPIR && !previousPIR) {
    createPirTask();
    previousPIR = 1;
  } else if (!currentPIR && previousPIR) {
    newCmdIdx = 5;
    token = T_REST;
    previousPIR = 0;
  }
}
