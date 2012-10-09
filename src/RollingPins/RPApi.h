
#ifndef RPApi_h
#define RPApi_h

#include "drivers/Light.h"
#include "drivers/Vibrator.h"
#include "drivers/Sound.h"
#include "drivers/Movement.h"
#include "drivers/XBeeAPIConf.h"
#include "controller/Worker/Worker.h"
#include "controller/EventManager/EventManager.h"
#include "controller/Communication/Communication.h"
#include "controller/Communication/Frame.h"
#include "controller/TimerManager/TimerManager.h"
#include "applications/Application.h"

extern char EVENT_PLAY[]; // Sound began playing
extern char EVENT_STOP[]; // Sound stoped playing

// For message compatibility with the Tiles, uncomment line:
//#define TILES_COMPATABLE

void vibrateOff();

void vibrateUntil(uint16_t ms);

void RP_setup();

void RP_loadCalibration();
void RP_calibrate();
void RP_configXBee();

void RP_makeTilesCompatable();

void RP_idleStart();
void RP_idleStop();

void RP_configXBee();
void RP_test();
void RP_test2();

#endif
