/*
 * Vibrator
 *
 * Functions related to built in vibrator
 *
 * Author: Arnþór Magnússon
 * Create: 22. Jun 2011
 * Updated: 22. Jun 2011
 */

#ifndef Vibrator_h
#define Vibrator_h

//#include "Arduino.h"
#include "/usr/share/arduino/hardware/arduino/cores/arduino/Arduino.h"
class Vibrator
{

  static const uint8_t PIN = 6;
  public:
    void setup();
    static void on();
    static void off();
    void toggle();
};

extern Vibrator vibrator;

#endif
