#ifndef _LED_RGB_H_
#define _LED_RGB_H_

#include "sysConfig.h"

#ifdef WITH_LED_RGB

#include <Arduino.h>

class LedRGB {
public:
  static LedRGB *getLedRGB();

  virtual ~LedRGB() {}

  virtual void setRGB(uint8_t aRed, uint8_t aGreen, uint8_t aBlue) {}
  virtual void setHex(uint32_t aColor) {}
  virtual uint32_t getHex() { return 0; }

  virtual void addCycle(time_t aTmSec, uint32_t aColor) {}
  virtual void checkCycle() {}
  virtual void resetCycle() {}

  virtual size_t cycleSize() { return 0; }
  virtual time_t cycleTmSec(size_t ind) { return 0; }
  virtual uint32_t cycleColor(size_t ind) { return 0; }

protected:
  LedRGB() {}
};

#endif // WITH_LED_RGB

#endif // _LED_RGB_H_
