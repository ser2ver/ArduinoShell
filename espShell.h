#ifndef _ESP_SHELL_H_
#define _ESP_SHELL_H_

#include "sysConfig.h"

#ifdef WITH_SHELL

#include "execBase.h"

class EspShell {
public:
  static EspShell *getShell();

  virtual ~EspShell() {}

  virtual const char *readCommand(Stream &stin) { return NULL; }
  virtual void execCommand(const char *cmdLine) {}
  virtual void exec(const char *cmdLine) {}

protected:
  EspShell() {}
};

#endif // WITH_SHELL

#endif // _ESP_SHELL_H_
