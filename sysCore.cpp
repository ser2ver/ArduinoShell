#include "sysConfig.h"

#ifdef WITH_SYSTEM

#include <Arduino.h>
#include "sysUtil.h"
#include "sysCore.h"

SysCore *_sysCoreObj = NULL;

SysCore *SysCore::getSystem() {
  if (!_sysCoreObj) {
    _sysCoreObj = new SysCore();
  }
  return _sysCoreObj;
}

double SysCore::upTimeF() {
  unsigned long ms = millis();
  return ms / 1000.0;
}

const char *SysCore::upTimeS() {
  static char buf[20];
  size_t pos = 0;
  unsigned long ms = millis();
  bool isStart = false;

  if (ms >= 86400000) {
    pos = intToStr(ms / 86400000, buf, pos);
    ms %= 86400000;
    buf[pos++] = 'd';
    buf[pos++] = ' ';
    isStart = true;
  }
  if (isStart || (ms >= 3600000)) {
    pos = intToStr(ms / 3600000, buf, pos, isStart ? 2 : 0);
    ms %= 3600000;
    buf[pos++] = ':';
    isStart = true;
  }
  if (isStart || (ms >= 60000)) {
    pos = intToStr(ms / 60000, buf, pos, isStart ? 2 : 0);
    ms %= 60000;
    buf[pos++] = ':';
    isStart = true;
  }
  pos = intToStr(ms / 1000, buf, pos, isStart ? 2 : 0);
  ms %= 1000;
  buf[pos++] = '.';

  pos = intToStr(ms, buf, pos, 3);

  return buf;
}

unsigned long SysCore::getHeapSize() {
  return ESP.getHeapSize();
}

unsigned long SysCore::getFreeHeap() {
  return ESP.getFreeHeap();
}

unsigned long SysCore::getMinFreeHeap() {
  return ESP.getMinFreeHeap();
}

unsigned long SysCore::getMaxAllocHeap() {
  return ESP.getMaxAllocHeap();
}

unsigned long SysCore::getPsramSize() {
  return ESP.getPsramSize();
}

const char *SysCore::getVersionArduinoEsp() {
  return ESP_ARDUINO_VERSION_STR;
}

const char *SysCore::getVersionEspIdf() {
  return esp_get_idf_version();
}

const char *SysCore::getChipModel() {
  return ESP.getChipModel();
}

int SysCore::getChipRevision() {
  return ESP.getChipRevision();
}

unsigned long SysCore::getChipCores() {
  return ESP.getChipCores();
}

unsigned long SysCore::getCpuFreqMHz() {
  return ESP.getCpuFreqMHz();
}

unsigned long SysCore::getFlashChipSize() {
  return ESP.getFlashChipSize();
}

void SysCore::reboot() {
  ESP.restart();
}

#endif // WITH_SYSTEM
