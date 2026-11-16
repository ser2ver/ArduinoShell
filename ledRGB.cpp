#include "sysConfig.h"

#ifdef WITH_LED_RGB

#include "ledRGB.h"

// Настройки ШИМ
const uint32_t PWM_FREQ = 5000;
const uint8_t PWM_RESOL = 8;

struct CycleRGBNode {
  time_t mTmSec;
  uint32_t mColor;
  CycleRGBNode *mNext;

  CycleRGBNode(time_t aTmSec, uint32_t aColor, CycleRGBNode *aNext = NULL) : mTmSec(aTmSec), mColor(aColor), mNext(aNext) {}
};

class LedRGBImpl : public LedRGB {
public:
  LedRGBImpl();
  virtual ~LedRGBImpl();

  virtual void setRGB(uint8_t aRed, uint8_t aGreen, uint8_t aBlue);
  virtual void setHex(uint32_t aColor);
  virtual uint32_t getHex();

  virtual void addCycle(time_t aTmSec, uint32_t aColor);
  virtual void checkCycle();
  virtual void resetCycle();

  virtual size_t cycleSize();
  virtual time_t cycleTmSec(size_t ind);
  virtual uint32_t cycleColor(size_t ind);

private:
  uint8_t mRed;
  uint8_t mGreen;
  uint8_t mBlue;

  CycleRGBNode *mCycleHead;
  CycleRGBNode *mCyclePos;
  time_t mCycleStart;
};

LedRGBImpl *_ledRGBObj = NULL;

LedRGB *LedRGB::getLedRGB() {
  if (!_ledRGBObj) {
    _ledRGBObj = new LedRGBImpl();
  }
  return _ledRGBObj;
}

LedRGBImpl::LedRGBImpl() {
  mRed = 0;
  mGreen = 0;
  mBlue = 0;

  mCycleHead = NULL;
  mCyclePos = NULL;
  mCycleStart = 0;

  // Настройка ШИМ по новому стандарту 3.x
  if (!ledcAttach(LED_PIN_RED, PWM_FREQ, PWM_RESOL)) {
    Serial.println("Ошибка настройки КРАСНОГО пина!");
  }
  if (!ledcAttach(LED_PIN_GREEN, PWM_FREQ, PWM_RESOL)) {
    Serial.println("Ошибка настройки ЗЕЛЕНОГО пина!");
  }
  if (!ledcAttach(LED_PIN_BLUE, PWM_FREQ, PWM_RESOL)) {
    Serial.println("Ошибка настройки СИНЕГО пина!");
  }

  setRGB(mRed, mGreen, mBlue);
}

LedRGBImpl::~LedRGBImpl() {
  resetCycle();
  setRGB(0, 0, 0);

  ledcDetach(LED_PIN_RED);
  ledcDetach(LED_PIN_GREEN);
  ledcDetach(LED_PIN_BLUE);
}

void LedRGBImpl::setRGB(uint8_t aRed, uint8_t aGreen, uint8_t aBlue) {
  mRed = aRed;
  mGreen = aGreen;
  mBlue = aBlue;

  ledcWrite(LED_PIN_RED, 255 - mRed);
  ledcWrite(LED_PIN_GREEN, 255 - mGreen);
  ledcWrite(LED_PIN_BLUE, 255 - mBlue);
}

void LedRGBImpl::setHex(uint32_t aColor) {
  setRGB(aColor / 65536 % 256, aColor / 256 % 256, aColor % 256);
}

uint32_t LedRGBImpl::getHex() {
  return mRed * 65536 + mGreen * 256 + mBlue;
}

void LedRGBImpl::addCycle(time_t aTmSec, uint32_t aColor) {
  if (!mCycleHead) {
    mCycleHead = new CycleRGBNode(aTmSec, aColor);
    return;
  }

  CycleRGBNode *t = mCycleHead;
  while (t->mNext) {
    t = t->mNext;
  }
  t->mNext = new CycleRGBNode(aTmSec, aColor);
}

void LedRGBImpl::checkCycle() {
  if (!mCycleHead) {
    return;
  }
  if (!mCyclePos) {
    mCyclePos = mCycleHead;
    setHex(mCyclePos->mColor);
    mCycleStart = millis();
  }
  if (mCycleStart + mCyclePos->mTmSec < millis()) {
    mCyclePos = mCyclePos->mNext ? mCyclePos->mNext : mCycleHead;
    setHex(mCyclePos->mColor);
    mCycleStart = millis();
  }
}

void LedRGBImpl::resetCycle() {
  CycleRGBNode *t;
  while (mCycleHead) {
    t = mCycleHead;
    mCycleHead = t->mNext;
    delete t;
  }
  mCyclePos = NULL;
}

size_t LedRGBImpl::cycleSize() {
  size_t n = 0;
  CycleRGBNode *t = mCycleHead;
  while (t) {
    ++n;
    t = t->mNext;
  }
  return n;
}

time_t LedRGBImpl::cycleTmSec(size_t ind) {
  size_t ts = 0;
  CycleRGBNode *t = mCycleHead;
  while (t) {
    if (!ind) {
      ts = t->mTmSec;
      break;
    }
    --ind;
    t = t->mNext;
  }
  return ts;
}

uint32_t LedRGBImpl::cycleColor(size_t ind) {
  uint32_t c = 0;
  CycleRGBNode *t = mCycleHead;
  while (t) {
    if (!ind) {
      c = t->mColor;
      break;
    }
    --ind;
    t = t->mNext;
  }
  return c;
}

#endif // WITH_LED_RGB
