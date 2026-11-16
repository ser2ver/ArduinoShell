#include "sysConfig.h"

#ifdef WITH_FS
#ifdef WITH_SD

#include <Arduino.h>
#include "SD.h"
#include "SPI.h"
#include "fsSD.h"

FsSD::FsSD(const char *a_fsName) : FsBase(a_fsName) {
  m_fsType = "SD";

#ifdef REASSIGN_PINS
  SPI.begin(sck, miso, mosi, cs);
  if (!SD.begin(cs)) {
#else
  if (!SD.begin()) {
#endif
    Serial.println("Card Mount Failed");
    return;
  }

  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE) {
    m_fsType = "NONE";
  } else if (cardType == CARD_MMC) {
    m_fsType = "MMC";
  } else if (cardType == CARD_SD) {
    m_fsType = "SDSC";
  } else if (cardType == CARD_SDHC) {
    m_fsType = "SDHC";
  } else {
    m_fsType = "UNKNOWN";
  }

  m_isEnabled = true;
}

FsSD::~FsSD() {
  SD.end();
}

uint64_t FsSD::getCapacity() const {
  return SD.cardSize();
}

uint64_t FsSD::getSize() const {
  return SD.totalBytes();
}

uint64_t FsSD::getUsed() const {
  return SD.usedBytes();
}

fs::FS *FsSD::getFS() const {
  return &SD;
}

#endif // WITH_SD
#endif // WITH_FS
