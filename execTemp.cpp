#include "sysConfig.h"

#ifdef WITH_SHELL
#ifdef WITH_TEMP

#include "dsTemp.h"
#include "execTemp.h"
#include "espShell.h"
#include "sysUtil.h"

ExecTemp::ExecTemp() : ExecBase("DsTemp") {}

ExecTemp::~ExecTemp() {}

void ExecTemp::init() {
  EspShell *sh = EspShell::getShell();
  if (sh) {
    sh->execCommand("do temp.cmd");
  }
}

bool ExecTemp::exec(const char *cmdName, const char *cmdArgs) {
  if (!strcmp(cmdName, "temp")) {
    return cmdTemp(cmdArgs);
  } else if (!strcmp(cmdName, "set_res")) {
    return cmdSetRes(cmdArgs);
  } else if (!strcmp(cmdName, "set_sel")) {
    return cmdSetSel(cmdArgs);
  } else {
    return false;
  }
}

void ExecTemp::cmdHelp() {
  Serial.print("  List of ");
  Serial.print(getName());
  Serial.println(" commands:");
  Serial.println("    temp                - show temperature");
  Serial.println("    set_res <res>       - set temperature resolution to <res>");
  Serial.println("    set_sel <id> <addr> - set temperature device <addr> to selected list by <id>");
}

void ExecTemp::cmdInfo() {
  Serial.print("  ");
  Serial.print(getName());
  Serial.println(" info:");
  cmdTemp();
}

bool ExecTemp::cmdTemp(const char *cmdArgs) {
  const char *indent = cmdArgs ? "" : "    ";
  DsTemp *dsTemp = DsTemp::getDsTemp();

  size_t devCnt = dsTemp->scan();
  Serial.print(indent);
  Serial.print("Scan temperature, found ");
  Serial.print(devCnt);
  Serial.print(" devices, scanning time ");
  Serial.print(dsTemp->scanTime());
  Serial.println(" ms");

  if (devCnt > 0) {
    Serial.print(indent);
    Serial.println(" ind |       addr       | res |    C     |    F");
    for (size_t i=0; i < devCnt; ++i) {
      Serial.printf("%s %3d | %s | %3d | %8.4f | %8.4f\n", indent, i, dsTemp->devAddr(i), dsTemp->devResol(i), dsTemp->devTemp(i), dsTemp->devTempF(i));
    }
  } else {
    Serial.print(indent);
    Serial.println("Temperature measurement devices not found");
  }

  devCnt = dsTemp->selCount();
  if (devCnt > 0) {
    Serial.print(indent);
    Serial.println("List of selected devices:");

    size_t ind;
    Serial.print(indent);
    Serial.println(" ind | id |       addr       | res |    C     |    F");
    for (size_t i=0; i < devCnt; ++i) {
      ind = dsTemp->selDevInd(dsTemp->selId(i));
      Serial.printf("%s %3d | %2d | %s | ", indent, i, dsTemp->selId(i), dsTemp->selAddr(i));
      if (ind == TEMP_DEV_NO_INDEX) {
        Serial.println("The device is not connected");
      } else if (ind == TEMP_NO_DEV_INDEX) {
        Serial.println("The device is not selected");
      } else {
        Serial.printf("%3d | %8.4f | %8.4f\n", dsTemp->devResol(ind), dsTemp->devTemp(ind), dsTemp->devTempF(ind));
      }
    }
  } else {
    Serial.print(indent);
    Serial.println("No devices selected");
  }

  return true;
}

bool ExecTemp::cmdSetRes(const char *cmdArgs) {
  size_t pos = 0;
  size_t cmdLen = strlen(cmdArgs);

  uint8_t res = strToInt(cmdArgs, pos);
  if (res < 8) {
    Serial.println("Incorrect <usec> value");
  } else {
    Serial.print("Set temperature resolution to ");
    Serial.println(res);
    DsTemp *dsTemp = DsTemp::getDsTemp();
    dsTemp->setResol(res);
  }

  return true;
}

bool ExecTemp::cmdSetSel(const char *cmdArgs) {
  size_t pos = 0;
  size_t cmdLen = strlen(cmdArgs);

  int id = strToInt(cmdArgs, pos);
  if (id < 0 || id >= 100) {
    Serial.println("Incorrect <id> value");
    return true;
  }

  size_t sLen = strlen(cmdArgs);
  for ( ; pos < sLen; ++pos) {
    if (!isspace(cmdArgs[pos])) {
      break;
    }
  }

  DsTemp *dsTemp = DsTemp::getDsTemp();
  if (pos < sLen) {
    Serial.print("Set temperature device ");
    Serial.print(cmdArgs + pos);
    Serial.print(" to selected list by ");
    Serial.println(id);
    dsTemp->selSet(id, cmdArgs + pos);
  } else {
    Serial.print("Remove from selected list item ");
    Serial.println(id);
    dsTemp->selSet(id, NULL);
  }

  return true;
}

#endif // WITH_TEMP
#endif // WITH_SHELL
