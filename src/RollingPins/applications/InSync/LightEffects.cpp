
#include "LightEffects.h"

#define MAX_LIGHT 255
#define MIN_LIGHT 0


void checkMinMax(int16_t* value)
{
  if (*value < MIN_LIGHT)
  {
    *value = MIN_LIGHT;
  }
  else if (MAX_LIGHT < *value)
  {
    *value = MAX_LIGHT;
  }
}
bool firstTime = true;
ColorEffect::ColorEffect()
{
  step = 20;
  curColor[0] = 255;
  curColor[1] = step;
  curColor[2] = 0;
  availColor[0] = 0;
  availColor[1] = 255-step;
  availColor[2] = 255;
  direction = true; // From cur to avail
  sequence[0] = 1;
  sequence[1] = 0;
  sequence[2] = 2;
  sequence[3] = 1;
  sequence[4] = 0;
  sequence[5] = 2;
  sequenceIdx = 0;
  colorIdx = sequence[sequenceIdx];
}

void ColorEffect::update()
{
  if ((curColor[colorIdx] == 0 || availColor[colorIdx] == 0))
  {
    sequenceIdx = (sequenceIdx + 1) % 6;
    colorIdx = sequence[sequenceIdx];
    direction = (curColor[colorIdx] == 0);
  }
  if (direction)
  {
    curColor[colorIdx] += step;
    availColor[colorIdx] -= step;
  }
  else
  {
    curColor[colorIdx] -= step;
    availColor[colorIdx] += step;
  }
  checkMinMax(&curColor[colorIdx]);
  checkMinMax(&availColor[colorIdx]);
}

void ColorEffect::setLights()
{
  setLights(100);
}


void ColorEffect::setLights(uint8_t strength)
{
  float convert = strength/100.0*3000.0/255.0;
  light.setAll(curColor[0]*convert, curColor[1]*convert, curColor[2]*convert);
  light.update();
}


uint8_t nextColorIdx = 0;
uint8_t getNextColor()
{
  nextColorIdx = (nextColorIdx + 1) % 8;
}

void setColorIdx(uint8_t colorIdx)
{
  nextColorIdx = colorIdx;
}

uint8_t getColorIdx()
{
  return nextColorIdx;
}

void setLight2(uint8_t strength, uint8_t red, uint8_t green, uint8_t blue)
{
  float strengthF = strength/100.0;
  light.setAll(red*strengthF, green*strengthF, blue*strengthF);
  light.update();
}

void setColorByIdx(uint8_t strength, uint8_t colorIdx)
{
  nextColorIdx = colorIdx;
  switch (colorIdx)
  {
    case 0:
      setLight2(strength, LIGHT_RED);
      break;
    case 1:
      setLight2(strength, LIGHT_GREEN);
      break;
    case 2:
      setLight2(strength, LIGHT_SOFTBLUE);
      break;
    case 3:
      setLight2(strength, LIGHT_YELLOW);
      break;
    case 4:
      setLight2(strength, LIGHT_CYAN);
      break;
    case 5:
      setLight2(strength, LIGHT_PURPLE);
      break;
    case 6:
      setLight2(strength, LIGHT_ORANGE);
      break;
    case 7:
      setLight2(strength, LIGHT_PINK);
      break;

  }
}
