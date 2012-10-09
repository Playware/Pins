
//#include "Arduino.h"
#include "/usr/share/arduino/hardware/arduino/cores/arduino/Arduino.h"
#include "Light.h"

Light::Light()
{
  pinMode(DATA_PIN, OUTPUT);
  pinMode(LATCH_PIN, OUTPUT);
  pinMode(ENABLE_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);

  digitalWrite(LATCH_PIN, LOW);
  digitalWrite(ENABLE_PIN, LOW);
};

void Light::update()
{
  unsigned int tempOne = 0;

  for (int i = 0; i < ((3*LED_COUNT)); i++)
  {
    tempOne = *(&LEDChannels[0][0] + i);

    for (int j = 0; j < 12; j++)
    {
      if ((tempOne >> (11 - j)) & 1)
      {
        DATPORT |= (1 << DATPIN);
      }
      else
      {
        DATPORT &= ~(1 << DATPIN);
      }

      CLKPORT |= (1 << CLKPIN);
      CLKPORT &= ~(1 << CLKPIN);
    }
  }
  LATPORT |= (1 << LATPIN);
  LATPORT &= ~(1 << LATPIN);
};


static int conv = 15; // = 3825/255
void Light::set(uint8_t idx, uint8_t red, uint8_t green, uint8_t blue)
{
  /*LEDChannels[idx][0] = blue;
  LEDChannels[idx][1] = green;
  LEDChannels[idx][2] = red;*/
  if (idx < 4)
  {
    LEDChannels[idx][0] = red*conv;
    LEDChannels[idx][1] = green*conv;
  }
  else
  {
    LEDChannels[idx][0] = green*conv;
    LEDChannels[idx][1] = red*conv;
  }

  LEDChannels[idx][2] = blue*conv;

};

void Light::set(uint8_t* idxs, uint8_t length, uint8_t red, uint8_t green, uint8_t blue)
{
  for (int i = 0; i < length; i++)
  {
    set(idxs[i], red, green, blue);
  }
}

void Light::setAll(uint8_t red, uint8_t green, uint8_t blue)
{
  for (int i=0; i<LED_COUNT; i++)
  {
    set(i, red, green, blue);
  }
};

void Light::clearAll()
{
  setAll(0, 0, 0);
};

Light light;
