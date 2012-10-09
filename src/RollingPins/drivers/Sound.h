/*
 * Arduino library for SOMO-14D (Sound Module)
 *
 * Common function to play sound file with SOMO-14D and arduino.
 *
 * Author: Arnþór Magnússon <arnthorm@gmail.com>
 * Created: 6. Jun 2011
 * Updated: 6. Jun 2011 
 * 
 */

#ifndef Sound_h
#define Sound_h

//#include "Arduino.h"
#include "/usr/share/arduino/hardware/arduino/cores/arduino/Arduino.h"
class Sound
{
  static const uint8_t CLOCK_PIN = 24;
  static const uint8_t DATA_PIN  = 25;
  static const uint8_t RESET_PIN = 26;
  static const uint8_t BUSY_PIN = 54;

  static const uint16_t PLAY = 0xFFFE;
  static const uint16_t STOP = 0xFFFF;
  static const uint8_t MIN_VOL = 1;
  static const uint8_t MAX_VOL = 7;
  static const uint16_t VOL_BASE = 0xFFF0;

  uint8_t nowPlayingIdx;
  int8_t volumeLevel;
  public:
    void reset();
    void setup();
    void play();
    void playSong(uint16_t idx);
    void stop();
    void next();
    void prev();
    void setVol(int8_t level);
    void incVol();
    void decVol();
    uint8_t getVol();
    uint16_t getNowPlaying();
    bool isPlaying();

  private:
    void sendData_(uint16_t data);
};

extern Sound sound;

#endif
