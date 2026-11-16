#ifndef _SYS_CORE_H_
#define _SYS_CORE_H_

#include "sysConfig.h"

#ifdef WITH_SYSTEM

class SysCore {
public:
  static SysCore *getSystem();

  virtual ~SysCore() {}

  double upTimeF();
  const char *upTimeS();

  unsigned long getHeapSize();
  unsigned long getFreeHeap();
  unsigned long getMinFreeHeap();
  unsigned long getMaxAllocHeap();
  unsigned long getPsramSize();

  const char *getVersionArduinoEsp();
  const char *getVersionEspIdf();
  const char *getChipModel();
  int getChipRevision();
  unsigned long getChipCores();
  unsigned long getCpuFreqMHz();
  unsigned long getFlashChipSize();

  void reboot();

protected:
  SysCore() {}
};

#endif // WITH_SYSTEM

#endif // _SYS_CORE_H_
