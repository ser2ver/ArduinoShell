#ifndef _EXEC_LED_RGB_H_
#define _EXEC_LED_RGB_H_

#include "sysConfig.h"

#ifdef WITH_SHELL
#ifdef WITH_LED_RGB

#include "execBase.h"

class ExecLedRGB : public ExecBase {
public:
  ExecLedRGB();
  virtual ~ExecLedRGB();

  virtual void loop();
  virtual bool exec(const char *cmdName, const char *cmdArgs);

  virtual void cmdHelp();
  virtual void cmdInfo();

private:
  virtual bool cmdLedInfo(const char *cmdArgs = NULL);
  virtual bool cmdLedSet(const char *cmdArgs);
  virtual bool cmdLedAdd(const char *cmdArgs);
  virtual bool cmdLedReset(const char *cmdArgs);
};

#endif // WITH_LED_RGB
#endif // WITH_SHELL

#endif // _EXEC_LED_RGB_H_
