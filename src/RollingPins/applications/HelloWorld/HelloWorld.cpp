#include "RPApi.h"

namespace HelloWorld
{
  static void setup(void);
}

Application_t helloWorld = {
  "HelloWorld",
  HelloWorld::setup
};

static bool isOn = false;

static void onTimerFired()
{
  if (isOn)
    light.clearAll();
  else
    light.setAll(LIGHT_WHITE);
  light.update();
  isOn = !isOn;
}

static void HelloWorld::setup(void)
{
  //timerManager.createPeriodic(0, 1000, onTimerFired);
  light.setAll(LIGHT_RED);
  light.update();
}
