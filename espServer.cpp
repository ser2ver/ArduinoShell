#include "sysConfig.h"

#ifdef WITH_WIFI
#ifdef WITH_ESP_SERVER

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Middlewares.h>
#include "wifiCore.h"
#include "espServer.h"

#include "sysUtil.h"
#include "sysCore.h"
#include "sysRTC.h"
#include "wifiCore.h"
#include "dsTemp.h"

static const char notFoundContent[] PROGMEM = R"==(
<html>
<head>
  <title>Resource not found</title>
</head>
<body>
  <p>The resource was not found.</p>
  <p><a href="/">Start again</a></p>
</body>
)==";

class CounterMiddleware;

class EspServerImpl : public EspServer {
public:
  EspServerImpl();
  virtual ~EspServerImpl();

  virtual bool isStarted() { return mIsStarted; }
  virtual size_t countRequest() { return mCountRequest; }
  virtual void addRequest() { ++mCountRequest; }

  virtual void init();
  virtual void loop();

private:
  bool mIsStarted;
  size_t mCountRequest;

  WebServer *mServer;
  LoggingMiddleware *mLogger;
  CounterMiddleware *mCounter;

  void handleInfo();
  void handleJsonFull();
  void handleJsonShort();

  String getTemp(int id);
};

class CounterMiddleware : public Middleware {
public:
  void setServer(EspServerImpl *server) {
    mServer = server;
  }

  bool run(WebServer &server, Middleware::Callback next) override {
    if (mServer) {
      mServer->addRequest();
    }
    return next();
  }

private:
  EspServerImpl *mServer = NULL;
};

EspServerImpl *_espServerObj = NULL;

EspServer *EspServer::getServer() {
  if (!_espServerObj) {
    _espServerObj = new EspServerImpl();
  }
  return _espServerObj;
}

EspServerImpl::EspServerImpl() {
  mIsStarted = false;
  mCountRequest = 0;
  mServer = new WebServer(ESP_SERVER_PORT);
  mLogger = new LoggingMiddleware();
  mCounter = new CounterMiddleware();
}

EspServerImpl::~EspServerImpl() {
  mIsStarted = false;
  delete mServer;
  delete mLogger;
  delete mCounter;
}

void EspServerImpl::init() {
  WiFiCore *wifi = WiFiCore::getWiFi();
  if (!mIsStarted && (wifi->status() == WL_CONNECTED)) {
    mLogger->setOutput(Serial);
    mServer->addMiddleware(mLogger);

    mCounter->setServer(this);
    mServer->addMiddleware(mCounter);

    mServer->on("/", HTTP_GET, [this]() mutable {
      this->handleInfo();
    });
    mServer->on("/full.json", HTTP_GET, [this]() mutable {
      this->handleJsonFull();
    });
    mServer->on("/short.json", HTTP_GET, [this]() mutable {
      this->handleJsonShort();
    });

    mServer->onNotFound([this]() {
      this->mServer->send(404, "text/html", FPSTR(notFoundContent));
    });

    mServer->begin();

    mIsStarted = true;
  }
}

void EspServerImpl::loop() {
  if (!mIsStarted) {
    init();
  }
  if (mIsStarted) {
    mServer->handleClient();
  }
}

void EspServerImpl::handleInfo() {
  String result;
  result.reserve(2048);

  result += "<html><head>\n";
  result += "<title>Термостанция ESP-32</title>\n";
  result += "<meta http-equiv=\"Content-Type\" Content=\"text/html; Charset=utf-8\">\n";
  result += "<style>\n";
  result += "body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\n";
  result += "</style>\n";
  result += "</head><body>\n";
  result += "<h1>Привет от ESP-32!</h1>\n";
  result += "<table colspan=\"1\" cellpadding=\"1\" cellspacing=\"1\" border=\"1\">\n";

#ifdef WITH_SYSTEM
  SysCore *sys = SysCore::getSystem();

  result += "<tr><td>Arduino-ESP32</td><td><b>" + String(sys->getVersionArduinoEsp()) + "</b></td></tr>";
  result += "<tr><td>ESP-IDF</td><td><b>" + String(sys->getVersionEspIdf()) + "</b></td></tr>";
  result += "<tr><td>Chip</td><td><b>" + String(sys->getChipModel()) + "</b> rev <b>" + String(ESP.getChipRevision()) + "</b></td></tr>";
  result += "<tr><td>Chip cores</td><td><b>" + String(sys->getChipCores()) + "</b></td></tr>";
  result += "<tr><td>CPU freq</td><td><b>" + String(sys->getCpuFreqMHz()) + "</b></td></tr>";
  result += "<tr><td>Flash size</td><td><b>" + String(sys->getFlashChipSize()) + "</b></td></tr>";

  result += "<tr><td>Heap size</td><td><b>" + String(sys->getHeapSize()) + "</b> bytes</td></tr>";
  result += "<tr><td>Free heap</td><td><b>" + String(sys->getFreeHeap()) + "</b> bytes</td></tr>";
  result += "<tr><td>Min free heap</td><td><b>" + String(sys->getMinFreeHeap()) + "</b> bytes</td></tr>";
  result += "<tr><td>Max alloc</td><td><b>" + String(sys->getMaxAllocHeap()) + "</b> bytes</td></tr>";
  result += "<tr><td>Psram size</td><td><b>" + String(sys->getPsramSize()) + "</b> bytes</td></tr>";

  result += "<tr><td>Up time</td><td><b>" + String(sys->upTimeS()) + "</b> (<b>" + String(sys->upTimeF(), 3) + "</b> s)</td></tr>";
#endif // WITH_SYSTEM

#ifdef WITH_TIME
  static char strTime[21];
  struct tm *stm = NULL;
  SysRTC *sysTm = SysRTC::getSysRTC();
  stm = sysTm->getTimeInfo();

  if (stm) {
    strftime(strTime, 21, "%d.%m.%Y %H:%M:%S", stm);
    result += "<tr><td>Время</td><td><b>" + String(strTime) + "</b></td></tr>";
  }
#endif // WITH_TIME

  result += "<tr><td>Количество запросов</td><td><b>" + String(mCountRequest) + "</b></td></tr>";

#ifdef WITH_TEMP
  DsTemp *dsTemp = DsTemp::getDsTemp();
  dsTemp->scan();

  result += "<tr><td><b>" + getTemp(1) + "</b></td><td><b>" + getTemp(2) + "</b></td></tr>";
  result += "<tr><td><b>" + getTemp(3) + "</b></td><td><b>" + getTemp(4) + "</b></td></tr>";
  result += "<tr><td><b>" + getTemp(5) + "</b></td><td><b>" + getTemp(6) + "</b></td></tr>";
  result += "<tr><td><b>" + getTemp(7) + "</b></td><td></td></tr>";
#endif // WITH_TEMP

  result += "<tr><td><a href=\"/full.json\"><b>Полная информация</b></a></td><td><a href=\"/short.json\"><b>Данные температуры</b></a></td></tr>";
  result += "</table></body></html>";

  mServer->sendHeader("Cache-Control", "no-cache");
  mServer->send(200, "text/html; charset=utf-8", result);
}

void EspServerImpl::handleJsonFull() {
  String result;
  result.reserve(1024);

  result += "{\n";
  result += "  \"Chip Model\": " + String(ESP.getChipModel()) + ",\n";
  result += "  \"Chip Cores\": " + String(ESP.getChipCores()) + ",\n";
  result += "  \"Chip Revision\": " + String(ESP.getChipRevision()) + ",\n";
  result += "  \"flashSize\": " + String(ESP.getFlashChipSize()) + ",\n";
  result += "  \"freeHeap\": " + String(ESP.getFreeHeap()) + ",\n";

  result += "}";

  mServer->sendHeader("Cache-Control", "no-cache");
  mServer->send(200, "application/json; charset=utf-8", result);
}

void EspServerImpl::handleJsonShort() {
  String result;
  result.reserve(512);
  unsigned long ms = millis();

  result += "{\n";
#ifdef WITH_TIME
  static char strTime[21];
  struct tm *stm = NULL;
  SysRTC *sysTm = SysRTC::getSysRTC();
  stm = sysTm->getTimeInfo();

  if (stm) {
    strftime(strTime, 21, "%d.%m.%Y %H:%M:%S", stm);
    result += "  \"dateTime\": \"" + String(strTime) + "\",\n";
  }
#endif // WITH_TIME

#ifdef WITH_TEMP
  DsTemp *dsTemp = DsTemp::getDsTemp();
  dsTemp->scan();

  result += "  \"temp1\": " + getTemp(1) + ",\n";
  result += "  \"temp2\": " + getTemp(2) + ",\n";
  result += "  \"temp3\": " + getTemp(3) + ",\n";
  result += "  \"temp4\": " + getTemp(4) + ",\n";
  result += "  \"temp5\": " + getTemp(5) + ",\n";
  result += "  \"temp6\": " + getTemp(6) + ",\n";
  result += "  \"temp7\": " + getTemp(7) + ",\n";
#endif // WITH_TEMP

#ifdef WITH_SYSTEM
  SysCore *sys = SysCore::getSystem();

  result += "  \"heapSize\": " + String(sys->getHeapSize()) + ",\n";
  result += "  \"heapFree\": " + String(sys->getFreeHeap()) + ",\n";
  result += "  \"upTime\": " + String(sys->upTimeF(), 3) + ",\n";
  result += "  \"upTimeStr\": \"" + String(sys->upTimeS()) + "\",\n";
#endif // WITH_SYSTEM

  result += "  \"workTime\": " + String(millis() - ms) + "\n";
  result += "}";

  mServer->sendHeader("Cache-Control", "no-cache");
  mServer->send(200, "application/json; charset=utf-8", result);
}

String EspServerImpl::getTemp(int id) {
  String res;

#ifdef WITH_TEMP
  size_t tempDevInd;
  float tempVal;
  DsTemp *dsTemp = DsTemp::getDsTemp();

  tempDevInd = dsTemp->selDevInd(id);
  if (tempDevInd < 100) {
    tempVal = dsTemp->devTemp(tempDevInd);
  }
  
  if (tempDevInd <= 100) {
    res = String(tempVal, 2);
  } else {
    res = String(tempDevInd == TEMP_DEV_NO_INDEX ? "-100" : "-200");
  }
#endif // WITH_TEMP

  return res;
}

#endif // WITH_ESP_SERVER
#endif // WITH_WIFI
