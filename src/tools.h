#define PT(s) Serial.print(s)                      // abbreviate print commands
#define PT_FMT(s, format) Serial.print(s, format)  // abbreviate print commands
#define PTL(s) Serial.println(s)
#define PTF(s) Serial.print(F(s))  // trade flash memory for dynamic memory with F() function
#define PTLF(s) Serial.println(F(s))
#define PTH(head, value) \
  { \
    PT(head); \
    PT('\t'); \
    PTL(value); \
  }

#ifdef DEVELOPER
#include "MemoryFree/MemoryFree.h"  //http://playground.arduino.cc/Code/AvailableMemory
#endif
#include <EEPROM.h>

//short tools
template<typename T> int8_t sign(T val) {
  return (T(0) < val) - (val < T(0));
}

int strlenUntil(const char *s, char terminator) {
  int l = 0;
  while (s[l++] != terminator)
    ;
  return l - 1;
}
void printRange(int r0 = 0, int r1 = 0) {
  if (r1 == 0)
    for (byte i = 0; i < r0; i++) {
      PT(i);
      PT('\t');
    }
  else
    for (byte i = r0; i < r1; i++) {
      PT(i);
      PT('\t');
    }
  PTL();
}
template<typename T> void printList(T *arr, byte len = DOF) {
  String temp = "";
  for (byte i = 0; i < len; i++) {
    temp += String(int(arr[i]));
    temp += ",\t";
    //PT((T)(arr[i]));
    //PT('\t');
  }
  PTL(temp);
}
template<typename T> void printListWithoutString(T *arr, byte len = DOF) {
  for (byte i = 0; i < len; i++) {
    PT((T)(arr[i]));
    PT('\t');
  }
  PTL();
}
template<typename T> void printTable(T *list) {
  printRange(0, DOF);
  printList(list, DOF);
}

// template<typename T, typename T1> void arrayNCPY(T *destination, const T1 *source, int len) {  //deep copy regardless of '\0'
//   for (int i = 0; i < len; i++)
//     destination[i] = min((T1)125, max((T1)-125, source[i]));
// }
template<typename T, typename T1> void arrayNCPY(T *destination, const T1 *source, int len) {  //deep copy regardless of '\0'
  for (int i = 0; i < len; i++)
    destination[i] = source[i];
}