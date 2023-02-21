#include "QList/QList.h"
long taskTimer = 0;
long taskInterval = -1;
class Task {
public:
  char tkn;
  const char* parameters;
  int dly;
  Task(char t, const char* p, int d = 0)
    : tkn{ t }, parameters{ p }, dly{ d } {};
  ~Task(){};
};

class TaskQueue : public QList<Task* > {
public:
  TaskQueue(){};
  TaskQueue(const Task* t, int len) {
    PT("Build task queue...");
  }
  void createTask() {  //this is an example task
    this->push_back(new Task('k', "wkF", 2000));
    this->push_back(new Task('k', "vtL", 2000));
    this->push_back(new Task('k', "pee"));
    this->push_back(new Task('m', "0 -30 0 30 0 0"));
  }
  void popTask() {
    if (taskInterval == -1 || millis() - taskTimer > taskInterval) {
      Task* t = this->front();
      // PT("task: ");
      // PT(t->tkn);
      // PT('\t');
      // PT(t->parameters);
      // PT('\t');
      // PTL(t->dly);
      PTL(this->size());
      this->pop_front();
      PTL(this->size());
      token = t->tkn;
      lowerToken = tolower(token);
      bufferPtr = (token == T_SKILL || lowerToken == T_INDEXED_SIMULTANEOUS_ASC || lowerToken == T_INDEXED_SEQUENTIAL_ASC) ? (int8_t*)newCmd : dataBuffer;
      cmdLen = strlen(t->parameters);
      taskInterval = t->dly;
      arrayNCPY(bufferPtr, t->parameters, cmdLen);
      bufferPtr[cmdLen] = '\0';
      // PT(cmdLen);
      // PTL(bufferPtr);
      taskTimer = millis();
      newCmdIdx = 5;
      delete t;
    }
  }
};
TaskQueue* tQueue;