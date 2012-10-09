
#ifndef Frame_h
#define Frame_h

//#include "Arduino.h"
#include "/usr/share/arduino/hardware/arduino/cores/arduino/Arduino.h"

#ifdef TILES_COMPATABLE
  #define FRAME_HEADER 1
#else
  #define FRAME_HEADER 0
#endif

struct Message_t
{
  uint8_t data_length;
  uint8_t header[0]; // Used to indicate that the header starts here
  uint8_t* data;
};

#endif
