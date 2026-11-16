#include "sysConfig.h"

#ifdef WITH_TIME

#include "wifiCore.h"
#include "sysRTC.h"
#include "esp_sntp.h"

// Callback function (gets called when time adjusts via NTP)
void _timeavailable(struct timeval *t) {
  SysRTC *sysTm = SysRTC::getSysRTC();
  sysTm->setSync(true);

  Serial.println("Got time adjustment from NTP!");
  Serial.print("Current time: ");
  Serial.println(sysTm->getTimeInfo(), "%Y-%m-%d %H:%M:%S");
}

SysRTC *_sysRTCObj = NULL;

SysRTC *SysRTC::getSysRTC() {
  if (!_sysRTCObj) {
    _sysRTCObj = new SysRTC();
  }
  return _sysRTCObj;
}

SysRTC::SysRTC() {
  mIsSync = false;
  mTz[0] = '\0';
  mNtp1[0] = '\0';
  mNtp2[0] = '\0';
  mNtp3[0] = '\0';

  setTz("MSK-3");
  setNtp1("pool.ntp.org");
  setNtp2("time.nist.gov");
  applyConfig();

#ifdef WITH_WIFI
  WiFiCore *wifi = WiFiCore::getWiFi();
  if (wifi->status() == WL_CONNECTED) {
    esp_sntp_servermode_dhcp(1);
    sntp_set_time_sync_notification_cb(_timeavailable);
  }
#endif // WITH_WIFI
}

struct tm *SysRTC::getTimeInfo() {
  static struct tm _timeInfo;
  if (!fillTimeInfo(&_timeInfo)) {
    return NULL;
  }
  return &_timeInfo;
}

struct tm *SysRTC::newTimeInfo() {
  struct tm *_timeInfo = new struct tm;
  if (!fillTimeInfo(_timeInfo)) {
    delete _timeInfo;
    return NULL;
  }
  return _timeInfo;
}

void SysRTC::setTz(const char *aTz) {
  size_t len = strlen(aTz);
  len = (sizeof(mTz) - 1 < len) ? sizeof(mTz) - 1 : len;
  strncpy(mTz, aTz, len);
  mTz[len] = '\0';
}

void SysRTC::setNtp1(const char *aNtp1) {
  size_t len = strlen(aNtp1);
  len = (sizeof(mNtp1) - 1 < len) ? sizeof(mNtp1) - 1 : len;
  strncpy(mNtp1, aNtp1, len);
  mNtp1[len] = '\0';
}

void SysRTC::setNtp2(const char *aNtp2) {
  size_t len = strlen(aNtp2);
  len = (sizeof(mNtp2) - 1 < len) ? sizeof(mNtp2) - 1 : len;
  strncpy(mNtp2, aNtp2, len);
  mNtp2[len] = '\0';
}

void SysRTC::setNtp3(const char *aNtp3) {
  size_t len = strlen(aNtp3);
  len = (sizeof(mNtp3) - 1 < len) ? sizeof(mNtp3) - 1 : len;
  strncpy(mNtp3, aNtp3, len);
  mNtp3[len] = '\0';
}

void SysRTC::applyConfig() {
#ifdef WITH_WIFI
  WiFiCore *wifi = WiFiCore::getWiFi();
  if (wifi->status() == WL_CONNECTED) {
    configTzTime(mTz, (*mNtp1 ? mNtp1 : NULL), (*mNtp2 ? mNtp2 : NULL), (*mNtp3 ? mNtp3 : NULL));
  }
#endif // WITH_WIFI
}

bool SysRTC::fillTimeInfo(struct tm *aTm) {
#ifdef WITH_WIFI
  WiFiCore *wifi = WiFiCore::getWiFi();
  if ((wifi->status() == WL_CONNECTED) || mIsSync) {
    if (!mIsSync) {
      applyConfig();
      esp_sntp_servermode_dhcp(1);
      sntp_set_time_sync_notification_cb(_timeavailable);
    }
    if (getLocalTime(aTm, 200)) {
      return true;
    }
  }
#endif // WITH_WIFI
  return false;
}

#endif // WITH_TIME
