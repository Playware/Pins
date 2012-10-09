
#include "RPApi.h"
#include <avr/eeprom.h>
#include "drivers/XBeeConf.h"

char EVENT_PLAY[] = "RA";
char EVENT_STOP[] = "RB";

void vibrateOff()
{
  vibrator.off();
}

void vibrateUntil(uint16_t ms) 
{
  vibrator.on();
  timerManager.createOneShot(ms, vibrateOff);
}

static bool isPlayingState = false;

void RP_act(char* eventType, void* event)
{
  // Check if the sound module has just started playing or 
  // just stopped and triggers an event accordingly.
  bool isPlaying = sound.isPlaying();
  if (isPlayingState != isPlaying)
  {
    isPlayingState = isPlaying;
    if (isPlaying)
      eventManager.triggerEvent(EVENT_PLAY, NULL);
    else
      eventManager.triggerEvent(EVENT_STOP, NULL);
  }
}

void RP_setup()
{
  eventManager.registerType(EVENT_PLAY);
  eventManager.registerType(EVENT_STOP);
  eventManager.subscribeListener(EVENT_ACT, COM_act);
  eventManager.subscribeListener(EVENT_ACT, TIM_act);
  eventManager.subscribeListener(EVENT_ACT, RP_act);

  // TODO: Register message handlers for reset, setInitialSettings
}


// Calibrate the accelerometer and the gyroscope and
// saves the values to eeprom.
void RP_calibrate()
{
  int16_t calibrateValues[6];
  movement.calibrate(calibrateValues);
  eeprom_write_block((const void*)&calibrateValues, (void*)0, sizeof(int16_t)*6);
}

// Loads calibration values for the accelerometer and the gyroscope.
void RP_loadCalibration()
{
  int16_t calibrateValues[6];
  eeprom_read_block((void*)&calibrateValues, (void*)0, sizeof(int16_t)*6);
  movement.setCalibrationValues(calibrateValues);
}

// When the pin is idle, it's good to have something running
// on the pin, too hint it's turned on. (Nice effects)
static uint8_t top[] = {6, 7};
static uint8_t middle[] = {0, 1, 2, 3};
static uint8_t bottom[] = {4, 5};
static uint8_t pos = 0;
static Timer_t* timer = NULL;
static uint8_t posValues[] = {200, 0, 0, 0};
static uint8_t step = 10;

static void timerIdle()
{
  if (posValues[pos] == 0)
    pos = (pos + 1) % 4;

  posValues[pos] -= step;
  posValues[(pos+1)%4] += step;
  light.clearAll();
  light.set(top, 2, posValues[0], posValues[0], posValues[0]);
  light.set(middle, 4, posValues[1], posValues[1], posValues[1]);
  light.set(bottom, 2, posValues[2], posValues[2], posValues[2]);
  light.update();
}

void RP_idleStart()
{
  if (timer == NULL)
    timer = timerManager.createPeriodic(0, 20, timerIdle);
}

void RP_idleStop()
{
  timerManager.remove(&timer);
}

void RP_makeTilesCompatable()
{
  char value;
  value = 0x03;
  XBeeAPIConf::setParam("BD", &value, 1);
  Serial.begin(9600);
  delay(100);
  value = 0x0F;
  XBeeAPIConf::setParam("CH", &value, 1);
}

/*
 * When a new pin is made, these functions need to be run below
 */

// Configure the XBee to use correct settings
// and save them as default.
void RP_configXBee()
{
  Serial.begin(9600);
  XBeeConf::setDefaults();
  Serial.begin(57600);
}

// Test functions
#define LIGHT40 30
#define LIGHT_ERROR LIGHT40, 0, 0
void RP_test()
{
  int idx = 0;
  int rounds = 0;
  uint8_t data = (uint8_t)'A';

  sound.playSong(0);
  vibrator.on();
  while(1)
  {
    switch(idx)
    {
      case 0:
        light.setAll(LIGHT40, 0, 0);
        break;
      case 1:
        light.setAll(0, LIGHT40, 0);
        break;
      case 2:
        light.setAll(0, 0, LIGHT40);
        break;
      case 3:
        light.setAll(LIGHT40, LIGHT40, LIGHT40);
        break;
    }
    light.update();
    comm.send(&data, 1);
    delay(1000);
    idx = (idx + 1) % 4;
    if (idx == 0)
      rounds++;
    if (idx == 1)
    {
      vibrator.off();
    }
/*    if (!sound.isPlaying())
    {
      light.setAll(LIGHT_PURPLE);
      light.update();
      delay(1000);
      sound.play();
    }*/
    if (rounds == 2)
    {
      break;
    }
  }
  light.clearAll();
  light.update();
}

static void blink(int idx1, int idx2, int times)
{
  light.clearAll();
  light.update();
  int i = 0;
  while(1)
  {
    light.set(idx1, LIGHT_ERROR);
    light.set(idx2, LIGHT_ERROR);
    light.update();
    delay(500);
    light.clearAll();
    light.update();
    i++;
    if (i == times)
      break;
    delay(100);
  }
}

static uint8_t limit(float value, int convert, uint8_t max)
{
  float result = abs(value*convert);
  if (result > max)
  {
    result = max;
  }
  return floor(result);
}

int mapping[] = {4, 1, 0, 6, 7, 3, 2, 5};
bool RP_test_xbee()
{
  // Xbee test
  Serial.begin(9600);
  delay(1000);
  bool success = XBeeConf::setParam("+++", "");
  if (!success)
  {
    delay(1000);
    Serial.begin(57600);
    success = XBeeConf::setParam("+++", "");
  }
  if (!success)
  {
    blink(mapping[1], mapping[6], 5);
    return false;
  }
  return true;
}

bool RP_test_sound()
{
  // Sound test
  bool success = true;
  sound.playSong(0);
  delay(2000);
  if (!sound.isPlaying())
  {
    success = false;
    blink(mapping[0], mapping[7], 5);
  }
  sound.stop();

  return success;
}

void RP_test_gyroacc()
{
  int convertAcc = floor(255/3);
  int convertGyro = floor(255/180);
  uint8_t red;
  uint8_t green;
  uint8_t blue;
  movement.calibrate();
  while(1)
  {
    movement.update();
    red = limit(*movement.Rx(), convertAcc, 255);
    green = limit(*movement.Ry(), convertAcc, 255);
    blue = limit(*movement.Rz(), convertAcc, 255);
    light.setAll(red, green, blue);

    red = limit(*movement.Gxy(), convertGyro, 255);
    green = limit(*movement.Gyz(), convertGyro, 255);
    blue = limit(*movement.Gxz(), convertGyro, 255);

    light.set(mapping[0], red, green, blue);
    light.set(mapping[7], red, green, blue);
    light.update();
    delay(100);
  }
}

void RP_test_light_and_vibrator()
{
  light.clearAll();
  light.update();
  delay(100);
  int i = 0;
  int idx = 0;
  int colorIdx = 0;
  uint8_t red = 0;
  uint8_t green = 0;
  uint8_t blue = 0;
  uint8_t error = 0;
  while (1)
  {
    idx = floor((i%24)/3);
    colorIdx = i % 3;
    red = 0;
    green = 0;
    blue = 0;
    switch (colorIdx)
    {
      case 0:
        red = LIGHT40;
        break;
      case 1:
        green = LIGHT40;
        break;
      case 2:
        blue = LIGHT40;
        break;
    }
    light.set(mapping[idx % 24], red, green, blue);
    light.update();
    delay(500);
    light.clearAll();
    light.update();
    delay(100);
    i = (i + 1);
    if ((i % 24) == 0)
    {
      vibrator.on();
      delay(1000);
      vibrator.off();
    }
    //break;
    if (i == 71)
    {
      // Three loops done, continue
      break;
    }
  }

}

void RP_test2()
{
  //RP_test();
  RP_test_light_and_vibrator();
  RP_test_sound();
  RP_test_xbee();
  RP_test_gyroacc();

}
