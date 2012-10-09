/*
 * Rolling pins
 * 
 * Author: Arnthor Magnusson <arnthorm@gmail.com>
 * Created: 1. Jun 2011
 * Updated: 28. Jun 2011
*/

#include <Arduino.h>
//#include "/usr/share/arduino/hardware/arduino/cores/arduino/Arduino.h"
#include <drivers/XBeeAPIConf.h>
#include "RPApi.h"

// Needed by linker
extern "C" void __cxa_pure_virtual(void) {
  while(1);
} 

int test(void)
{
  return 0;
}

int main(void)
{
  init(); // Arduino init
  Serial.begin(57600);
  vibrator.setup();
  light.clearAll();
  light.update();
  sound.setup();
  comm.setup();
  movement.setup();
  RP_loadCalibration();
#ifdef TILES_COMPATABLE
  RP_makeTilesCompatable();
#endif 

  //RP_configXBee();
  Worker::setup(NULL);
  Application::setup(RP_setup);
  
  //RP_test();
  //RP_test2();
  movement.update();
  if (*movement.Ry() > *movement.Rx() && 
      *movement.Ry() > *movement.Rz() &&
      0.8 < *movement.Ry() && *movement.Ry() < 1.2)
  {
    RP_test2();
  }

  while(true)
  {
    Worker::act();
  }

  return 0;
}

