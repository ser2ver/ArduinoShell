#ifndef _EXEC_FS_H_
#define _EXEC_FS_H_

#include "sysConfig.h"

#ifdef WITH_SHELL
#ifdef WITH_FS

#include "execBase.h"
#include "fsBase.h"

class ExecFS : public ExecBase {
public:
  ExecFS();
  virtual ~ExecFS();

  virtual bool exec(const char *cmdName, const char *cmdArgs);

  virtual void cmdHelp();
  virtual void cmdInfo();

private:
  FsBase *m_fsA;
#ifdef WITH_SD
  FsBase *m_fsC;
#endif // WITH_SD

  virtual bool cmdDo(const char *cmdArgs);
  virtual bool cmdDiskUsage(const char *cmdArgs);
  virtual bool cmdList(const char *cmdArgs);
  virtual bool cmdCopy(const char *cmdArgs);
  virtual bool cmdCat(const char *cmdArgs);
  virtual bool cmdFOut(const char *cmdArgs);
  virtual bool cmdFAdd(const char *cmdArgs);
  virtual bool cmdRemove(const char *cmdArgs);

  void showFsInfo(FsBase *fs, const char *indent);
};

#endif // WITH_FS
#endif // WITH_SHELL

#endif // _EXEC_FS_H_
