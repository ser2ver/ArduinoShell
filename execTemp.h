#ifndef _EXEC_TEMP_H_
#define _EXEC_TEMP_H_

#include "sysConfig.h"

#ifdef WITH_SHELL
#ifdef WITH_TEMP

#include "execBase.h"

class ExecTemp : public ExecBase {
public:
  ExecTemp();
  virtual ~ExecTemp();

  virtual void init();
  virtual bool exec(const char *cmdName, const char *cmdArgs);

  virtual void cmdHelp();
  virtual void cmdInfo();

private:
  virtual bool cmdTemp(const char *cmdArgs = NULL);
  virtual bool cmdSetRes(const char *cmdArgs);
  virtual bool cmdSetSel(const char *cmdArgs);
};

#endif // WITH_TEMP
#endif // WITH_SHELL

#endif // _EXEC_TEMP_H_
