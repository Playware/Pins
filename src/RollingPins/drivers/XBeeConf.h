

#ifndef XBeeConf_h
#define XBeeConf_h

//#include "Arduino.h"
#include "/usr/share/arduino/hardware/arduino/cores/arduino/Arduino.h"

namespace XBeeConf
{
  char read(uint16_t timeout);
  char read();
  String readLine();
  bool isOk();
  void send(String command);
  bool setParam(String command, String value);
  String getParam(String command);
  void setDefaults();

};

#endif
