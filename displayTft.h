#ifndef _DISPLAY_TFT_H_
#define _DISPLAY_TFT_H_

#include "sysConfig.h"

#ifdef WITH_DISPLAY

#include <Arduino.h>

class DisplayTft {
public:
  static DisplayTft *getDisplay();
  virtual ~DisplayTft() {}

  virtual void setRotation(uint8_t r) {}
  virtual uint8_t getRotation() { return 0; }
  virtual uint16_t sizeX() { return 0; }
  virtual uint16_t sizeY() { return 0; }

  virtual bool getColors() { return true; }
  virtual void putColors(bool aShowColors=false) {}

  virtual void show(bool refresh=false) {}

protected:
  DisplayTft() {}
};

#endif // WITH_DISPLAY

#endif // _DISPLAY_TFT_H_
