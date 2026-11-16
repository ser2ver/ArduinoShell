#include "sysConfig.h"

#ifdef WITH_TEMP

#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "dsTemp.h"

struct DsTempDevNode {
  DeviceAddress mDevAddr;
  char mAddr[18];
  float mTemp;
  DsTempDevNode *mNext;

  DsTempDevNode(DeviceAddress &aDevAddr, DsTempDevNode *aNext = NULL);
};

DsTempDevNode::DsTempDevNode(DeviceAddress &aDevAddr, DsTempDevNode *aNext) : mNext(aNext) {
  for (size_t i=0; i < 8; ++i) {
    mDevAddr[i] = aDevAddr[i];

    mAddr[i * 2] = ((mDevAddr[i] / 16 < 10) ? '0' : 'A' - 10) + mDevAddr[i] / 16;
    mAddr[i * 2 + 1] = ((mDevAddr[i] % 16 < 10) ? '0' : 'A' - 10) + mDevAddr[i] % 16;
  }
  mAddr[16] = '\0';
}

struct DsTempSelNode {
  int mId;
  char mAddr[18];
  size_t mDevInd;
  DsTempSelNode *mNext;

  DsTempSelNode(int aId, const char* aAddr, DsTempSelNode *aNext = NULL);
  void setAddr(const char* aAddr);
};

DsTempSelNode::DsTempSelNode(int aId, const char* aAddr, DsTempSelNode *aNext) : mId(aId), mDevInd(TEMP_DEV_NO_INDEX), mNext(aNext) {
  setAddr(aAddr);
}

void DsTempSelNode::setAddr(const char* aAddr) {
  size_t sLen = strlen(aAddr);
  if (sLen > sizeof(mAddr) - 1) {
    sLen = sizeof(mAddr) - 1;
  }
  size_t pos = 0;
  for ( ; pos < sLen; ++pos) {
    if (isspace(aAddr[pos])) {
      break;
    }
    mAddr[pos] = aAddr[pos];
  }
  mAddr[pos] = '\0';
}

class DsTempImpl : public DsTemp {
public:
  DsTempImpl();
  virtual ~DsTempImpl();

  virtual uint8_t getResol() { return mResol; }
  virtual void setResol(uint8_t aResol) { mResol = aResol; }
  virtual size_t scan();
  virtual time_t scanTime() { return mScanTime; }

  virtual size_t devCount() { return mDevCount; }
  virtual const char *devAddr(size_t ind);
  virtual uint8_t devResol(size_t ind);
  virtual float devTemp(size_t ind);
  virtual float devTempF(size_t ind);

  virtual size_t selCount() { return mSelCount; }
  virtual bool selSet(int id, const char *addr);
  virtual int selId(size_t ind);
  virtual const char *selAddr(size_t ind);
  virtual bool selHasDev(int id);
  virtual size_t selDevInd(int id);

private:
  OneWire *mOneWire;
  DallasTemperature *mSensors;

  uint8_t mResol;
  time_t mScanStart;
  time_t mScanTime;

  size_t mDevCount;
  DsTempDevNode *mDevHead;

  size_t mSelCount;
  DsTempSelNode *mSelHead;

  DsTempDevNode *getDevNode(size_t ind);
  void clearDevs();

  DsTempSelNode *getSelNodeByInd(size_t ind);
  DsTempSelNode *getSelNodeById(int id);
  void clearSels();
};

DsTempImpl *_dsTempObj = NULL;

DsTemp *DsTemp::getDsTemp() {
  if (!_dsTempObj) {
    _dsTempObj = new DsTempImpl();
  }
  return _dsTempObj;
}

DsTempImpl::DsTempImpl() {
  mOneWire = new OneWire(TEMP_PIN);
  mSensors = new DallasTemperature(mOneWire);

  mSensors->begin();

  mResol = 10;
  mScanStart = 0;
  mScanTime = 0;

  mDevCount = 0;
  mDevHead = NULL;

  mSelCount = 0;
  mSelHead = NULL;
}

DsTempImpl::~DsTempImpl() {
  delete mSensors;
  delete mOneWire;
  clearDevs();
  clearSels();
}

size_t DsTempImpl::scan() {
  if (mScanStart + TEMP_PERIOD < millis()) {
    mScanTime = millis();
    clearDevs();

    DsTempSelNode *st;
    for (st = mSelHead; st; st = st->mNext) {
      st->mDevInd = TEMP_DEV_NO_INDEX;
    }

    mSensors->requestTemperatures();
    mDevCount = mSensors->getDeviceCount();
    if (mDevCount > 0) {
      DeviceAddress tempAddr;
      DsTempDevNode *t;
      for (size_t i=0; i < mDevCount; ++i) {
        mSensors->getAddress(tempAddr, i);
        if (mSensors->getResolution(tempAddr) != mResol) {
          mSensors->setResolution(tempAddr, mResol);
        }

        if (!mDevHead) {
          mDevHead = new DsTempDevNode(tempAddr);
          t = mDevHead;
        } else {
          t = mDevHead;
          while (t->mNext) {
            t = t->mNext;
          }
          t->mNext = new DsTempDevNode(tempAddr);
          t = t->mNext;
        }
        t->mTemp = mSensors->getTempC(t->mDevAddr);

        for (st = mSelHead; st; st = st->mNext) {
          if (!strcmp(st->mAddr, t->mAddr)) {
            st->mDevInd = i;
            break;
          }
        }
      }
    }

    mScanStart = millis();
    mScanTime = millis() - mScanTime;
  }
  return devCount();
}

const char *DsTempImpl::devAddr(size_t ind) {
  const char *res = NULL;
  DsTempDevNode *t = getDevNode(ind);
  if (t) {
    res = t->mAddr;
  }
  return res;
}

uint8_t DsTempImpl::devResol(size_t ind) {
  uint8_t res = 0;
  DsTempDevNode *t = getDevNode(ind);
  if (t) {
    res = mSensors->getResolution(t->mDevAddr);
  }
  return res;
}

float DsTempImpl::devTemp(size_t ind) {
  float res = 0;
  DsTempDevNode *t = getDevNode(ind);
  if (t) {
    res = t->mTemp;
  }
  return res;
}

float DsTempImpl::devTempF(size_t ind) {
  return (ind < devCount()) ? DallasTemperature::toFahrenheit(devTemp(ind)) : 0;
}

bool DsTempImpl::selSet(int id, const char *addr) {
  if (addr) {
    if (!mSelHead) {
      mSelHead = new DsTempSelNode(id, addr);
    } else {
      DsTempSelNode *t = mSelHead;
      while (t->mNext) {
        if (t->mId == id) {
          break;
        }
        t = t->mNext;
      }
      if (t->mId == id) {
        t->setAddr(addr);
      } else {
        t->mNext = new DsTempSelNode(id, addr);
      }
    }
  } else {
    DsTempSelNode *t = mSelHead;
    DsTempSelNode *p = NULL;
    while (t) {
      if (t->mId == id) {
        break;
      }
      p = t;
      t = t->mNext;
    }
    if (t) {
      if (p) {
        p->mNext = t->mNext;
      } else {
        mSelHead = t->mNext;
      }
      delete t;
    }
  }

  mSelCount = 0;
  DsTempSelNode *st;
  for (st = mSelHead; st; st = st->mNext) {
    ++mSelCount;
  }

  return true;
}

int DsTempImpl::selId(size_t ind) {
  DsTempSelNode *t = getSelNodeByInd(ind);
  return t ? t->mId : -1;
}

const char *DsTempImpl::selAddr(size_t ind) {
  DsTempSelNode *t = getSelNodeByInd(ind);
  return t ? t->mAddr : NULL;
}

bool DsTempImpl::selHasDev(int id) {
  DsTempSelNode *t = getSelNodeById(id);
  return t ? true : false;
}
size_t DsTempImpl::selDevInd(int id) {
  DsTempSelNode *t = getSelNodeById(id);
  return t ? t->mDevInd : TEMP_NO_DEV_INDEX;
}

DsTempDevNode *DsTempImpl::getDevNode(size_t ind) {
  DsTempDevNode *t = NULL;
  if (ind < mDevCount) {
    for (t = mDevHead; t && ind; --ind) {
      t = t->mNext;
    }
  }
  return t;
}

void DsTempImpl::clearDevs() {
  DsTempDevNode *t;
  while (mDevHead) {
    t = mDevHead;
    mDevHead = t->mNext;
    delete t;
  }
}

DsTempSelNode *DsTempImpl::getSelNodeByInd(size_t ind) {
  DsTempSelNode *t = NULL;
  if (ind < mSelCount) {
    for (t = mSelHead; t && ind; --ind) {
      t = t->mNext;
    }
  }
  return t;
}

DsTempSelNode *DsTempImpl::getSelNodeById(int id) {
  DsTempSelNode *t = NULL;
  for (t = mSelHead; t; t = t->mNext) {
    if (t->mId == id) {
      break;
    }
  }
  return t;
}

void DsTempImpl::clearSels() {
  DsTempSelNode *t;
  while (mSelHead) {
    t = mSelHead;
    mSelHead = t->mNext;
    delete t;
  }
}

#endif // WITH_TEMP
