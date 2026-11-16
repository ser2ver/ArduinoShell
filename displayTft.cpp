#include "sysConfig.h"

#ifdef WITH_DISPLAY

#include <Arduino.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include "displayTft.h"

#include "sysUtil.h"
#include "sysCore.h"
#include "sysRTC.h"
#include "wifiCore.h"
#include "espServer.h"
#include "dsTemp.h"

class DisplayTftImpl : public DisplayTft {
public:
  DisplayTftImpl();
  virtual ~DisplayTftImpl();

  virtual void setRotation(uint8_t r);
  virtual uint8_t getRotation() { return mRotation; }
  virtual uint16_t sizeX();
  virtual uint16_t sizeY();

  virtual bool getColors() { return mShowColors; }
  virtual void putColors(bool aShowColors=false);

  virtual void show(bool refresh=false);

private:
  uint16_t mColorBorder;
  uint8_t mRotation;
  bool mShowColors;
  TFT_eSPI *mTft;

  uint16_t getColor(uint8_t red, uint8_t green, uint8_t blue);
  void showBorder(bool refresh);
  void showSys(bool refresh);
  void showTime(bool refresh);
  void showWiFi(bool refresh);
  void showTemp(bool refresh);
  void showColors(bool refresh);

  void drawMemBar(int16_t x, int16_t y, uint16_t w, uint16_t h);
  void drawWiFiLevel(int16_t x, int16_t y, bool refresh=false);
  void drawTemp(int id, uint16_t color, int16_t x, int16_t y);
};

DisplayTftImpl *_displayTftObj = NULL;

DisplayTft *DisplayTft::getDisplay() {
  if (!_displayTftObj) {
    _displayTftObj = new DisplayTftImpl();
  }
  return _displayTftObj;
}

DisplayTftImpl::DisplayTftImpl() {
  mColorBorder = TFT_SILVER;
  mRotation = 5;
  mShowColors = false;
  mTft = new TFT_eSPI();
  mTft->init();
  setRotation(0);
}

DisplayTftImpl::~DisplayTftImpl() {
  delete mTft;
}

void DisplayTftImpl::setRotation(uint8_t r) {
  if (r != mRotation) {
    mRotation = r;
    mTft->setRotation(mRotation);
    show(true);
  }
}

uint16_t DisplayTftImpl::sizeX() {
  return mTft->width();
}

uint16_t DisplayTftImpl::sizeY() {
  return mTft->height();
}

void DisplayTftImpl::putColors(bool aShowColors) {
  if (mShowColors != aShowColors) {
    mShowColors = aShowColors;
    show(true);
  }
}

void DisplayTftImpl::show(bool refresh) {
  showBorder(refresh);
  showSys(refresh);
  showTime(refresh);
  showWiFi(refresh);
  if (mShowColors) {
    showColors(refresh);
  } else {
    showTemp(refresh);
  }
}

uint16_t DisplayTftImpl::getColor(uint8_t red, uint8_t green, uint8_t blue) {
  return (uint16_t)(red % 32) << 11 | (uint16_t)(green % 64) << 5 | (uint16_t)(blue % 32);
}

void DisplayTftImpl::showBorder(bool refresh) {
  if (refresh) {
    mTft->fillScreen(TFT_BLACK);
  }

  mTft->drawFastHLine(0, 0, mTft->width() - 1, mColorBorder);
  mTft->drawFastHLine(0, mTft->height() - 1, mTft->width() - 1, mColorBorder);
  mTft->drawFastVLine(0, 0, mTft->height() - 1, mColorBorder);
  mTft->drawFastVLine(mTft->width() - 1, 0, mTft->height() - 1, mColorBorder);
}

void DisplayTftImpl::showSys(bool refresh) {
  uint16_t colorUpTime = TFT_MAGENTA;
  uint16_t colorCntReq = TFT_GREEN;
#ifdef WITH_SYSTEM
  SysCore *sys = SysCore::getSystem();
#endif // WITH_SYSTEM
#ifdef WITH_WIFI and WITH_ESP_SERVER
  EspServer *serv = EspServer::getServer();
#endif // WITH_ESP_SERVER and WITH_WIFI

  if (mRotation % 2) {
    mTft->drawFastHLine(0, mTft->height() - 30, mTft->width() - 1, mColorBorder);
    mTft->drawLine(160, mTft->height() - 30, 160, mTft->height() - 1, mColorBorder);
    mTft->drawLine(254, mTft->height() - 30, 254, mTft->height() - 1, mColorBorder);

    mTft->setTextDatum(BL_DATUM);
    mTft->setTextColor(colorUpTime, TFT_BLACK);
#ifdef WITH_SYSTEM
    mTft->drawNumber(sys->getFreeHeap(), 164, mTft->height() - 1, 4);
#else
    mTft->drawNumber(323232, 164, mTft->height() - 1, 4);
#endif // WITH_SYSTEM

    mTft->setTextDatum(BR_DATUM);
    mTft->setTextColor(colorUpTime, TFT_BLACK);
#ifdef WITH_SYSTEM
    mTft->drawString(sys->upTimeS(), mTft->width() - 5, mTft->height() - 1, 4);
#else
    mTft->drawString("333d 23:55:55.333", mTft->width() - 5, mTft->height() - 1, 4);
#endif // WITH_SYSTEM

    drawMemBar(2, mTft->height() - 28, 157, 26);

    mTft->setTextDatum(TR_DATUM);
    mTft->setTextColor(colorCntReq, TFT_BLACK);
#ifdef WITH_WIFI and WITH_ESP_SERVER
    mTft->drawNumber(serv->countRequest(), 470, mTft->height() - 1, 4);
#else
    mTft->drawNumber(1234567, 470, mTft->height() - 1, 4);
#endif // WITH_WIFI and WITH_ESP_SERVER
  } else {
    mTft->drawFastHLine(0, 420, mTft->width() - 1, mColorBorder);
    mTft->drawFastHLine(0, mTft->height() - 31, mTft->width() - 1, mColorBorder);
    mTft->drawLine(94, mTft->height() - 31, 94, mTft->height() - 1, mColorBorder);

    mTft->setTextDatum(BL_DATUM);
    mTft->setTextColor(colorUpTime, TFT_BLACK);
#ifdef WITH_SYSTEM
    mTft->drawNumber(sys->getFreeHeap(), 4, mTft->height() - 1, 4);
#else
    mTft->drawNumber(323232, 4, mTft->height() - 1, 4);
#endif // WITH_SYSTEM

    mTft->setTextDatum(BR_DATUM);
    mTft->setTextColor(colorUpTime, TFT_BLACK);
#ifdef WITH_SYSTEM
    mTft->drawString(sys->upTimeS(), mTft->width() - 5, mTft->height() - 1, 4);
#else
    mTft->drawString("333d 23:55:55.333", mTft->width() - 5, mTft->height() - 1, 4);
#endif // WITH_SYSTEM

    drawMemBar(2, 422, mTft->width() - 4, 26);

    mTft->setTextDatum(TR_DATUM);
    mTft->setTextColor(colorCntReq, TFT_BLACK);
#ifdef WITH_WIFI and WITH_ESP_SERVER
    mTft->drawNumber(serv->countRequest(), mTft->width() - 10, mTft->height() - 1, 4);
#else
    mTft->drawNumber(1234567, 4, mTft->height() - 1, 4);
#endif // WITH_WIFI and WITH_ESP_SERVER
  }
}

void DisplayTftImpl::showTime(bool refresh) {
  static char strDate[12];
  static char strTime[12];
  uint16_t colorDate = TFT_SKYBLUE;
  uint16_t colorTime = TFT_YELLOW;
  struct tm *stm = NULL;

  if (mRotation % 2) {
    mTft->drawFastHLine(0, 110, mTft->width() - 1, mColorBorder);
  } else {
    mTft->drawFastHLine(0, 180, mTft->width() - 1, mColorBorder);
  }

#ifdef WITH_TIME
  SysRTC *sysTm = SysRTC::getSysRTC();
  stm = sysTm->getTimeInfo();
#endif // WITH_TIME

  if (stm) {
    strftime(strDate, 12, ((mRotation % 2) ? "%d.%m.%y " : " %d.%m.%Y"), stm);
    strftime(strTime, 12, "%H:%M:%S", stm);
  } else {
    snprintf(strDate, sizeof(strDate), ((mRotation % 2) ? "01.01.70 " : " 01.01.1970"));
    snprintf(strTime, sizeof(strTime), "00:00:00");
  }

  if (mRotation % 2) {
    mTft->setTextDatum(TL_DATUM);
    mTft->setTextColor(colorDate, TFT_BLACK);
    mTft->drawString(strDate, 8, 55, 6);

    mTft->setTextDatum(TR_DATUM);
    mTft->setTextColor(colorTime, TFT_BLACK);
    mTft->drawString(strTime, mTft->width() - 8, 45, 7);
  } else {
    mTft->setTextDatum(TR_DATUM);
    mTft->setTextColor(colorDate, TFT_BLACK);
    mTft->drawString(strDate, mTft->width() - 8, 40, 6);

    mTft->setTextDatum(TC_DATUM);
    mTft->setTextColor(colorTime, TFT_BLACK);
    mTft->drawString(strTime, mTft->width() / 2, 105, 7);
  }
}

void DisplayTftImpl::showWiFi(bool refresh) {
  static int oldRssi = 100;
  uint16_t colorWiFi = getColor(0, 31, 31);
  uint16_t colorWiFiLow = getColor(0, 10, 15);
  uint16_t colorWiFiNo = getColor(31, 15, 7);
#ifdef WITH_WIFI
  static uint8_t oldStatus = WL_NO_SHIELD;
  WiFiCore *wifi = WiFiCore::getWiFi();
#endif // WITH_WIFI

  if (mRotation % 2) {
    mTft->drawLine(40, 30, mTft->width() - 1, 30, mColorBorder);
#ifdef WITH_WIFI
    if (oldStatus != wifi->status()) {
      mTft->fillRect(41, 1, mTft->width() - 42, 28, TFT_BLACK);
      oldStatus = wifi->status();
    }
    if (wifi->status() == WL_CONNECTED) {
#endif // WITH_WIFI
      mTft->drawLine(280, 0, 280, 30, mColorBorder);
#ifdef WITH_WIFI
    }
#endif // WITH_WIFI
    mTft->drawLine(40, 1, 40, 40, mColorBorder);
    mTft->drawLine(1, 40, 40, 40, mColorBorder);

    mTft->setTextDatum(TL_DATUM);
    mTft->setTextColor(colorWiFi, TFT_BLACK);
#ifdef WITH_WIFI
    if (wifi->status() == WL_CONNECTED) {
      mTft->drawString(wifi->ssid().c_str(), 46, 4, 4);
    } else {
      mTft->drawString("Status:", 46, 4, 4);
      mTft->drawString(wifi->statusStr(), 136, 4, 4);
    }
#else
    mTft->drawString("APname", 4, 4, 4);
#endif // WITH_WIFI

    mTft->setTextDatum(TR_DATUM);
    mTft->setTextColor(colorWiFi, TFT_BLACK);
#ifdef WITH_WIFI
    if (wifi->status() == WL_CONNECTED) {
      mTft->drawString(wifi->localIP().c_str(), mTft->width() - 5, 4, 4);
    }
#else
    mTft->drawString("222.222.222.222", mTft->width() - 5, 4, 4);
#endif // WITH_WIFI

    drawWiFiLevel(0, 0, refresh);
  } else {
    mTft->drawFastHLine(0, 30, mTft->width() - 1, mColorBorder);
#ifdef WITH_WIFI
    if (oldStatus != wifi->status()) {
      mTft->fillRect(1, 1, mTft->width() - 2, 28, TFT_BLACK);
      oldStatus = wifi->status();
    }
    if (wifi->status() == WL_CONNECTED) {
#endif // WITH_WIFI
      mTft->drawLine(120, 0, 120, 30, mColorBorder);
#ifdef WITH_WIFI
    }
#endif // WITH_WIFI
    mTft->drawLine(40, 30, 40, 70, mColorBorder);
    mTft->drawLine(0, 70, 40, 70, mColorBorder);

    mTft->setTextDatum(TL_DATUM);
    mTft->setTextColor(colorWiFi, TFT_BLACK);
#ifdef WITH_WIFI
    if (wifi->status() == WL_CONNECTED) {
      mTft->drawString(wifi->ssid().c_str(), 4, 4, 4);
    } else {
      mTft->drawString("Status:", 6, 4, 4);
      mTft->drawString(wifi->statusStr(), 96, 4, 4);
    }
#else
    mTft->drawString("APname", 4, 4, 4);
#endif // WITH_WIFI

    mTft->setTextDatum(TR_DATUM);
    mTft->setTextColor(colorWiFi, TFT_BLACK);
#ifdef WITH_WIFI
    if (wifi->status() == WL_CONNECTED) {
      mTft->drawString(wifi->localIP().c_str(), mTft->width() - 5, 4, 4);
    }
#else
    mTft->drawString("222.222.222.222", mTft->width() - 5, 4, 4);
#endif // WITH_WIFI

    drawWiFiLevel(0, 30, refresh);
  }
}

void DisplayTftImpl::showTemp(bool refresh) {
  uint16_t colorHot = getColor(31, 16, 8);
  uint16_t colorCold = getColor(15, 31, 31);
  uint16_t colorHome = TFT_LIGHTGREY;
#ifdef WITH_TEMP
  DsTemp *dsTemp = DsTemp::getDsTemp();
  dsTemp->scan();
#endif // WITH_TEMP

  if (mRotation % 2) {
    uint16_t posX = 0, posY = 110, h = 60;

    mTft->drawLine(160, posY, 160, posY + h * 3, mColorBorder);
    mTft->drawLine(320, posY, 320, posY + h * 3, mColorBorder);
    mTft->drawFastHLine(0, posY + h * 1, mTft->width() - 1, mColorBorder);
    mTft->drawLine(0, posY + h * 2, 320, posY + h * 2, mColorBorder);

    drawTemp(1, colorHot,  150, posY + 10);
    drawTemp(2, colorCold, 310, posY + 10);
    drawTemp(3, colorHot,  150, posY + h + 10);
    drawTemp(4, colorCold, 310, posY + h + 10);
    drawTemp(5, colorHot,  150, posY + h * 2 + 10);
    drawTemp(6, colorCold, 310, posY + h * 2 + 10);
    drawTemp(7, colorHome, 470, posY + 10);
  } else {
    uint16_t posX = 0, posY = 180, h = 60;

    mTft->drawLine(mTft->width() / 2, posY, mTft->width() / 2, posY + h * 4, mColorBorder);
    mTft->drawFastHLine(0, posY + h * 1, mTft->width() - 1, mColorBorder);
    mTft->drawFastHLine(0, posY + h * 2, mTft->width() - 1, mColorBorder);
    mTft->drawFastHLine(0, posY + h * 3, mTft->width() - 1, mColorBorder);

    drawTemp(1, colorHot,  mTft->width() / 2 - 10, posY + 10);
    drawTemp(2, colorCold, mTft->width() - 10, posY + 10);
    drawTemp(3, colorHot,  mTft->width() / 2 - 10, posY + h + 10);
    drawTemp(4, colorCold, mTft->width() - 10, posY + h + 10);
    drawTemp(5, colorHot,  mTft->width() / 2 - 10, posY + h * 2 + 10);
    drawTemp(6, colorCold, mTft->width() - 10, posY + h * 2 + 10);
    drawTemp(7, colorHome, mTft->width() / 2 - 10, posY + h * 3 + 10);
  }
}

void DisplayTftImpl::showColors(bool refresh) {
//#define TFT_BLACK       0x0000      /*   0,   0,   0 */
//#define TFT_NAVY        0x000F      /*   0,   0, 128 */
//#define TFT_DARKGREEN   0x03E0      /*   0, 128,   0 */
//#define TFT_DARKCYAN    0x03EF      /*   0, 128, 128 */
//#define TFT_MAROON      0x7800      /* 128,   0,   0 */
//#define TFT_PURPLE      0x780F      /* 128,   0, 128 */
//#define TFT_OLIVE       0x7BE0      /* 128, 128,   0 */
//#define TFT_LIGHTGREY   0xD69A      /* 211, 211, 211 */
//#define TFT_DARKGREY    0x7BEF      /* 128, 128, 128 */
//#define TFT_BLUE        0x001F      /*   0,   0, 255 */
//#define TFT_GREEN       0x07E0      /*   0, 255,   0 */
//#define TFT_CYAN        0x07FF      /*   0, 255, 255 */
//#define TFT_RED         0xF800      /* 255,   0,   0 */
//#define TFT_MAGENTA     0xF81F      /* 255,   0, 255 */
//#define TFT_YELLOW      0xFFE0      /* 255, 255,   0 */
//#define TFT_WHITE       0xFFFF      /* 255, 255, 255 */
//#define TFT_ORANGE      0xFDA0      /* 255, 180,   0 */
//#define TFT_GREENYELLOW 0xB7E0      /* 180, 255,   0 */
//#define TFT_PINK        0xFE19      /* 255, 192, 203 */ //Lighter pink, was 0xFC9F
//#define TFT_BROWN       0x9A60      /* 150,  75,   0 */
//#define TFT_GOLD        0xFEA0      /* 255, 215,   0 */
//#define TFT_SILVER      0xC618      /* 192, 192, 192 */
//#define TFT_SKYBLUE     0x867D      /* 135, 206, 235 */
//#define TFT_VIOLET      0x915C      /* 180,  46, 226 */

  if (mRotation % 2) {
    uint16_t posX = 20, posY = 135, w = 55, h = 25;

    mTft->fillRect(posX + (w + 10) * 0, posY + (h + 10) * 0, w, h, TFT_BLACK);
    mTft->fillRect(posX + (w + 10) * 0, posY + (h + 10) * 1, w, h, TFT_NAVY);
    mTft->fillRect(posX + (w + 10) * 0, posY + (h + 10) * 2, w, h, TFT_DARKGREEN);
    mTft->fillRect(posX + (w + 10) * 0, posY + (h + 10) * 3, w, h, TFT_DARKCYAN);

    mTft->fillRect(posX + (w + 10) * 1, posY + (h + 10) * 0, w, h, TFT_MAROON);
    mTft->fillRect(posX + (w + 10) * 1, posY + (h + 10) * 1, w, h, TFT_PURPLE);
    mTft->fillRect(posX + (w + 10) * 1, posY + (h + 10) * 2, w, h, TFT_OLIVE);
    mTft->fillRect(posX + (w + 10) * 1, posY + (h + 10) * 3, w, h, TFT_LIGHTGREY);

    mTft->fillRect(posX + (w + 10) * 2, posY + (h + 10) * 0, w, h, TFT_DARKGREY);
    mTft->fillRect(posX + (w + 10) * 2, posY + (h + 10) * 1, w, h, TFT_BLUE);
    mTft->fillRect(posX + (w + 10) * 2, posY + (h + 10) * 2, w, h, TFT_GREEN);
    mTft->fillRect(posX + (w + 10) * 2, posY + (h + 10) * 3, w, h, TFT_CYAN);

    mTft->fillRect(posX + (w + 10) * 3, posY + (h + 10) * 0, w, h, TFT_RED);
    mTft->fillRect(posX + (w + 10) * 3, posY + (h + 10) * 1, w, h, TFT_MAGENTA);
    mTft->fillRect(posX + (w + 10) * 3, posY + (h + 10) * 2, w, h, TFT_YELLOW);
    mTft->fillRect(posX + (w + 10) * 3, posY + (h + 10) * 3, w, h, TFT_WHITE);

    mTft->fillRect(posX + (w + 10) * 4, posY + (h + 10) * 0, w, h, TFT_ORANGE);
    mTft->fillRect(posX + (w + 10) * 4, posY + (h + 10) * 1, w, h, TFT_GREENYELLOW);
    mTft->fillRect(posX + (w + 10) * 4, posY + (h + 10) * 2, w, h, TFT_PINK);
    mTft->fillRect(posX + (w + 10) * 4, posY + (h + 10) * 3, w, h, TFT_BROWN);

    mTft->fillRect(posX + (w + 10) * 5, posY + (h + 10) * 0, w, h, TFT_GOLD);
    mTft->fillRect(posX + (w + 10) * 5, posY + (h + 10) * 1, w, h, TFT_SILVER);
    mTft->fillRect(posX + (w + 10) * 5, posY + (h + 10) * 2, w, h, TFT_SKYBLUE);
    mTft->fillRect(posX + (w + 10) * 5, posY + (h + 10) * 3, w, h, TFT_VIOLET);

    mTft->fillRect(posX + (w + 10) * 6, posY + (h + 10) * 0, w, h, getColor(31, 31, 15));
    mTft->fillRect(posX + (w + 10) * 6, posY + (h + 10) * 1, w, h, getColor(15, 31, 31));
    mTft->fillRect(posX + (w + 10) * 6, posY + (h + 10) * 2, w, h, getColor(31, 16, 8));
    mTft->fillRect(posX + (w + 10) * 6, posY + (h + 10) * 3, w, h, getColor(8, 16, 31));
  } else {
    uint16_t posX = 25, posY = 200, w = 60, h = 20;

    mTft->fillRect(posX + (w + 10) * 0, posY + (h + 10) * 0, w, h, TFT_BLACK);
    mTft->fillRect(posX + (w + 10) * 1, posY + (h + 10) * 0, w, h, TFT_NAVY);
    mTft->fillRect(posX + (w + 10) * 2, posY + (h + 10) * 0, w, h, TFT_DARKGREEN);
    mTft->fillRect(posX + (w + 10) * 3, posY + (h + 10) * 0, w, h, TFT_DARKCYAN);

    mTft->fillRect(posX + (w + 10) * 0, posY + (h + 10) * 1, w, h, TFT_MAROON);
    mTft->fillRect(posX + (w + 10) * 1, posY + (h + 10) * 1, w, h, TFT_PURPLE);
    mTft->fillRect(posX + (w + 10) * 2, posY + (h + 10) * 1, w, h, TFT_OLIVE);
    mTft->fillRect(posX + (w + 10) * 3, posY + (h + 10) * 1, w, h, TFT_LIGHTGREY);

    mTft->fillRect(posX + (w + 10) * 0, posY + (h + 10) * 2, w, h, TFT_DARKGREY);
    mTft->fillRect(posX + (w + 10) * 1, posY + (h + 10) * 2, w, h, TFT_BLUE);
    mTft->fillRect(posX + (w + 10) * 2, posY + (h + 10) * 2, w, h, TFT_GREEN);
    mTft->fillRect(posX + (w + 10) * 3, posY + (h + 10) * 2, w, h, TFT_CYAN);

    mTft->fillRect(posX + (w + 10) * 0, posY + (h + 10) * 3, w, h, TFT_RED);
    mTft->fillRect(posX + (w + 10) * 1, posY + (h + 10) * 3, w, h, TFT_MAGENTA);
    mTft->fillRect(posX + (w + 10) * 2, posY + (h + 10) * 3, w, h, TFT_YELLOW);
    mTft->fillRect(posX + (w + 10) * 3, posY + (h + 10) * 3, w, h, TFT_WHITE);

    mTft->fillRect(posX + (w + 10) * 0, posY + (h + 10) * 4, w, h, TFT_ORANGE);
    mTft->fillRect(posX + (w + 10) * 1, posY + (h + 10) * 4, w, h, TFT_GREENYELLOW);
    mTft->fillRect(posX + (w + 10) * 2, posY + (h + 10) * 4, w, h, TFT_PINK);
    mTft->fillRect(posX + (w + 10) * 3, posY + (h + 10) * 4, w, h, TFT_BROWN);

    mTft->fillRect(posX + (w + 10) * 0, posY + (h + 10) * 5, w, h, TFT_GOLD);
    mTft->fillRect(posX + (w + 10) * 1, posY + (h + 10) * 5, w, h, TFT_SILVER);
    mTft->fillRect(posX + (w + 10) * 2, posY + (h + 10) * 5, w, h, TFT_SKYBLUE);
    mTft->fillRect(posX + (w + 10) * 3, posY + (h + 10) * 5, w, h, TFT_VIOLET);

    mTft->fillRect(posX + (w + 10) * 0, posY + (h + 10) * 6, w, h, getColor(31, 31, 15));
    mTft->fillRect(posX + (w + 10) * 1, posY + (h + 10) * 6, w, h, getColor(15, 31, 31));
    mTft->fillRect(posX + (w + 10) * 2, posY + (h + 10) * 6, w, h, getColor(31, 16, 8));
    mTft->fillRect(posX + (w + 10) * 3, posY + (h + 10) * 6, w, h, getColor(8, 16, 31));
  }
}

void DisplayTftImpl::drawMemBar(int16_t x, int16_t y, uint16_t w, uint16_t h) {
  static uint32_t oldMemFill = 0;
  static uint32_t oldMemMax = 0;
  static uint32_t oldMemBlock = 0;

  uint16_t colorBorder = getColor(0, 30, 0);
  uint16_t colorFill = getColor(12, 30, 0);
  uint16_t colorMaxFill = getColor(31, 30, 12);
  uint16_t colorMaxBlock = getColor(30, 60, 30);
  uint32_t memFill = 0.70 * w;
  uint32_t memMax = 0.80 * w;
  uint32_t memBlock = 0.85 * w;
#ifdef WITH_SYSTEM
  SysCore *sys = SysCore::getSystem();
  memFill = (uint32_t)((float)(sys->getHeapSize() - sys->getFreeHeap()) / sys->getHeapSize() * (w - 2));
  memMax = (uint32_t)((float)(sys->getHeapSize() - sys->getMinFreeHeap()) / sys->getHeapSize() * (w - 2));
  memBlock = (uint32_t)((float)(sys->getMaxAllocHeap()) / sys->getHeapSize() * (w - 2));
#endif // WITH_SYSTEM

  if (oldMemFill != memFill) {
    mTft->fillRect(x + 1, y + 1, memFill, h - 2, colorFill);
    mTft->fillRect(x + 1 + memFill, y + 1, w - memFill - 2, h - 2, TFT_BLACK);
    oldMemFill = memFill;
  } else {
    if (oldMemMax != memMax) {
      mTft->drawWideLine(x + 1 + oldMemMax, y + 1, x + 1 + oldMemMax, y + h - 2, 3, TFT_BLACK, TFT_BLACK);
    }
    if (oldMemBlock != memBlock) {
      mTft->drawWideLine(x + w - oldMemBlock - 2, y + 1, x + w - oldMemBlock - 2, y + h - 2, 3, TFT_BLACK, TFT_BLACK);
    }
  }
  oldMemMax = memMax;
  oldMemBlock = memBlock;

  mTft->drawWideLine(x + 1 + memMax, y + 1, x + 1 + memMax, y + h - 2, 3, colorMaxFill, TFT_BLACK);
  mTft->drawWideLine(x + w - memBlock - 2, y + 1, x + w - memBlock - 2, y + h - 2, 3, colorMaxBlock, TFT_BLACK);
  mTft->drawRect(x, y, w, h, colorBorder);
}

void DisplayTftImpl::drawWiFiLevel(int16_t x, int16_t y, bool refresh) {
  static int oldRssi = 100;
  uint16_t colorWiFi = getColor(0, 31, 31);
  uint16_t colorWiFiLow = getColor(0, 10, 15);
  uint16_t colorWiFiNo = getColor(31, 15, 7);
#ifdef WITH_WIFI
  WiFiCore *wifi = WiFiCore::getWiFi();
#endif // WITH_WIFI

  if (refresh) {
    oldRssi = 100;
  }

  int rssi = 0;
#ifdef WITH_WIFI
  if (wifi->status() == WL_CONNECTED) {
    rssi = wifi->rssi();
  }
#else
  static int rssiGen = 0;
  static time_t rssiNext = 0;
  if (rssiNext < millis()) {
    rssiGen -= 5;
    if (rssiGen < -100) {
      rssiGen = 20;
    }
    rssiNext = millis() + 1000;
  }
  rssi = rssiGen;
#endif // WITH_WIFI

  if (rssi > -10) {
    rssi = -200;
  }
  rssi = rssi / 10 * 10;

  if (oldRssi != rssi) {
    mTft->fillRect(x + 1, y + 1, 38, 38, TFT_BLACK);
    mTft->drawCircle(x + 20, y + 32, 2, rssi <= -90 ? colorWiFiLow : colorWiFi);
    mTft->drawCircle(x + 20, y + 32, 3, rssi <= -90 ? colorWiFiLow : colorWiFi);
    mTft->drawArc(x + 20, y + 32, 9, 7, 140, 220, rssi <= -80 ? colorWiFiLow : colorWiFi, TFT_BLACK);
    mTft->drawArc(x + 20, y + 32, 15, 13, 140, 220, rssi <= -70 ? colorWiFiLow : colorWiFi, TFT_BLACK);
    mTft->drawArc(x + 20, y + 32, 21, 19, 140, 220, rssi <= -60 ? colorWiFiLow : colorWiFi, TFT_BLACK);
    mTft->drawArc(x + 20, y + 32, 27, 25, 140, 220, rssi <= -50 ? colorWiFiLow : colorWiFi, TFT_BLACK);
    if (rssi < -120) {
      mTft->drawWideLine(x + 5, y + 5, x + 35, y + 35, 5, colorWiFiNo, TFT_BLACK);
      mTft->drawWideLine(x + 35, y + 5, x + 5, y + 35, 5, colorWiFiNo, TFT_BLACK);
    }
    oldRssi = rssi;
  }
}

void DisplayTftImpl::drawTemp(int id, uint16_t color, int16_t x, int16_t y) {
  static char buf[10];
  size_t tempDevInd;
  float tempVal;
#ifdef WITH_TEMP
  DsTemp *dsTemp = DsTemp::getDsTemp();
  tempDevInd = dsTemp->selDevInd(id);
  if (tempDevInd < 100) {
    tempVal = dsTemp->devTemp(tempDevInd);
  }
#else
  tempDevInd = 100;
  tempVal = 22.3456;
#endif // WITH_TEMP
  
  mTft->setTextDatum(TR_DATUM);
  mTft->setTextColor(color, TFT_BLACK);
  if (tempDevInd <= 100) {
    snprintf(buf, sizeof(buf) - 1, "  %.2f", tempVal);
    mTft->drawString(buf, x, y, 6);
  } else {
    mTft->drawString((tempDevInd == TEMP_DEV_NO_INDEX ? "          -" : "         --"), x, y, 6);
  }
}

#endif // WITH_DISPLAY
