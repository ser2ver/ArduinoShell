#ifndef _EXEC_BASE_H_
#define _EXEC_BASE_H_

#include "sysConfig.h"

#ifdef WITH_SHELL

class ExecBase {
public:
  ExecBase(const char *a_name) : m_name(a_name) {}
  virtual ~ExecBase() {}

  const char *getName() const { return m_name; }

  virtual void init() {}
  virtual void loop() {}
  virtual bool exec(const char *cmdName, const char *cmdArgs) { return false; }

  virtual void cmdHelp() {}
  virtual void cmdInfo() {}

private:
  const char *m_name;
};

#endif // WITH_SHELL

#endif // _EXEC_BASE_H_
