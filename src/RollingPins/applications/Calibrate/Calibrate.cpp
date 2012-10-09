

#include "RPApi.h"

namespace Calibrate
{
  static void setup(void);
  static void loop(void);
}

Application_t calibrate = {
  "Calibrate",
  Calibrate::setup 
};

static void Calibrate::setup(void)
{
  light.setAll(LIGHT_YELLOW);
  light.update();
  delay(1000);
  light.setAll(LIGHT_RED);
  light.update();
  RP_calibrate();
  light.setAll(LIGHT_GREEN);
  light.update();
}
