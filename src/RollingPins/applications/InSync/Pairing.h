

#ifndef _Pairing_h
#define _Pairing_h

//#include <controller/Communication/Communication.h>
//#include <controller/TimerManager/TimerManager.h>
#include "RPApi.h"

struct PairingManager_t
{
  bool isMaster;
  bool isConnected;
  void (*onPair)(void);
  Timer_t* requestTimer;
};

namespace Pairing
{
  PairingManager_t* start(void (*onDone)(void));
  void broadcastRequest();
  void handleMessage(Message_t*);
  void onPaired();
};

#endif
