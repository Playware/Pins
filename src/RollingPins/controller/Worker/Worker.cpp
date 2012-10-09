
#include "Arduino.h"
#include "Worker.h"
#include "controller/EventManager/EventManager.h"

extern char EVENT_ACT[] = "CA";
extern char EVENT_SETUP_DONE[] = "CB";

void Worker::setup(void (*ctrl_setup)())
{
  eventManager.registerType(EVENT_ACT);
  eventManager.registerType(EVENT_SETUP_DONE);
  if (ctrl_setup != NULL)
  {
    ctrl_setup();
  }
  eventManager.triggerEvent(EVENT_SETUP_DONE, NULL);
}

void Worker::act()
{
  eventManager.triggerEvent(EVENT_ACT, NULL);
}

