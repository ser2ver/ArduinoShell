#ifndef _EXEC_DISPLAY_H_
#define _EXEC_DISPLAY_H_

#include "sysConfig.h"

#ifdef WITH_SHELL
#ifdef WITH_DISPLAY

#include "execBase.h"

class ExecDisplay : public ExecBase {
public:
  ExecDisplay();
  virtual ~ExecDisplay();

  virtual bool exec(const char *cmdName, const char *cmdArgs);

  virtual void cmdHelp();
  virtual void cmdInfo();

private:
  virtual bool cmdDInfo(const char *cmdArgs = NULL);
  virtual bool cmdRotation(const char *cmdArgs);
  virtual bool cmdColors(const char *cmdArgs);
};

#endif // WITH_DISPLAY
#endif // WITH_SHELL

#endif // _EXEC_DISPLAY_H_
