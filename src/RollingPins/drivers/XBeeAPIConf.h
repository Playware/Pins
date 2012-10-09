
#ifndef XBeeAPIConf_h
#define XBeeAPIConf_h

//#include "Arduino.h"
#include "/usr/share/arduino/hardware/arduino/cores/arduino/Arduino.h"
#include "libraries/XBee/XBee.h"

namespace XBeeAPIConf
{
  static AtCommandRequest atRequest;
  static AtCommandResponse atResponse;
  static XBee xbee;

  void setup();
  bool setParam(char* cmd, char* value, uint8_t length);
  void send();
  void setDefaults();
};

#endif
