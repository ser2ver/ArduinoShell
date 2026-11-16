#ifndef _ESP_SERVER_H_
#define _ESP_SERVER_H_

#include "sysConfig.h"

#ifdef WITH_WIFI
#ifdef WITH_ESP_SERVER

class EspServer {
public:
  static EspServer *getServer();

  virtual ~EspServer() {}

  virtual bool isStarted() { return false; }
  virtual size_t countRequest() { return 0; }

  virtual void init() {}
  virtual void loop() {}

protected:
  EspServer() {}
};

#endif // WITH_ESP_SERVER
#endif // WITH_WIFI

#endif // _ESP_SERVER_H_
