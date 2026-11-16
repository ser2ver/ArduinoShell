#include "sysConfig.h"

#ifdef WITH_WIFI

#include <Arduino.h>
#include <WiFi.h>
#include "wifiCore.h"

#ifdef WITH_WIFI_PING
#include "ping/ping_sock.h"
#include "argtable3/argtable3.h"
#endif // WITH_WIFI_PING

WiFiScan WiFiScan::scan() {
  return WiFiScan(WiFi.scanNetworks());
}

WiFiScan::WiFiScan(uint16_t aNumItems) : mCnt(NULL), mNumItems(aNumItems) {
  mCnt = new uint16_t;
  *mCnt = 1;
}

WiFiScan::WiFiScan(const WiFiScan &old) : mCnt(old.mCnt), mNumItems(old.mNumItems) {
  ++(*mCnt);
}

WiFiScan::~WiFiScan() {
  if (--(*mCnt) <= 0) {
    WiFi.scanDelete();
    delete mCnt;
  }
}

String WiFiScan::ssid(uint16_t ind) {
  return (ind < mNumItems) ? WiFi.SSID(ind) : String();
}

String WiFiScan::bssid(uint16_t ind) {
  return (ind < mNumItems) ? WiFi.BSSIDstr(ind) : String();
}

int32_t WiFiScan::rssi(uint16_t ind) {
  return (ind < mNumItems) ? WiFi.RSSI(ind) : 0;
}

int32_t WiFiScan::channel(uint16_t ind) {
  return (ind < mNumItems) ? WiFi.channel(ind) : 0;
}

wifi_auth_mode_t WiFiScan::encryptionType(uint16_t ind) {
  return (ind < mNumItems) ? WiFi.encryptionType(ind) : WIFI_AUTH_OPEN;
}

const char *WiFiScan::encryptionStr(uint16_t ind) {
  if (ind >= mNumItems) {
    return "";
  }

  const char *enc;
  switch (encryptionType(ind)) {
    case WIFI_AUTH_OPEN:            enc = "OPEN"; break;
    case WIFI_AUTH_WEP:             enc = "WEP"; break;
    case WIFI_AUTH_WPA_PSK:         enc = "WPA PSK"; break;
    case WIFI_AUTH_WPA2_PSK:        enc = "WPA2 PSK"; break;
    case WIFI_AUTH_WPA_WPA2_PSK:    enc = "WPA/WPA2 PSK"; break;
    case WIFI_AUTH_WPA2_ENTERPRISE: enc = "WPA2 ENTERPRISE"; break;
    case WIFI_AUTH_WPA3_PSK:        enc = "WPA3 PSK"; break;
    case WIFI_AUTH_WPA2_WPA3_PSK:   enc = "WPA2/WPA3 PSK"; break;
    case WIFI_AUTH_WAPI_PSK:        enc = "WPAPI PSK"; break;
    default:                        enc = "UNKNOWN"; break;
  }
  return enc;
}


WiFiCore *_wifiCoreObj = NULL;

WiFiCore *WiFiCore::getWiFi() {
  if (!_wifiCoreObj) {
    _wifiCoreObj = new WiFiCore();
  }
  return _wifiCoreObj;
}

WiFiCore::WiFiCore() {
  WiFi.mode(WIFI_STA);
  WiFi.begin();
}

bool WiFiCore::connect(const char *ssid, const char *pass) {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  return true;
}

void WiFiCore::disconnect() {
  WiFi.mode(WIFI_OFF);
  Serial.println("WiFi disconnected.");
}

wl_status_t WiFiCore::status() {
  return WiFi.status();
}

const char * WiFiCore::statusStr() {
  wl_status_t s = status();
  const char *statusStr;
  switch (s) {
    case WL_IDLE_STATUS:     statusStr = "Idle"; break;
    case WL_NO_SSID_AVAIL:   statusStr = "SSID not found"; break;
    case WL_SCAN_COMPLETED:  statusStr = "Scan completed"; break;
    case WL_CONNECTED:       statusStr = "Connected"; break;
    case WL_CONNECT_FAILED:  statusStr = "Connect failed"; break;
    case WL_CONNECTION_LOST: statusStr = "Connection lost"; break;
    case WL_DISCONNECTED:    statusStr = "Disconnected"; break;
    case WL_STOPPED:         statusStr = "WiFi stopped"; break;
    case WL_NO_SHIELD:       statusStr = "No WiFi module"; break;
    default:                 statusStr = "Unknown"; break;
  }
  return statusStr;
}

String WiFiCore::ssid() {
  return (status() == WL_CONNECTED) ? WiFi.SSID() : String();
}

String WiFiCore::bssid() {
  return (status() == WL_CONNECTED) ? WiFi.BSSIDstr() : String();
}

int WiFiCore::rssi() {
  return (status() == WL_CONNECTED) ? WiFi.RSSI() : 0;
}

String WiFiCore::localIP() {
  return (status() == WL_CONNECTED) ? WiFi.localIP().toString() : String();
}

String WiFiCore::gatewayIP() {
  return (status() == WL_CONNECTED) ? WiFi.gatewayIP().toString() : String();
}

String WiFiCore::dnsIP() {
  return (status() == WL_CONNECTED) ? WiFi.dnsIP().toString() : String();
}

String WiFiCore::subnetMask() {
  return (status() == WL_CONNECTED) ? WiFi.subnetMask().toString() : String();
}

String WiFiCore::macAddress() {
  return WiFi.macAddress();
}

WiFiScan WiFiCore::scan() {
  return WiFiScan::scan();
}

#endif // WITH_WIFI
