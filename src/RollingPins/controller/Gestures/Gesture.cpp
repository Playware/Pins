#include "Arduino.h"
#include "Gesture.h"
#include "drivers/Movement.h"

void doIt()
{
  int a = 1;
}

// Subcribed events
void (*gestureStart_)() = NULL;
void (*gestureEnd_)() = NULL;

Gesture::Gesture()
{
  isDetectingGesture = false;
};

void Gesture::subscribeForEvents(void (*gestureStart)(void), void (*gestureEnd)(void))
{
  gestureStart_ = gestureStart;
  gestureEnd_ = gestureEnd;
}

void Gesture::update()
{
  if (isDetectingGesture && *movement.R() < THRESHOLD_ACC)
  {
    isDetectingGesture = false;
    if (gestureEnd_ != NULL)
    {
      gestureEnd_();
    }
  }
  else if (!isDetectingGesture && *movement.R() > THRESHOLD_ACC)
  {
    isDetectingGesture = true;
    if (gestureStart_ != NULL)
    {
      gestureStart_();
    }
  }
};


Gesture gesture;
