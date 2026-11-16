#include "sysConfig.h"

#ifdef WITH_SHELL

#include <Arduino.h>
#include "espShell.h"
#include "execLedRGB.h"
#include "execSys.h"
#include "execFS.h"
#include "execWiFi.h"
#include "execServer.h"
#include "execTime.h"
#include "execTemp.h"
#include "execDisplay.h"
#include "ledRGB.h"

struct ExecNode {
  ExecBase *worker;
  ExecNode *next;

  ExecNode(ExecBase *a_worker, ExecNode *a_next = NULL) : worker(a_worker), next(a_next) {}
};

class EspShellImpl : public EspShell {
public:
  EspShellImpl();
  virtual ~EspShellImpl();

  virtual void init();
  virtual void loop();
  virtual void addWorker(ExecBase *worker);
  virtual const char *readCommand(Stream &stin);
  virtual void execCommand(const char *cmdLine);
  virtual void exec(const char *cmdLine);

private:
  ExecNode *m_root;

  void cmdHelp();
  void cmdInfo();
};

EspShellImpl *_espShellObj = NULL;

EspShell *EspShell::getShell() {
  if (!_espShellObj) {
    _espShellObj = new EspShellImpl();
#ifdef WITH_LED_RGB
    LedRGB *led = LedRGB::getLedRGB();
    led->setRGB(1, 0, 0);
    _espShellObj->addWorker(new ExecLedRGB());
#endif // WITH_LED_RGB
#ifdef WITH_SYSTEM
    _espShellObj->addWorker(new ExecSys());
#endif // WITH_SYSTEM
#ifdef WITH_FS
    _espShellObj->addWorker(new ExecFS());
#endif // WITH_FS
#ifdef WITH_WIFI
    _espShellObj->addWorker(new ExecWiFi());
#ifdef WITH_ESP_SERVER
    _espShellObj->addWorker(new ExecServer());
#endif // WITH_ESP_SERVER
#endif // WITH_WIFI
#ifdef WITH_TIME
    _espShellObj->addWorker(new ExecTime());
#endif // WITH_TIME
#ifdef WITH_TEMP
    _espShellObj->addWorker(new ExecTemp());
#endif // WITH_TEMP
#ifdef WITH_DISPLAY
    _espShellObj->addWorker(new ExecDisplay());
#endif // WITH_DISPLAY

#ifdef WITH_LED_RGB
    led->setRGB(0, 0, 1);
#endif // WITH_LED_RGB
    _espShellObj->init();
#ifdef WITH_LED_RGB
    led->setRGB(0, 1, 0);
#endif // WITH_LED_RGB

    _espShellObj->execCommand("do start.cmd");
  }
  return _espShellObj;
}

EspShellImpl::EspShellImpl() : EspShell() {
  m_root = NULL;
}

EspShellImpl::~EspShellImpl() {
  ExecNode *t;
  while (m_root) {
    t = m_root;
    m_root = t->next;
    delete t->worker;
    delete t;
  }
}

void EspShellImpl::init() {
  ExecNode *t = m_root;
  while (t) {
    t->worker->init();
    t = t->next;
  }
}

void EspShellImpl::loop() {
  ExecNode *t = m_root;
  while (t) {
    t->worker->loop();
    t = t->next;
  }
}

void EspShellImpl::addWorker(ExecBase *worker) {
  if (m_root) {
    ExecNode *t = m_root;
    while (t->next) {
      t = t->next;
    }
    t->next = new ExecNode(worker);
  } else {
    m_root = new ExecNode(worker);
  }
}

const char *EspShellImpl::readCommand(Stream &stin) {
  static char cmdBuf[512];
  size_t bufLen = sizeof(cmdBuf) - 1;
  size_t pos;

  loop();

  for (pos=0; pos < bufLen; ++pos) {
    if (!stin.available()) {
      break;
    }
    cmdBuf[pos] = stin.read();
    if (cmdBuf[pos] == '\n') {
      break;
    }
  }
  cmdBuf[pos] = '\0';
  if (!stin.available() && (pos == 0)) {
    return NULL;
  }

  for ( ; pos > 0; --pos) {
    if (!isspace(cmdBuf[pos-1])) {
      break;
    }
    cmdBuf[pos-1] = '\0';
  }

  return cmdBuf;
}

void EspShellImpl::execCommand(const char *cmdLine) {
  size_t cmdMaxLen = 20;
  char cmdName[cmdMaxLen + 1];
  size_t cmdLen = strlen(cmdLine);
  size_t pos = 0, p = 0;

  for ( ; pos < cmdLen; ++pos) {
    if (!isspace(cmdLine[pos])) {
      break;
    }
  }
  for ( ; pos < cmdMaxLen; ++pos) {
    if (!cmdLine[pos] || isspace(cmdLine[pos])) {
      break;
    }
    cmdName[p++] = tolower(cmdLine[pos]);
  }
  cmdName[p] = '\0';
  for ( ; pos < cmdLen; ++pos) {
    if (!isspace(cmdLine[pos])) {
      break;
    }
  }
  if (pos > cmdLen) {
    pos = cmdLen;
  }

  if (!strcmp(cmdName, "?") || !strcmp(cmdName, "h") || !strcmp(cmdName, "help")) {
    cmdHelp();
  } else if (!strcmp(cmdName, "i") || !strcmp(cmdName, "info")) {
    cmdInfo();
  } else {
    ExecNode *t = m_root;
    while (t) {
      if (t->worker->exec(cmdName, cmdLine + pos)) {
        break;
      }
      t = t->next;
    }
    if (!t) {
      Serial.print("Unknown command: ");
      Serial.println(cmdName);
    }
  }
}

void EspShellImpl::exec(const char *cmdLine) {
  Serial.print("> ");
  Serial.println(cmdLine);

  if (!m_root) {
    Serial.println("The workers are not registered.");
    return;
  }

  execCommand(cmdLine);
}

void EspShellImpl::cmdHelp() {
  Serial.println("EspShell help:");
  Serial.println("  List commands for all workers:");
  Serial.println("    help - show this message (alias: h, ?)");
  Serial.println("    info - show info about devices (alias: i)");

  ExecNode *t = m_root;
  while (t) {
    t->worker->cmdHelp();
    t = t->next;
  }
}

void EspShellImpl::cmdInfo() {
  Serial.println("Show info by workers:");

  ExecNode *t = m_root;
  while (t) {
    t->worker->cmdInfo();
    t = t->next;
  }
}

#endif // WITH_SHELL
