#include "sysConfig.h"

#ifdef WITH_SHELL
#ifdef WITH_LED_RGB

#include "ledRGB.h"
#include "execLedRGB.h"
#include "espShell.h"
#include "sysUtil.h"

ExecLedRGB::ExecLedRGB() : ExecBase("LedRGB") {}

ExecLedRGB::~ExecLedRGB() {}

void ExecLedRGB::loop() {
  LedRGB *led = LedRGB::getLedRGB();
  led->checkCycle();
}

bool ExecLedRGB::exec(const char *cmdName, const char *cmdArgs) {
  if (!strcmp(cmdName, "linfo")) {
    return cmdLedInfo(cmdArgs);
  } else if (!strcmp(cmdName, "lset")) {
    return cmdLedSet(cmdArgs);
  } else if (!strcmp(cmdName, "ladd")) {
    return cmdLedAdd(cmdArgs);
  } else if (!strcmp(cmdName, "lreset")) {
    return cmdLedReset(cmdArgs);
  } else {
    return false;
  }
}

void ExecLedRGB::cmdHelp() {
  Serial.print("  List of ");
  Serial.print(getName());
  Serial.println(" commands:");
  Serial.println("    linfo                      - show current Led status");
  Serial.println("    lset <hex_color>           - set Led color to <hex_color>");
  Serial.println("    ladd <ms_time> <hex_color> - add color <hex_color> to Led cycle for period of <ms_time>");
  Serial.println("    lreset                     - reset Led cycle");
}

void ExecLedRGB::cmdInfo() {
  Serial.print("  ");
  Serial.print(getName());
  Serial.println(" info:");
  cmdLedInfo();
}

bool ExecLedRGB::cmdLedInfo(const char *cmdArgs) {
  const char *indent = cmdArgs ? "" : "    ";
  LedRGB *led = LedRGB::getLedRGB();

  uint32_t color = led->getHex();
  char buf[8];
  hexToStr(color, buf, 0, 6);

  Serial.print(indent);
  Serial.print("Current Led color: ");
  Serial.println(buf);

  size_t cycleSize = led->cycleSize();
  if (!cycleSize) {
    Serial.print(indent);
    Serial.println("Led cycle not started");
  } else {
    Serial.print(indent);
    Serial.print("Started a ");
    Serial.print(cycleSize);
    Serial.println("-steps LED cycle:");

    for (size_t i=0; i < cycleSize; ++i) {
      Serial.print(indent);
      Serial.print(" ");
      Serial.print(i);
      Serial.print("\t| ");
      Serial.print(led->cycleTmSec(i));
      Serial.print("\t| ");
      hexToStr(led->cycleColor(i), buf, 0, 6);
      Serial.println(buf);
    }
  }

  return true;
}

bool ExecLedRGB::cmdLedSet(const char *cmdArgs) {
  size_t pos = 0;
  uint32_t color = strToHex(cmdArgs, pos);

  char buf[8];
  hexToStr(color, buf, 0, 6);
  Serial.print("Set Led color to: ");
  Serial.println(buf);

  LedRGB *led = LedRGB::getLedRGB();
  led->resetCycle();
  led->setHex(color);
  return true;
}

bool ExecLedRGB::cmdLedAdd(const char *cmdArgs) {
  size_t pos = 0;
  time_t tmSec = strToInt(cmdArgs, pos);
  uint32_t color = strToHex(cmdArgs, pos);

  char buf[8];
  hexToStr(color, buf, 0, 6);
  Serial.print("Add color ");
  Serial.print(buf);
  Serial.print(" to Led cycle for period of ");
  Serial.println(tmSec);

  LedRGB *led = LedRGB::getLedRGB();
  led->addCycle(tmSec, color);

  return true;
}

bool ExecLedRGB::cmdLedReset(const char *cmdArgs) {
  Serial.println("Reset Led cycle");
  LedRGB *led = LedRGB::getLedRGB();
  led->resetCycle();
  led->setHex(0);
  return true;
}

#endif // WITH_LED_RGB
#endif // WITH_SHELL
