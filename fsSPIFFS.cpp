#include "sysConfig.h"

#ifdef WITH_FS

#include <Arduino.h>
#include "SPIFFS.h"
#include "fsSPIFFS.h"

/* You only need to format SPIFFS the first time you run a
   test or else use the SPIFFS plugin to create a partition
   https://github.com/me-no-dev/arduino-esp32fs-plugin */
#define FORMAT_SPIFFS_IF_FAILED true

FsSPIFFS::FsSPIFFS(const char *a_fsName) : FsBase(a_fsName) {
  m_fsType = "SPIFFS";

  if (!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)) {
    Serial.println("SPIFFS Mount Failed");
    return;
  }

  m_isEnabled = true;
}

FsSPIFFS::~FsSPIFFS() {
  SPIFFS.end();
}

uint64_t FsSPIFFS::getCapacity() const {
  return SPIFFS.totalBytes();
}

uint64_t FsSPIFFS::getSize() const {
  return SPIFFS.totalBytes();
}

uint64_t FsSPIFFS::getUsed() const {
  return SPIFFS.usedBytes();
}

fs::FS *FsSPIFFS::getFS() const {
  return &SPIFFS;
}

#endif // WITH_FS
