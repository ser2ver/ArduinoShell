#ifndef _FS_BASE_H_
#define _FS_BASE_H_

#include "sysConfig.h"

#ifdef WITH_FS

#include "FS.h"

class FsBase {
public:
  FsBase(const char *a_fsName) : m_fsName(a_fsName), m_isEnabled(false) {}
  virtual ~FsBase() {}

  bool isEnabled() const { return m_isEnabled; }
  const char *getName() const { return m_fsName; }

  virtual const char *getType() const { return NULL; }
  virtual uint64_t getCapacity() const { return 0; }
  virtual uint64_t getSize() const { return 0; }
  virtual uint64_t getUsed() const { return 0; }

  virtual fs::FS *getFS() const { return NULL; }

private:
  const char *m_fsName;

protected:
  bool m_isEnabled;
};

#endif // WITH_FS

#endif // _FS_BASE_H_
