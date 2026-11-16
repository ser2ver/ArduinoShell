#include "sysConfig.h"

#ifdef WITH_SHELL
#ifdef WITH_TIME

#include "sysRTC.h"
#include "execTime.h"
#include "espShell.h"

ExecTime::ExecTime() : ExecBase("Time") {}

ExecTime::~ExecTime() {}

void ExecTime::init() {
  EspShell *sh = EspShell::getShell();
  if (sh) {
    sh->execCommand("do time.cmd");
  }
}

bool ExecTime::exec(const char *cmdName, const char *cmdArgs) {
  if (!strcmp(cmdName, "tinfo")) {
    return cmdTInfo(cmdArgs);
  } else if (!strcmp(cmdName, "date")) {
    return cmdDate(cmdArgs);
  } else if (!strcmp(cmdName, "time")) {
    return cmdTime(cmdArgs);
  } else if (!strcmp(cmdName, "set_tz")) {
    return cmdSetTz(cmdArgs);
  } else if (!strcmp(cmdName, "set_ntp1")) {
    return cmdSetNtp1(cmdArgs);
  } else if (!strcmp(cmdName, "set_ntp2")) {
    return cmdSetNtp2(cmdArgs);
  } else if (!strcmp(cmdName, "set_ntp3")) {
    return cmdSetNtp3(cmdArgs);
  } else {
    return false;
  }
}

void ExecTime::cmdHelp() {
  Serial.print("  List of ");
  Serial.print(getName());
  Serial.println(" commands:");
  Serial.println("    tinfo               - show time settings");
  Serial.println("    date                - show current date and time");
  Serial.println("    time                - show current time");
  Serial.println("    set_tz <tz_conf>    - set TZ config (examples: https://github.com/esp8266/Arduino/blob/master/cores/esp8266/TZ.h)");
  Serial.println("    set_ntp1 <ntp_host> - set address of ntp server 1");
  Serial.println("    set_ntp2 <ntp_host> - set address of ntp server 2");
  Serial.println("    set_ntp3 <ntp_host> - set address of ntp server 3");
}

void ExecTime::cmdInfo() {
  Serial.print("  ");
  Serial.print(getName());
  Serial.println(" info:");
  cmdTInfo();
}

bool ExecTime::cmdTInfo(const char *cmdArgs) {
  const char *indent = cmdArgs ? "" : "    ";
  SysRTC *sysTm = SysRTC::getSysRTC();

  Serial.print(indent);
  Serial.print("System time ");
  Serial.println(sysTm->isSync() ? "IS synchronized" : "IS NOT synchronized");

  Serial.print(indent);
  Serial.print("Time zone config: ");
  Serial.println(sysTm->getTz());

  Serial.print(indent);
  Serial.print("NTP server 1: ");
  Serial.println(sysTm->getNtp1());

  Serial.print(indent);
  Serial.print("NTP server 2: ");
  Serial.println(sysTm->getNtp2());

  Serial.print(indent);
  Serial.print("NTP server 3: ");
  Serial.println(sysTm->getNtp3());

  cmdDate(cmdArgs);
  return true;
}

bool ExecTime::cmdDate(const char *cmdArgs) {
  const char *indent = cmdArgs ? "" : "    ";
  SysRTC *sysTm = SysRTC::getSysRTC();
  struct tm *stm = sysTm->getTimeInfo();
  Serial.print(indent);
  Serial.print("Current date and time: ");
  if (stm) {
    Serial.println(sysTm->getTimeInfo(), "%Y-%m-%d %H:%M:%S");
  } else {
    Serial.println("NOT set");
  }
  return true;
}

bool ExecTime::cmdTime(const char *cmdArgs) {
  const char *indent = cmdArgs ? "" : "    ";
  SysRTC *sysTm = SysRTC::getSysRTC();
  struct tm *stm = sysTm->getTimeInfo();
  Serial.print(indent);
  Serial.print("Current time: ");
  if (stm) {
    Serial.println(sysTm->getTimeInfo(), "%H:%M:%S");
  } else {
    Serial.println("NOT set");
  }
  return true;
}

bool ExecTime::cmdSetTz(const char *cmdArgs) {
  SysRTC *sysTm = SysRTC::getSysRTC();
  sysTm->setTz(cmdArgs);
  sysTm->applyConfig();
  return true;
}

bool ExecTime::cmdSetNtp1(const char *cmdArgs) {
  SysRTC *sysTm = SysRTC::getSysRTC();
  sysTm->setNtp1(cmdArgs);
  sysTm->applyConfig();
  return true;
}

bool ExecTime::cmdSetNtp2(const char *cmdArgs) {
  SysRTC *sysTm = SysRTC::getSysRTC();
  sysTm->setNtp2(cmdArgs);
  sysTm->applyConfig();
  return true;
}

bool ExecTime::cmdSetNtp3(const char *cmdArgs) {
  SysRTC *sysTm = SysRTC::getSysRTC();
  sysTm->setNtp3(cmdArgs);
  sysTm->applyConfig();
  return true;
}

#endif // WITH_TIME
#endif // WITH_SHELL
