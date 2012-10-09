#ifndef TimerManager_h
#define TimerManager_h

#include "controller/LinkedList/LinkedList.h"

void TIMER_act();

struct Timer_t
{
  void (*callback)();
  bool isPeriodic;
  int shotsLeft;
  unsigned long nextShot;
  unsigned long interval;
};

void TIM_act(char* eventType, void* event);

class TimerManager
{
  ListNode* timers;
  public:
    Timer_t* createPeriodic(unsigned long start, unsigned long interval, void (*callback)());
    Timer_t* createOneShot(unsigned long start, void (*callback)());
    Timer_t* createNShot(unsigned long start, unsigned long interval, int shots, void (*callback)());

    void act();
    void remove(Timer_t**);
    void removeAll();
};

extern TimerManager timerManager;

#endif
