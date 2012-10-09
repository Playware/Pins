
#include "Application.h"
#include "controller/Communication/Communication.h"
#include "RPApi.h"

Application_t* activeApp = DEFAULT_APPLICATION;

static char MSG_CHANGEAPP = 50;

Application_t *apps[] = {
  &calibrate,
  &helloWorld,
  &gyroAcc,
  &mp3Player,
  &inSync,
  &helloTile,
  &jungleSpeed
};

void (*sSetup)(void);

void Application::setup(void (*softwareSetup)(void))
{
  light.setAll(LIGHT_SOFTBLUE);
  light.update();
  delay(500);

  sSetup = softwareSetup;
  softwareSetup();
  comm.subscribe(MSG_CHANGEAPP, Application::handleChangeApp);

  light.clearAll();
  light.update();
  vibrator.off();
  sound.stop();
  activeApp->setup();
}

void Application::handleChangeApp(Message_t* msg)
{
  int i = 0;
  ChangeApp_msg* data = (ChangeApp_msg*)msg->data;
  char* receivedName = (char*) &data->name;
  while(apps[i]->name[0] != NULL)
  {
    if(strcmp(apps[i]->name, receivedName) == 0)
    {
      activeApp = apps[i];
      comm.removeAllSubscriptions();
      eventManager.removeAllSubscriptions();
      timerManager.removeAll();

      Application::setup(sSetup);
      break;
    }
    i++;
  }
}
