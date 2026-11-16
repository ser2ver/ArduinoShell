#ifndef _DS_TEMP_H_
#define _DS_TEMP_H_

#include "sysConfig.h"

#ifdef WITH_TEMP

#include <Arduino.h>

class DsTemp {
public:
  static DsTemp *getDsTemp();

  virtual ~DsTemp() {}

  virtual uint8_t getResol() { return 0; }
  virtual void setResol(uint8_t aResol) {}
  virtual size_t scan() { return 0; }
  virtual time_t scanTime() { return 0; }

  virtual size_t devCount() { return 0; }
  virtual const char *devAddr(size_t ind) { return NULL; }
  virtual uint8_t devResol(size_t ind) { return 0; }
  virtual float devTemp(size_t ind) { return 0; }
  virtual float devTempF(size_t ind) { return 0; }

  virtual size_t selCount() { return 0; }
  virtual bool selSet(int id, const char *addr) { return false; }
  virtual int selId(size_t ind) { return -1; }
  virtual const char *selAddr(size_t ind) { return NULL; }
  virtual bool selHasDev(int id) { return false; }
  virtual size_t selDevInd(int id) { return TEMP_NO_DEV_INDEX; }

protected:
  DsTemp() {}
};

#endif // WITH_TEMP

#endif // _DS_TEMP_H_
