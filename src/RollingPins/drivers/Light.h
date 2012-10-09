#ifndef Light_h
#define Light_h

#include <Arduino.h>

// Colors
#define LIGHT_RED     200,0,0
#define LIGHT_GREEN   0,200,0
#define LIGHT_BLUE    0,0,200
#define LIGHT_YELLOW  200,200,0
#define LIGHT_CYAN    0,200,200
#define LIGHT_PURPLE  200,0,200
#define LIGHT_BLACK   0,0,0
#define LIGHT_WHITE   200,200,200

#define LIGHT_ORANGE   200,100,0
#define LIGHT_SOFTBLUE   0,100,200
#define LIGHT_DARKPURPLE   100,0,200
#define LIGHT_PINK   200,0,100


// Defines for direct port access
#define CLKPORT PORTB
#define ENAPORT PORTB
#define LATPORT PORTB
#define DATPORT PORTB
#define ENAPIN  0        // SS   (BL/EN)
#define CLKPIN  1        // SCK  (CLOCK)
#define DATPIN  2        // MOSI (SIN)
#define LATPIN  3        // MISO (XLAT)

class Light
{
  // Array storing color values
  //  BLUE: LEDChannels[x][0]   Range: {0 to 4095}
  // GREEN: LEDChannels[x][1]   Range: {0 to 4095}
  //   RED: LEDChannels[x][2]   Range: {0 to 4095}
  uint16_t LEDChannels[8][3];

  // Defines for use with Arduino functions
  static const int ENABLE_PIN = 53; // BL
  static const int CLOCK_PIN  = 52; // CL
  static const int DATA_PIN   = 51; // SI
  static const int LATCH_PIN  = 50; // XL

  static const int LED_COUNT = 8;
  static const int CHANNEL_COUNT = 3;

  public:
    Light();
    void update();
    void set(uint8_t idx, uint8_t red, uint8_t green, uint8_t blue);
    void set(uint8_t* idxs, uint8_t length, uint8_t red, uint8_t green, uint8_t blue);
    void setAll(uint8_t red, uint8_t green, uint8_t blue);
    void clearAll();
};

extern Light light;

#endif
