
#include "RPApi.h"

namespace GyroAcc
{
  static void setup();
  static void loop();
};

Application_t gyroAcc = {
  "GyroAcc",
  GyroAcc::setup
};

char MSG_POINT = 0x00;
char MSG_COMMAND = 0x01;
enum CommandType {COMMAND_LOG, COMMAND_STOP, COMMAND_CALIBRATE};

struct Point_msg
{
  uint8_t msgType;
  uint8_t name;
  float value;
  uint16_t index;
} point;

struct Command_msg
{
  uint8_t msgType;
  uint8_t command;
  uint8_t value;
};

static Timer_t* timer = NULL;

void debugSend(const uint8_t name, float& value)
{
  point.name = name;
  point.value = value;
  comm.send((uint8_t*)&point, sizeof(struct Point_msg));
}

static void logTimer()
{
  movement.update();
  movement.mdebug(debugSend, point.index);
}

static void stop()
{
  point.index = 0;
  if (timer != NULL)
  {
    timerManager.remove(&timer);
    RP_idleStart();
  }
}

static void handleCommand(Message_t* msg)
{
  Command_msg* c = (Command_msg*)msg->data;
  light.setAll(LIGHT_BLUE);
  light.update();
  delay(100);
  uint8_t sampleTime = 500;
  switch(c->command)
  {
    case COMMAND_LOG:
      stop();
      RP_idleStop();
      light.setAll(LIGHT_GREEN);
      light.update();
      if (c->value != 0)
        sampleTime = c->value;
      timer = timerManager.createPeriodic(0, sampleTime, logTimer);
      break;
    case COMMAND_STOP:
      stop();
      break;
    case COMMAND_CALIBRATE:
      light.setAll(LIGHT_RED);
      light.update();
      RP_calibrate();
      light.clearAll();
      light.update();
      break;
  }
}

static void GyroAcc::setup()
{
  stop();
  RP_idleStart();
  point.index = 0;
  point.msgType = MSG_POINT;
  comm.subscribe(MSG_COMMAND, handleCommand);
}

