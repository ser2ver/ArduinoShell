#ifndef _FS_SD_H_
#define _FS_SD_H_

#include "sysConfig.h"

#ifdef WITH_FS
#ifdef WITH_SD

#include "fsBase.h"

class FsSD : public FsBase {
public:
  FsSD(const char *a_fsName);
  virtual ~FsSD();

  virtual const char *getType() const { return m_fsType; }
  virtual uint64_t getCapacity() const;
  virtual uint64_t getSize() const;
  virtual uint64_t getUsed() const;

  virtual fs::FS *getFS() const;

private:
  const char *m_fsType;
};

#endif // WITH_SD
#endif // WITH_FS

#endif // _FS_SD_H_
