

#include "RPApi.h"
#include "Pairing.h"
#include "LightEffects.h"

#define MAX_PERIOD 1000

#define LIGHT2_RED     100,0,0
#define LIGHT2_GREEN   0,100,0
#define LIGHT2_BLUE    0,0,100
#define LIGHT2_YELLOW  100,100,0
#define LIGHT2_CYAN    0,100,100
#define LIGHT2_PURPLE  100,0,100
#define LIGHT2_BLACK   0,0,0
#define LIGHT2_WHITE   100,100,100
#define LIGHT2_ORANGE   100,50,0


namespace InSync
{
  static void setup(void);
}

Application_t inSync = {
  "InSync",
  InSync::setup
};

static char MSG_PEAK = 0;
static char MSG_SCORE = 1;
static char MSG_COMMAND = 2;

struct Peak_msg
{
  uint8_t msgType;
  long period;
  long timestamp;
} peakMsgOut, *peakMsgIn;

struct Score_msg
{
  uint8_t msgType;
  uint8_t score;
  uint8_t colorIdx;
  uint8_t soundLevel;
  uint16_t scoreNumber;
} scoreMsgOut;
Score_msg* scoreMsgIn = NULL;

struct Command_msg
{
  uint8_t msgType;
  uint8_t command;
  uint16_t value;
} commandMsg;

enum Command_type {CMD_CALIBRATE, CMD_WINDOW_SIZE_PERCENT, CMD_SET_VOL, CMD_PLAY_SONG, CMD_DUMMY, PLAY, STOP, STATUS};

static uint16_t songIdx = 5;
static PairingManager_t* pairingManager = NULL;
static long timeOffset = 0;
static long syncDelay = 0;

static long lastRemotePeak = 0;
static long lastPeak = 0;
static long window = 0;
static long lastScoreOccured = 0;
static uint16_t scoreNumber = 0;
static Timer_t* waitForPeakTimer = NULL;
static uint8_t scoreMsgResent = 3;

static long periodHistory[3] = {1000, 1000, 1000};
static uint8_t periodHistoryIdx = 2;
static long period = 0;
static long lastPeakOccured = 0;
static bool pauseColorTimer = false;
static uint8_t colorIdx;

static float windowSizePercent = 0.7;

static uint8_t scoreHistory[3] = {0, 0, 0};
static uint8_t scoreHistoryIdx = 2;

static void onNewScore(Score_msg* score)
{
  if (score->scoreNumber > scoreNumber)
  {
    if (score->score > 10)
    {
      //vibrateUntil(score->score/100.0*period/2);
    }
    sound.setVol(score->soundLevel);
    if (score->score < 20 && !pairingManager->isMaster)
    {
      setColorByIdx(sound.getVol()/7.0*100, score->colorIdx-2);
    }
    else
    {
      setColorByIdx(sound.getVol()/7.0*100, score->colorIdx);
    }
    waitForPeakTimer = NULL;
    lastScoreOccured = millis();
    scoreNumber = score->scoreNumber;
  }
  else if (!pairingManager->isMaster && score->scoreNumber == scoreNumber)
  {
    sound.setVol(score->soundLevel);
    setColorIdx(score->colorIdx);
  }
}

static void updateScore()
{
  if (lastRemotePeak != 0 || lastPeak != 0)
  {
    // Find the window from the periods from the pins
    long distance = abs(lastRemotePeak - lastPeak);

    uint8_t scoreValue = 0;
    if (distance < window)
    {
      uint16_t value = 100 - (100.0*distance/window);
      scoreValue = value*value/100.0;
      lastRemotePeak = 0;
      lastPeak = 0;
    }

    scoreMsgOut.colorIdx = getNextColor();
    if (scoreValue > 50)
    {
      sound.incVol();
    }
    else if (scoreValue < 20)
    {
      sound.decVol();
    }
    scoreMsgOut.score = scoreValue;
    scoreMsgOut.soundLevel = sound.getVol();
    scoreMsgOut.scoreNumber = scoreNumber + 1;
    comm.send((uint8_t*) &scoreMsgOut, sizeof(struct Score_msg));
    scoreMsgResent = 0;
    onNewScore(&scoreMsgOut);
  }
}

static void onWaitForPeakTimer()
{
  if (waitForPeakTimer != NULL)
  {
    updateScore();
    lastRemotePeak = 0;
    lastPeak = 0;
  }
}

static bool onPeakDetected()
{
  window = floor((windowSizePercent)*(period + peakMsgIn->period)/2.0);
  if (lastRemotePeak != 0 && lastPeak != 0)
  {
    updateScore();
  }
  else
  {
    waitForPeakTimer = timerManager.createOneShot(window+syncDelay+10, onWaitForPeakTimer);
  }
}

static float highestValue = 0;
static int highestCount = 0;

static bool isPeakDetected()
{
  bool result = false;
  if (*movement.R() > 2.3)
  {
    if (*movement.R() > highestValue)
    {
      highestValue = *movement.R();
      highestCount = 0;
    }
    else if (highestCount > 2)
    {
      highestCount = 0;
      highestValue = 0;
      return true;
    }
    else
    {
      highestCount++;
    }
  }
  else if (highestValue != 0)
  {
    result = true;
    highestValue = 0;
  }
  return result;
}

static void updatePeriodNow()
{
  if (lastPeak == 0)
  {
    period = MAX_PERIOD;
  }
  else
  {
    period = millis() - lastPeak;
    if (period > MAX_PERIOD)
      period = MAX_PERIOD;
  }
  lastPeak = millis();
  periodHistoryIdx = (periodHistoryIdx + 1) % 3;
  periodHistory[periodHistoryIdx] = period;
  //period = (periodHistory[0] + periodHistory[1] + periodHistory[2])/3;
}

static void onSampleTimer()
{
  movement.update(); // about 6-7 ms
  if (isPeakDetected())// && (millis() - lastPeak) > (period*windowSizePercent))
  {
    updatePeriodNow();
    // Broadcast peak
    peakMsgOut.period = period;
    peakMsgOut.timestamp = millis() - timeOffset;
    comm.send((uint8_t*) &peakMsgOut, sizeof(struct Peak_msg));
    onPeakDetected();
    vibrateUntil(period/4);
  }
}

static void onDowngradeTimer()
{
  if ((millis() - lastScoreOccured) > 2*period)
  {
    sound.decVol();
    if (scoreMsgIn != NULL)
      setColorByIdx(sound.getVol()/7.0*100, getColorIdx());
  }
}

static void onSendScoreTimer()
{
  if (scoreMsgResent < 3)
  {
    comm.send((uint8_t*) &scoreMsgOut, sizeof(struct Score_msg));
    scoreMsgResent++;
  }
}

static void handlePeakMessage(Message_t* msg)
{
  peakMsgIn = (Peak_msg*)msg->data;
  lastRemotePeak = millis();
  onPeakDetected();
}

// TODO: remove sometime
static void handleCmdMessage(Message_t* msg)
{
  Command_msg* command = (Command_msg*)msg->data;

  switch(command->command)
  {
    case CMD_CALIBRATE:
      light.setAll(LIGHT_RED);
      light.update();
      RP_calibrate();
      light.clearAll();
      light.update();
      break;
    case CMD_WINDOW_SIZE_PERCENT:
      windowSizePercent = command->value/100.0;
      break;
    case CMD_SET_VOL:
      sound.setVol(command->value);
      break;
    case CMD_PLAY_SONG:
      songIdx = command->value;
      sound.playSong(command->value);
      break;
    case PLAY:
      sound.play();
      break;
    case STOP:
      sound.stop();
      break;
  }
}

static void handleScoreMessage(Message_t* msg)
{
  scoreMsgIn = (Score_msg*)msg->data;
  onNewScore(scoreMsgIn);
}

static void onMusicStop(char* eventType, void* event)
{
  // When a song stops playing, play it again
  sound.playSong(songIdx);
}

ColorEffect colorObject;
static void onColorTimer()
{
  if (!pauseColorTimer)
  {
    colorObject.update();
    colorObject.setLights();
  }
}

static void onPaired()
{
  timerManager.createPeriodic(50, 50, onSampleTimer);
  timerManager.createPeriodic(1000, 1000, onDowngradeTimer);
  if (pairingManager->isMaster)
    timerManager.createPeriodic(100, 100, onSendScoreTimer);
  eventManager.subscribeListener(EVENT_STOP, onMusicStop);
  sound.setVol(1);
  sound.playSong(songIdx);
  setColorByIdx(sound.getVol()/7.0*100, getColorIdx());
}

static void InSync::setup(void)
{
  peakMsgOut.msgType = MSG_PEAK;
  scoreMsgOut.msgType = MSG_SCORE;
  commandMsg.msgType = MSG_COMMAND;

  comm.subscribe(MSG_PEAK, handlePeakMessage);
  comm.subscribe(MSG_SCORE, handleScoreMessage);
  comm.subscribe(MSG_COMMAND, handleCmdMessage);
  pairingManager = Pairing::start(onPaired);
}
