
#ifndef _application_h
#define _application_h

#include <controller/Communication/Frame.h>

#define MAX_APP_NAME 20
#define DEFAULT_APPLICATION &inSync

struct Application_t {
  char* name;
  void (*setup)(void);
};

struct ChangeApp_msg
{
  uint8_t msgType;
  char name[MAX_APP_NAME];
};

extern Application_t helloWorld;
extern Application_t calibrate;
extern Application_t gyroAcc;
extern Application_t mp3Player;
extern Application_t fikt;
extern Application_t inSync;
extern Application_t helloTile;
extern Application_t jungleSpeed;

namespace Application
{
  void setup();
  void setup(void (*softwareSetup)(void));
  void loop(char* eventType, void* event);
  void handleChangeApp(Message_t* msg);
}

#endif
