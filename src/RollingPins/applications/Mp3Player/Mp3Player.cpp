

#include "RPApi.h"

#define sign1(x) ((x > 0) ? 1 : ((x < 0) ? -1 : 1))

namespace Mp3Player
{
  static void setup();
  static void loop();
  static void handleMessage(Message_t* msg);
}

Application_t mp3Player = {
  "Mp3Player",
  Mp3Player::setup
};

static char MSG_COMMAND = 0;
enum CommandType {LIGHT, VOLUMELIGHT, VOLUME, NEXT, PLAY, STOP, PLAY_SONG, IS_PLAYING, SET_AS_PLAYER, CALIBRATE};
enum State {NONE, SHAKER, ROLLER};

static bool shaken = false;
static bool isOn = false;
static unsigned long timer;
static unsigned long timerRoller;
static unsigned long timerNext;
static uint8_t currState;
static uint8_t lastState;
static float lastValueX;
static int lightLevel = (int)4000*3/7.0;
static int lightR = 0;
static int lightG = 0;
static int lightB = 0;
static int lastLightLevel = 1;
static bool forward = true;
static uint8_t vol = 3;
static bool isPlaying = false;
static bool isPlayer = false;

struct Command
{
  uint8_t id;
  uint8_t command;
  uint8_t value;

} command;

bool stateChecker(uint8_t state)
{
  return (currState == NONE && (millis() - timer > 1000 || lastState == state)) || currState == state;
}

void endState()
{
  lastState = currState;
  currState = NONE;
  timer = millis();
}

void sendCommand(uint8_t cmd, uint16_t value)
{
  command.command = cmd;
  command.value = value;
  comm.send((uint8_t*)&command, sizeof(struct Command));
}

void stop()
{
  sound.stop();
  light.clearAll();
  light.update();
}

static void onLoop()
{
  if (isPlayer)
  {

  }
  else
  {
    movement.update();

    if (stateChecker(SHAKER) && *movement.R() > 1.2)
    {
      currState = SHAKER;
      int level = (int)(lightLevel - (*movement.R()-1.2)/3*lightLevel);
      if (level < 0)
        level = 0;
      sendCommand(LIGHT, level);
      if (*movement.R() > 3)
      {
        if (millis() - timerNext > 500)
        {
          timerNext = millis();
          sendCommand(LIGHT, 0);
          if (abs(*movement.Ry()) > abs(*movement.Rx()) && abs(*movement.Ry()) > abs(*movement.Rz()))
          {
            sendCommand(PLAY, 0);
          }
          else
          {
            sendCommand(NEXT, 0);
          }
          vibrateUntil(300);
        }
      }
    }
    else if(currState == SHAKER)
    {
      endState();
      sendCommand(LIGHT, lightLevel);
    }
    else if (stateChecker(ROLLER) && (*movement.Ay() < 100 && *movement.Ay() > 80))
    {
      if (currState == NONE && lastState != ROLLER)
      {
        timerRoller = millis();
      }
      currState = ROLLER;
      if (millis() - timerRoller > 10)
      {
        timerRoller = millis();
        uint16_t dx = abs(*movement.Ax() - lastValueX);

        if (dx < 45 && (*movement.Gxz() > 100 || *movement.Gxz() < -100))
        {
          if (*movement.Gxz() > 10)
          {
            forward = true;
          }
          else if (*movement.Gxz() < -10)
          {
            forward = false;
          }

          if (forward)
          {
            lightLevel = lightLevel + (int)(abs(dx)*10);
          }
          else
          {
            lightLevel = lightLevel - (int)(abs(dx)*10);
          }

          if (lightLevel > 4000)
            lightLevel = 4000;
          else if (lightLevel < 0)
            lightLevel = 0;
          lastValueX = *movement.Ax();
          if (lightLevel != lastLightLevel)
          {
            lastLightLevel = lightLevel;
            sendCommand(VOLUMELIGHT, lightLevel);
          }
        }
      }
      //sendCommand(BLINK, (1-sin(*movement.Ax()/RAD_TO_DEG))*3000);
    }
    else if(currState == ROLLER)
    {
      endState();
      sendCommand(LIGHT, lightLevel);
    }
  }
}

void setVol()
{
  if (sound.getVol() != vol)
  {
    sound.setVol(vol);
  }
}

static void Mp3Player::handleMessage(Message_t* msg)
{
  Command* data = (Command*)msg->data;
  if (isPlayer)
  {
    switch (data->command)
    {
      case LIGHT:
        light.setAll(data->value, data->value, data->value);
        light.update();
        break;
      case VOLUMELIGHT:
        vol = (uint8_t)7*(data->value/255.0);
        light.setAll(data->value, data->value, data->value);
        light.update();
        if (isPlaying)
        {
          sound.setVol(vol);
        }
        break;
      case VOLUME:
        sound.setVol(data->value);
        break;
      case NEXT:
        isPlaying = true;
        sound.next();
        vibrateUntil(300);
        delay(1000);
        if (sound.getNowPlaying() >= 2)
        {
          sound.playSong(0);
        }
        break;
      case PLAY:
        setVol();
        isPlaying = !isPlaying;
        sound.play();
        break;
      case STOP:
        isPlaying = false;
        stop();
        break;
      case PLAY_SONG:
        setVol();
        isPlaying = true;
        sound.playSong(data->value);
        break;
      case IS_PLAYING:
        if (sound.isPlaying())
          light.setAll(LIGHT_WHITE);
          //blink(2, 3000);
        else
          light.setAll(LIGHT_WHITE);
          //blink(1, 3000);
        break;
    }
  }
  switch(data->command)
  {
    case SET_AS_PLAYER:
      isPlayer = true;
      light.set(1, 100, 0, 100);
      light.update();
      break;
    case CALIBRATE:
      light.setAll(100, 100, 0);
      light.update();
      movement.calibrate();
      light.clearAll();
      light.update();
  }
}

static void Mp3Player::setup()
{
  command.id = MSG_COMMAND;
  currState = NONE;
  lastState = NONE;
  movement.calibrate();
  timer = millis();
  timerNext = 0;
  sound.setVol(vol);
  comm.subscribe(MSG_COMMAND, handleMessage);
  timerManager.createPeriodic(50, 50, onLoop);
}

