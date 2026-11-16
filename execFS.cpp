#include "sysConfig.h"

#ifdef WITH_SHELL
#ifdef WITH_FS

#include <Arduino.h>
#include "fsSPIFFS.h"
#include "fsSD.h"
#include "espShell.h"
#include "execFS.h"

struct FsPath {
  char dev;
  char path[256];
};

FsPath *parseFsPath(const char *cmd, size_t &pos) {
  FsPath *path = new FsPath();
  path->dev = 'a';
  size_t cmdLen = strlen(cmd);
  bool hasQuote = false;

  for ( ; pos < cmdLen; ++pos) {
    if (!isspace(cmd[pos])) {
      break;
    }
  }

  if (cmd[pos] == '\'') {
    hasQuote = true;
    ++pos;
  }

  if (cmd[pos + 1] == ':') {
    path->dev = tolower(cmd[pos]);
    pos += 2;
  }

  size_t p = 0;
  size_t pathLen = sizeof(path->path) - 1;
  if (cmd[pos] != '/') {
    path->path[0] = '/';
    ++p;
  }

  while ((pos < cmdLen) && (p < pathLen)) {
    if ((hasQuote && (cmd[pos] == '\'')) || (!hasQuote && isspace(cmd[pos]))) {
      if (hasQuote) {
        ++pos;
      }
      break;
    }
    path->path[p] = cmd[pos];
    ++pos;
    ++p;
  }
  path->path[p] = '\0';

  return path;
}

ExecFS::ExecFS() : ExecBase("FS") {
  m_fsA = new FsSPIFFS("A");
#ifdef WITH_SD
  m_fsC = new FsSD("C");
#endif // WITH_SD
}

ExecFS::~ExecFS() {
  delete m_fsA;
#ifdef WITH_SD
  delete m_fsC;
#endif // WITH_SD
}

bool ExecFS::exec(const char *cmdName, const char *cmdArgs) {
  if (!strcmp(cmdName, "do")) {
    return cmdDo(cmdArgs);
  } else if (!strcmp(cmdName, "df")) {
    return cmdDiskUsage(cmdArgs);
  } else if (!strcmp(cmdName, "ls")) {
    return cmdList(cmdArgs);
  } else if (!strcmp(cmdName, "cp")) {
    return cmdCopy(cmdArgs);
  } else if (!strcmp(cmdName, "cat")) {
    return cmdCat(cmdArgs);
  } else if (!strcmp(cmdName, "fout")) {
    return cmdFOut(cmdArgs);
  } else if (!strcmp(cmdName, "fadd")) {
    return cmdFAdd(cmdArgs);
  } else if (!strcmp(cmdName, "rm")) {
    return cmdRemove(cmdArgs);
  } else {
    return false;
  }
}

void ExecFS::cmdHelp() {
  Serial.print("  List of ");
  Serial.print(getName());
  Serial.println(" commands:");
  Serial.println("    do <file>         - execute commands from <file>");
  Serial.println("    df [<drive>]      - show used space on <drive> or on all drives");
  Serial.println("    ls [<path>]       - show files and directories in directory <path> (default 'a:/')");
  Serial.println("    cp <from> <to>    - copy file <from> to directopy <to>");
  Serial.println("    cat <file>        - show content of <file>");
  Serial.println("    fout <file> <str> - write <str> to <file>");
  Serial.println("    fadd <file> <str> - write <str> to end of <file>");
  Serial.println("    rm <file>         - remove <file>");
}

void ExecFS::cmdInfo() {
  Serial.print("  ");
  Serial.print(getName());
  Serial.println(" info:");
  showFsInfo(m_fsA, "    ");
#ifdef WITH_SD
  showFsInfo(m_fsC, "    ");
#endif // WITH_SD
}

bool ExecFS::cmdDo(const char *cmdArgs) {
  size_t pos = 0;
  FsPath *path = parseFsPath(cmdArgs, pos);
  FsBase *fs = (path->dev == 'a') ? m_fsA : NULL;
#ifdef WITH_SD
  fs = (path->dev == 'c') ? m_fsC : fs;
#endif // WITH_SD

  if (!fs) {
    Serial.print("Unknown drive '");
    Serial.print(path->dev);
    Serial.println("'");
    delete path;
    return true;
  }

  Serial.printf("Executing file: %s:%s\n", fs->getName(), path->path);
  File file = fs->getFS()->open(path->path);
  delete path;
  if (!file) {
    Serial.println("Failed to open file for reading");
    return true;
  }

  EspShell *sh = EspShell::getShell();
  const char *cmd = sh->readCommand(file);
  while (cmd) {
    if (cmd[0] != '\0') {
      sh->execCommand(cmd);
    }
    cmd = sh->readCommand(file);
  }
  file.close();

  return true;
}

bool ExecFS::cmdDiskUsage(const char *cmdArgs) {
  size_t pos = 0;
  size_t cmdLen = strlen(cmdArgs);

  for ( ; pos < cmdLen; ++pos) {
    if (!isspace(cmdArgs[pos])) {
      break;
    }
  }

  if ((pos != cmdLen) && (cmdArgs[pos + 1] == ':')) {
    FsBase *fs = (tolower(cmdArgs[pos]) == 'a') ? m_fsA : NULL;
#ifdef WITH_SD
    fs = (tolower(cmdArgs[pos]) == 'c') ? m_fsC : fs;
#endif // WITH_SD
    if (fs) {
      showFsInfo(fs, "");
    }
  } else {
    Serial.println("Disk usage by devices:");
    showFsInfo(m_fsA, "  ");
#ifdef WITH_SD
    showFsInfo(m_fsC, "  ");
#endif // WITH_SD
  }

  return true;
}

bool ExecFS::cmdList(const char *cmdArgs) {
  size_t pos = 0;
  FsPath *path = parseFsPath(cmdArgs, pos);
  FsBase *fs = (path->dev == 'a') ? m_fsA : NULL;
#ifdef WITH_SD
  fs = (path->dev == 'c') ? m_fsC : fs;
#endif // WITH_SD

  if (!fs) {
    Serial.print("Unknown drive '");
    Serial.print(path->dev);
    Serial.println("'");
    delete path;
    return true;
  }

  Serial.printf("Listing directory: %s:%s\n", fs->getName(), path->path);
  File root = fs->getFS()->open(path->path);
  delete path;
  if (!root) {
    Serial.println("Failed to open directory");
    return true;
  }
  if (!root.isDirectory()) {
    Serial.println("Not a directory");
    root.close();
    return true;
  }

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      Serial.print("  DIR : ");
      Serial.println(file.name());
    } else {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print(",  size: ");
      Serial.println(file.size());
    }
    file = root.openNextFile();
  }
  root.close();

  return true;
}

bool ExecFS::cmdCopy(const char *cmdArgs) {
  size_t pos = 0;
  FsPath *path = parseFsPath(cmdArgs, pos);
  FsBase *fs1 = (path->dev == 'a') ? m_fsA : NULL;
#ifdef WITH_SD
  fs1 = (path->dev == 'c') ? m_fsC : fs1;
#endif // WITH_SD

  if (!fs1) {
    Serial.print("Unknown drive in <from> '");
    Serial.print(path->dev);
    Serial.println("'");
    delete path;
    return true;
  }

  File file1 = fs1->getFS()->open(path->path);
  delete path;
  if (!file1) {
    Serial.println("Failed to open file for reading");
    return true;
  }

  path = parseFsPath(cmdArgs, pos);
  FsBase *fs2 = (path->dev == 'a') ? m_fsA : NULL;
#ifdef WITH_SD
  fs2 = (path->dev == 'c') ? m_fsC : fs2;
#endif // WITH_SD

  if (!fs2) {
    Serial.print("Unknown drive in <to> '");
    Serial.print(path->dev);
    Serial.println("'");
    delete path;
    file1.close();
    return true;
  }

  size_t p = strlen(path->path);
  if (path->path[p - 1] == '/') {
    const char* fname = file1.name();
    size_t pathLen = sizeof(path->path) - 1;
    size_t nameLen = strlen(fname);
    for (size_t i=0; (i < nameLen) && (p < pathLen); ++i) {
      path->path[p++] = fname[i];
    }
    path->path[p] = '\0';
  }

  Serial.printf("Copy file %s:%s to %s:%s\n", fs1->getName(), file1.path(), fs2->getName(), path->path);
  File file2 = fs2->getFS()->open(path->path, FILE_WRITE);
  delete path;
  if (!file2) {
    Serial.println("Failed to open file for writing");
    file1.close();
    return true;
  }

  while (file1.available()) {
    file2.write((char)file1.read());
  }

  file1.close();
  file2.close();

  return true;
}

bool ExecFS::cmdCat(const char *cmdArgs) {
  size_t pos = 0;
  FsPath *path = parseFsPath(cmdArgs, pos);
  FsBase *fs = (path->dev == 'a') ? m_fsA : NULL;
#ifdef WITH_SD
  fs = (path->dev == 'c') ? m_fsC : fs;
#endif // WITH_SD

  if (!fs) {
    Serial.print("Unknown drive '");
    Serial.print(path->dev);
    Serial.println("'");
    delete path;
    return true;
  }

  File file = fs->getFS()->open(path->path);
  Serial.printf("Reading file: %s:%s\n", fs->getName(), path->path);
  delete path;
  if (!file) {
    Serial.println("Failed to open file for reading");
    return true;
  }

  while (file.available()) {
    Serial.print((char)file.read());
  }
  file.close();

  return true;
}

bool ExecFS::cmdFOut(const char *cmdArgs) {
  size_t pos = 0;
  FsPath *path = parseFsPath(cmdArgs, pos);
  FsBase *fs = (path->dev == 'a') ? m_fsA : NULL;
#ifdef WITH_SD
  fs = (path->dev == 'c') ? m_fsC : fs;
#endif // WITH_SD

  if (!fs) {
    Serial.print("Unknown drive '");
    Serial.print(path->dev);
    Serial.println("'");
    delete path;
    return true;
  }

  Serial.printf("Writing to file: %s:%s\n", fs->getName(), path->path);
  File file = fs->getFS()->open(path->path, FILE_WRITE);
  delete path;
  if (!file) {
    Serial.println("Failed to open file for writing");
    return true;
  }

  size_t cmdLen = strlen(cmdArgs);
  for ( ; pos < cmdLen; ++pos) {
    if (!isspace(cmdArgs[pos])) {
      break;
    }
  }
  file.write((const uint8_t *)(cmdArgs + pos), cmdLen - pos);
  file.write('\n');
  file.close();

  return true;
}

bool ExecFS::cmdFAdd(const char *cmdArgs) {
  size_t pos = 0;
  FsPath *path = parseFsPath(cmdArgs, pos);
  FsBase *fs = (path->dev == 'a') ? m_fsA : NULL;
#ifdef WITH_SD
  fs = (path->dev == 'c') ? m_fsC : fs;
#endif // WITH_SD

  if (!fs) {
    Serial.print("Unknown drive '");
    Serial.print(path->dev);
    Serial.println("'");
    delete path;
    return true;
  }

  Serial.printf("Writing to end of file: %s:%s\n", fs->getName(), path->path);
  File file = fs->getFS()->open(path->path, FILE_APPEND);
  delete path;
  if (!file) {
    Serial.println("Failed to open file for append");
    return true;
  }

  size_t cmdLen = strlen(cmdArgs);
  for ( ; pos < cmdLen; ++pos) {
    if (!isspace(cmdArgs[pos])) {
      break;
    }
  }
  file.write((const uint8_t *)(cmdArgs + pos), cmdLen - pos);
  file.write('\n');
  file.close();

  return true;
}

bool ExecFS::cmdRemove(const char *cmdArgs) {
  size_t pos = 0;
  FsPath *path = parseFsPath(cmdArgs, pos);
  FsBase *fs = (path->dev == 'a') ? m_fsA : NULL;
#ifdef WITH_SD
  fs = (path->dev == 'c') ? m_fsC : fs;
#endif // WITH_SD

  if (!fs) {
    Serial.print("Unknown drive '");
    Serial.print(path->dev);
    Serial.println("'");
    delete path;
    return true;
  }

  Serial.printf("Remove file: %s:%s\n", fs->getName(), path->path);
  fs->getFS()->remove(path->path);
  delete path;

  return true;
}

void ExecFS::showFsInfo(FsBase *fs, const char *indent) {
  Serial.print(indent);
  Serial.print("Drive '");
  Serial.print(fs->getName());
  Serial.print("' type '");
  Serial.print(fs->getType());
  Serial.print("' is ");
  Serial.print(fs->isEnabled() ? "ON" : "OFF");
  Serial.println(", info:");

  if (fs->isEnabled()) {
    Serial.print(indent);
    Serial.print("  Capacity: ");
    Serial.print(fs->getCapacity() / (1024 * 1024));
    Serial.println("MB");

    Serial.print(indent);
    Serial.print("  Total space: ");
    Serial.println(fs->getSize());
    Serial.print(indent);
    Serial.print("  Used space: ");
    Serial.println(fs->getUsed());
  }
}

#endif // WITH_FS
#endif // WITH_SHELL
