#include "QList/QList.h"
long taskTimer = 0;
long taskInterval = -1;
class Task {
public:
  char tkn;
  char* parameters;
  int paraLength;
  int dly;
  template<typename T> Task(char t, T* p, int d = 0)
    : tkn{ t }, dly{ d } {
    paraLength = (tkn < 'a') ? strlenUntil(p, '~') : strlen(p);
    // if (paraLength) {
    parameters = new char[paraLength + 1];
    arrayNCPY(parameters, p, paraLength);
    parameters[paraLength] = token < 'a' ? '~' : '\0';
    // }
  };
  ~Task() {
    // if (paraLength)
    delete[] parameters;
  };
  void info() {
    printCmdByType(tkn, parameters, paraLength);
  }
};

class TaskQueue : public QList<Task* > {
public:
  TaskQueue() {
    PTLF("TaskQ");
  };
  // void createTask() {  //this is an example task
  //   this->push_back(new Task('k', "vtF", 2000));
  //   this->push_back(new Task('k', "up"));
  // }
  void popTask() {
    if (taskInterval == -1 || millis() - taskTimer > taskInterval) {
      Task* t = this->front();
      // t->info();
      token = t->tkn;
      lowerToken = tolower(token);
      cmdLen = t->paraLength;
      taskInterval = t->dly;
      // if (cmdLen) {
      arrayNCPY(newCmd, t->parameters, cmdLen);
      newCmd[cmdLen] = token < 'a' ? '~' : '\0';
      // }
      taskTimer = millis();
      newCmdIdx = 5;
      delete t;
      this->pop_front();
    }
  }
};
TaskQueue* tQueue;
