
#ifndef EventManager_h
#define EventManager_h

//#include "Arduino.h"
#include "/usr/share/arduino/hardware/arduino/cores/arduino/Arduino.h"
#include "controller/LinkedList/LinkedList.h"

struct Event_t
{
  char* eventType;
};

struct EventSubscription_t
{
  char* eventType;
  void (*listener)(char* eventType, void* event);
};

class EventManager
{
  ListNode* events;
  ListNode* subscriptions;

  public:
    EventManager();
    void setup();
    uint8_t registerType(char* eventType);
    void unregisterType(char* eventType);
    void subscribeListener(char* eventType, void (*listener)(char* eventType, void* event));
    void unsubscribeListener(char* eventType, void (*listener)(char* eventType, void* event));
    void triggerEvent(char* eventType, void* event);
    void removeAllSubscriptions();
};

extern EventManager eventManager;

#endif
