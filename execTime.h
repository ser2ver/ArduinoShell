#ifndef _EXEC_TIME_H_
#define _EXEC_TIME_H_

#include "sysConfig.h"

#ifdef WITH_SHELL
#ifdef WITH_TIME

#include "execBase.h"

class ExecTime : public ExecBase {
public:
  ExecTime();
  virtual ~ExecTime();

  virtual void init();
  virtual bool exec(const char *cmdName, const char *cmdArgs);

  virtual void cmdHelp();
  virtual void cmdInfo();

private:
  virtual bool cmdTInfo(const char *cmdArgs = NULL);
  virtual bool cmdDate(const char *cmdArgs = NULL);
  virtual bool cmdTime(const char *cmdArgs = NULL);
  virtual bool cmdSetTz(const char *cmdArgs);
  virtual bool cmdSetNtp1(const char *cmdArgs);
  virtual bool cmdSetNtp2(const char *cmdArgs);
  virtual bool cmdSetNtp3(const char *cmdArgs);
};

#endif // WITH_TIME
#endif // WITH_SHELL

#endif // _EXEC_TIME_H_
