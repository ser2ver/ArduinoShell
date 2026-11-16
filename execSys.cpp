#include "sysConfig.h"

#ifdef WITH_SHELL
#ifdef WITH_SYSTEM

#include <Arduino.h>
#include "sysCore.h"
#include "execSys.h"
#include "sysUtil.h"

ExecSys::ExecSys() : ExecBase("System") {}

ExecSys::~ExecSys() {}

bool ExecSys::exec(const char *cmdName, const char *cmdArgs) {
  if (!strcmp(cmdName, "heap")) {
    return cmdHeap(cmdArgs);
  } else if (!strcmp(cmdName, "uptime")) {
    return cmdUpTime(cmdArgs);
  } else if (!strcmp(cmdName, "version")) {
    return cmdVersion(cmdArgs);
  } else if (!strcmp(cmdName, "ps")) {
    return cmdTasks(cmdArgs);
  } else if (!strcmp(cmdName, "sleep")) {
    return cmdSleep(cmdArgs);
  } else if (!strcmp(cmdName, "usleep")) {
    return cmdUSleep(cmdArgs);
  } else if (!strcmp(cmdName, "reboot")) {
    return cmdReboot(cmdArgs);
  } else {
    return false;
  }
}

void ExecSys::cmdHelp() {
  Serial.print("  List of ");
  Serial.print(getName());
  Serial.println(" commands:");
  Serial.println("    heap          - show heap memory statistics");
  Serial.println("    uptime        - show time since last reboot");
  Serial.println("    version       - show system component versions");
  Serial.println("    ps            - list FreeRTOS tasks");
  Serial.println("    sleep <sec>   - wait <sec> seconds");
  Serial.println("    usleep <usec> - wait <usec> miliseconds");
  Serial.println("    reboot        - system reboot on the controller");
}

void ExecSys::cmdInfo() {
  Serial.print("  ");
  Serial.print(getName());
  Serial.println(" info:");
  cmdVersion();
  cmdHeap();
  cmdUpTime();
}

bool ExecSys::cmdHeap(const char *cmdArgs) {
  const char *indent = cmdArgs ? "" : "    ";
  SysCore *sys = SysCore::getSystem();

  Serial.print(indent);
  Serial.println("Show heap usage:");
  Serial.printf("%s  Heap size    : %7lu bytes\n", indent, sys->getHeapSize());
  Serial.printf("%s  Free heap    : %7lu bytes\n", indent, sys->getFreeHeap());
  Serial.printf("%s  Min free heap: %7lu bytes\n", indent, sys->getMinFreeHeap());
  Serial.printf("%s  Max alloc    : %7lu bytes\n", indent, sys->getMaxAllocHeap());
  Serial.printf("%s  Psram size   : %7lu bytes\n", indent, sys->getPsramSize());

  return true;
}

bool ExecSys::cmdUpTime(const char *cmdArgs) {
  const char *indent = cmdArgs ? "" : "    ";
  SysCore *sys = SysCore::getSystem();

  Serial.print(indent);
  Serial.println("Show time since last reboot:");
  Serial.printf("%s  %s (%.3f s)\n", indent, sys->upTimeS(), sys->upTimeF());

  return true;
}

bool ExecSys::cmdVersion(const char *cmdArgs) {
  const char *indent = cmdArgs ? "" : "    ";
  SysCore *sys = SysCore::getSystem();

  Serial.print(indent);
  Serial.println("Show system component versions:");
  Serial.printf("%s  Arduino-ESP32: %s\n", indent, sys->getVersionArduinoEsp());
  Serial.printf("%s  ESP-IDF      : %s\n", indent, sys->getVersionEspIdf());
  Serial.printf("%s  Chip         : %s rev %d\n", indent, sys->getChipModel(), sys->getChipRevision());
  Serial.printf("%s  Chip cores   : %lu\n", indent, sys->getChipCores());
  Serial.printf("%s  CPU freq     : %lu MHz\n", indent, sys->getCpuFreqMHz());
  Serial.printf("%s  Flash size   : %lu bytes\n", indent, sys->getFlashChipSize());

  return true;
}

bool ExecSys::cmdTasks(const char *cmdArgs) {
  uint32_t nTasks = uxTaskGetNumberOfTasks();
  TaskStatus_t *pxTaskStatusArray = (TaskStatus_t *)malloc(nTasks * sizeof(TaskStatus_t));
  if (!pxTaskStatusArray) {
    Serial.println("Out of memory\n");
    return true;
  }

  uint32_t ulTotalRunTime = 0;
  nTasks = uxTaskGetSystemState(pxTaskStatusArray, nTasks, &ulTotalRunTime);

  Serial.printf("%-20s %8s %5s %5s %4s\n", "Name", "State", "Prio", "Stack", "Core");
  Serial.printf("%-20s %8s %5s %5s %4s\n", "--------------------", "--------", "-----", "-----", "----");

  static const char *stateNames[] = {"Running", "Ready", "Blocked", "Suspend", "Deleted", "Invalid"};

  for (uint32_t i = 0; i < nTasks; i++) {
    TaskStatus_t *t = &pxTaskStatusArray[i];
    int state = (int)t->eCurrentState;
    if (state > 5) {
      state = 5;
    }
    int core = (int)t->xCoreID;
    char coreStr[4];
    if (core == tskNO_AFFINITY) {
      snprintf(coreStr, sizeof(coreStr), "any");
    } else {
      snprintf(coreStr, sizeof(coreStr), "%" PRIu8, (uint8_t)core);
    }
    Serial.printf("%-20s %8s %5lu %5lu %4s\n", t->pcTaskName, stateNames[state], (unsigned long)t->uxCurrentPriority, (unsigned long)t->usStackHighWaterMark, coreStr);
  }

  free(pxTaskStatusArray);

  return true;
}

bool ExecSys::cmdSleep(const char *cmdArgs) {
  size_t pos = 0;
  size_t cmdLen = strlen(cmdArgs);

  long sec = strToInt(cmdArgs, pos);
  if (sec <= 0) {
    Serial.println("Incorrect <sec> value");
  } else {
    sleep(sec);
  }

  return true;
}

bool ExecSys::cmdUSleep(const char *cmdArgs) {
  size_t pos = 0;
  size_t cmdLen = strlen(cmdArgs);

  long usec = strToInt(cmdArgs, pos);
  if (usec <= 0) {
    Serial.println("Incorrect <usec> value");
  } else {
    delay(usec);
  }

  return true;
}

bool ExecSys::cmdReboot(const char *cmdArgs) {
  SysCore *sys = SysCore::getSystem();

  Serial.println("Restarting in 1 second...");
  sleep(1);
  sys->reboot();

  return true;
}

#endif // WITH_SYSTEM
#endif // WITH_SHELL
