#ifndef _FS_SPIFFS_H_
#define _FS_SPIFFS_H_

#include "sysConfig.h"

#ifdef WITH_FS

#include "fsBase.h"

class FsSPIFFS : public FsBase {
public:
  FsSPIFFS(const char *a_fsName);
  virtual ~FsSPIFFS();

  virtual const char *getType() const { return m_fsType; }
  virtual uint64_t getCapacity() const;
  virtual uint64_t getSize() const;
  virtual uint64_t getUsed() const;

  virtual fs::FS *getFS() const;

private:
  const char *m_fsType;
};

#endif // WITH_FS

#endif // _FS_SPIFFS_H_
