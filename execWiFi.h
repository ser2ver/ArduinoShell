#ifndef _EXEC_WIFI_H_
#define _EXEC_WIFI_H_

#include "sysConfig.h"

#ifdef WITH_SHELL
#ifdef WITH_WIFI

#include "execBase.h"

class ExecWiFi : public ExecBase {
public:
  ExecWiFi();
  virtual ~ExecWiFi();

  virtual void init();
  virtual bool exec(const char *cmdName, const char *cmdArgs);

  virtual void cmdHelp();
  virtual void cmdInfo();

private:
  virtual bool cmdScan(const char *cmdArgs);
  virtual bool cmdConnect(const char *cmdArgs);
  virtual bool cmdDisconnect(const char *cmdArgs);
  virtual bool cmdStatus(const char *cmdArgs = NULL);
};

#endif // WITH_WIFI
#endif // WITH_SHELL

#endif // _EXEC_WIFI_H_
