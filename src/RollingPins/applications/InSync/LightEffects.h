
#ifndef _LightEffects_h
#define _LightEffects_h

#include "RPApi.h"

class ColorEffect
{
  int16_t curColor[3];
  int16_t availColor[3];
  uint8_t colorIdx;
  uint8_t step;
  bool direction;
  uint8_t sequence[6]; // = {2, 1, 3, 2, 1, 3};
  uint8_t sequenceIdx;
  
  public:
    ColorEffect();
    void update();
    void setLights();
    void setLights(uint8_t strength);

  private:
    void test(); 
};

uint8_t getNextColor();
void setColorIdx(uint8_t colorIdx);
uint8_t getColorIdx();
void setColorByIdx(uint8_t, uint8_t);

#endif
