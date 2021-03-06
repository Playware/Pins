#ifndef CRC16_h
#define CRC16_h

#include <stdint.h>

/**
  This updates the given CRC16 sum according to the given data and CRC
  @param __crc the value of the current crc (set this to 0 when calculating a CRC from starters)
  @param __data the data the CRC should be calculated over.
  @returns the updated CRC sum.
 */
static uint16_t crc16_update(uint16_t __crc, uint8_t __data) {
  uint16_t i,j;
  for(i = 0; i != 8; __data >>= 1, i++) {
    j = (__data^__crc)&1;
    __crc >>= 1;
    if(j)
      __crc^=0xa001;
  }
  return __crc;
}

static uint16_t calc_crc16(uint8_t* data, uint8_t length) {
	uint16_t crc = 0;

	for ( uint8_t i = 2; i < length - 3; i++) {
		crc = crc16_update(crc, data[i]);
	}

	return crc;
}

#endif

