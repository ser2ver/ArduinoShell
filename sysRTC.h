#ifndef _SYS_RTC_H_
#define _SYS_RTC_H_

#include "sysConfig.h"

#ifdef WITH_TIME

#include <Arduino.h>

class SysRTC {
public:
  static SysRTC *getSysRTC();

  virtual ~SysRTC() {}

  virtual struct tm *getTimeInfo();
  virtual struct tm *newTimeInfo();

  virtual bool isSync() { return mIsSync; }
  virtual void setSync(bool aIsSync) { mIsSync = aIsSync; }

  /**
   * A more convenient approach to handle TimeZones with daylightOffset
   * would be to specify a environment variable with TimeZone definition including daylight adjustmnet rules.
   * A list of rules for your zone could be obtained from https://github.com/esp8266/Arduino/blob/master/cores/esp8266/TZ.h
   */
  virtual const char *getTz() { return mTz; }
  virtual const char *getNtp1() { return mNtp1; }
  virtual const char *getNtp2() { return mNtp2; }
  virtual const char *getNtp3() { return mNtp3; }

  virtual void setTz(const char *aTz);
  virtual void setNtp1(const char *aNtp1);
  virtual void setNtp2(const char *aNtp2);
  virtual void setNtp3(const char *aNtp3);

  virtual void applyConfig();

protected:
  SysRTC();

private:
  bool mIsSync;
  char mTz[64];
  char mNtp1[64];
  char mNtp2[64];
  char mNtp3[64];

  bool fillTimeInfo(struct tm *aTm);
};

#endif // WITH_TIME

#endif // _SYS_RTC_H_
