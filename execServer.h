#ifndef _EXEC_SERVER_H_
#define _EXEC_SERVER_H_

#include "sysConfig.h"

#ifdef WITH_SHELL
#ifdef WITH_WIFI
#ifdef WITH_ESP_SERVER

#include "execBase.h"

class ExecServer : public ExecBase {
public:
  ExecServer();
  virtual ~ExecServer();

  virtual void init();
  virtual void loop();
  virtual bool exec(const char *cmdName, const char *cmdArgs);

  virtual void cmdHelp();
  virtual void cmdInfo();

private:
  virtual bool cmdServInfo(const char *cmdArgs = NULL);
};

#endif // WITH_ESP_SERVER
#endif // WITH_WIFI
#endif // WITH_SHELL

#endif // _EXEC_SERVER_H_
