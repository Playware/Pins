
//nclude "Arduino.h"
#include "/usr/share/arduino/hardware/arduino/cores/arduino/Arduino.h"
#include "Sound.h"


void Sound::reset()
{
  digitalWrite(RESET_PIN, LOW);
  delay(1);
  digitalWrite(RESET_PIN, HIGH);
  nowPlayingIdx = 512;
  volumeLevel = MAX_VOL; // Maximum is default
  sendData_(PLAY); // To initialize 
}

/*
 * Setup pins
 */
void Sound::setup()
{
  DDRF = DDRF | B00000010;  // this is safer as it sets pins 1 as outputs
                      // without changing the value of the other pins 
  PORTF = (1 << PF1);       // Pin one on port F set high to Enable Audio amplifier 
  
  pinMode(DATA_PIN, OUTPUT);  // set pin 4 to output for Data
  pinMode(CLOCK_PIN, OUTPUT); // set pin 3 to output for data
  pinMode(RESET_PIN, OUTPUT); // set pin 7 to allow software reset
  pinMode(BUSY_PIN, INPUT);   // set pin 6 to monitor while Song is playing
  reset();
}

/*
 * Send data to SOMO 14D
 */
void Sound::sendData_(uint16_t data)
{
  uint16_t dataTmp = data;

  digitalWrite(CLOCK_PIN, HIGH); // Hold (idle) for 300msec to prepare data start
  //delay(300); // According to datasheet, it should be delayed by 300 ms
  delay(50); // But 50 ms seems to work
  digitalWrite(CLOCK_PIN, LOW); // Hold for 2msec to signal data start
  delay(2);

  for(uint8_t i=0; i<16; i++)
  {
    digitalWrite(CLOCK_PIN, LOW);
    if (dataTmp & 0x8000)
    {
      digitalWrite(DATA_PIN, HIGH);
    }
    else
    {
      digitalWrite(DATA_PIN, LOW);
    }
    dataTmp = dataTmp << 1;
    delayMicroseconds(200); // Clock cycle period is 200 uSec - LOW
    digitalWrite(CLOCK_PIN, HIGH);
    delayMicroseconds(200); // Clock cycle period is 200 uSec - HIGH
  }

  digitalWrite(DATA_PIN, LOW);
  digitalWrite(CLOCK_PIN, HIGH); // Place clock high to signal end of data
};

/*
 * Play / pause
 */
void Sound::play()
{
  if (isPlaying())
  {
    sendData_(PLAY);
  }
  else
  {
    sendData_(0);
  }
};

/*
 * Play song (specific song)
 */
void Sound::playSong(uint16_t idx)
{
  if (0 <= idx && idx < 512)
  {
    nowPlayingIdx = idx;
    sendData_(idx);
  }
  else
  {
    stop();
  }
};

/*
 * Stop playing
 */
void Sound::stop()
{
  nowPlayingIdx = 512;
  sendData_(STOP);
}

/*
 * Play next available song
 */
void Sound::next()
{
  if (!isPlaying())
    playSong(0);
  else
    playSong(nowPlayingIdx+1);
};

void Sound::prev()
{
  if (isPlaying())
    playSong(nowPlayingIdx-1);
};

/*
 * Set volume
 * level: Volume level, must be between 1 to 7
 */
void Sound::setVol(int8_t level)
{
  if (level < MIN_VOL)
    level = MIN_VOL;
  else if (MAX_VOL < level)
    level = MAX_VOL;
  if (volumeLevel != level)
  {
    volumeLevel = level;
    sendData_(volumeLevel + VOL_BASE);
  }
};

/*
 * Increase volume
 */
void Sound::incVol()
{
  setVol(volumeLevel+1);
};

/*
 * Decrease volume
 */
void Sound::decVol()
{
  setVol(volumeLevel-1);
};

uint8_t Sound::getVol()
{
  return volumeLevel;
}

uint16_t Sound::getNowPlaying()
{
  return nowPlayingIdx;
}

bool Sound::isPlaying()
{
  return digitalRead(BUSY_PIN) > 0;
}

Sound sound;
