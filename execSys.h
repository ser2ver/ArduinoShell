#ifndef _EXEC_SYS_H_
#define _EXEC_SYS_H_

#include "sysConfig.h"

#ifdef WITH_SHELL
#ifdef WITH_SYSTEM

#include "execBase.h"

class ExecSys : public ExecBase {
public:
  ExecSys();
  virtual ~ExecSys();

  virtual bool exec(const char *cmdName, const char *cmdArgs);

  virtual void cmdHelp();
  virtual void cmdInfo();

private:
  virtual bool cmdHeap(const char *cmdArgs = NULL);
  virtual bool cmdUpTime(const char *cmdArgs = NULL);
  virtual bool cmdVersion(const char *cmdArgs = NULL);
  virtual bool cmdTasks(const char *cmdArgs);
  virtual bool cmdSleep(const char *cmdArgs);
  virtual bool cmdUSleep(const char *cmdArgs);
  virtual bool cmdReboot(const char *cmdArgs);
};

#endif // WITH_SYSTEM
#endif // WITH_SHELL

#endif // _EXEC_SYS_H_
