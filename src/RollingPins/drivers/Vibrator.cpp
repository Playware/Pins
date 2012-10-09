
#include "Arduino.h"
#include "Vibrator.h"

#define sbi(port, bitnum) port |= _BV(bitnum)
#define cbi(port, bitnum) port &= ~_BV(bitnum)

#define readPin(port, bitnum) (_SFR_BYTE(port) & _BV(bitnum))

void Vibrator::setup()
{
  sbi(DDRD, PIN); // VIB
  cbi(PORTD, PIN);   
};

void Vibrator::on()
{
  sbi(PORTD, PIN);
};

void Vibrator::off()
{
  cbi(PORTD, PIN);
};

void Vibrator::toggle()
{
  if (readPin(PIND, PIN))
  {
    off();
  }
  else
  {
    on();
  }
};

Vibrator vibrator;
