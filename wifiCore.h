#ifndef _WIFI_CORE_H_
#define _WIFI_CORE_H_

#include "sysConfig.h"

#ifdef WITH_WIFI

#include <Arduino.h>
#include <WiFiType.h>

class WiFiScan {
public:
  static WiFiScan scan();

  WiFiScan(const WiFiScan &old);
  virtual ~WiFiScan();

  uint16_t getNumItems() { return mNumItems; }

  String ssid(uint16_t ind);
  String bssid(uint16_t ind);
  int32_t rssi(uint16_t ind);
  int32_t channel(uint16_t ind);
  wifi_auth_mode_t encryptionType(uint16_t ind);
  const char *encryptionStr(uint16_t ind);

private:
  WiFiScan(uint16_t aNumItems);

  uint16_t *mCnt;
  uint16_t mNumItems;
};

class WiFiCore {
public:
  static WiFiCore *getWiFi();

  virtual ~WiFiCore() {}

  bool connect(const char *ssid, const char *pass);
  void disconnect();

  String macAddress();

  wl_status_t status();
  const char *statusStr();

  String ssid();
  String bssid();
  int rssi();

  String localIP();
  String gatewayIP();
  String dnsIP();
  String subnetMask();

  WiFiScan scan();

#ifdef WITH_WIFI_PING
  void ping();
#endif // WITH_WIFI_PING

protected:
  WiFiCore();
};

#endif // WITH_WIFI

#endif // _WIFI_CORE_H_
