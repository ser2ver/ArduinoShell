#include "sysConfig.h"

#ifdef WITH_SHELL
#ifdef WITH_DISPLAY

#include "displayTft.h"
#include "execDisplay.h"
#include "sysUtil.h"

ExecDisplay::ExecDisplay() : ExecBase("Display") {}

ExecDisplay::~ExecDisplay() {}

bool ExecDisplay::exec(const char *cmdName, const char *cmdArgs) {
  if (!strcmp(cmdName, "dinfo")) {
    return cmdDInfo(cmdArgs);
  } else if (!strcmp(cmdName, "rotation")) {
    return cmdRotation(cmdArgs);
  } else if (!strcmp(cmdName, "colors")) {
    return cmdColors(cmdArgs);
  } else {
    return false;
  }
}

void ExecDisplay::cmdHelp() {
  Serial.print("  List of ");
  Serial.print(getName());
  Serial.println(" commands:");
  Serial.println("    dinfo         - show display info");
  Serial.println("    rotation <r>  - set display orientation to <r>");
  Serial.println("    colors <show> - display colors (1 - on, 0 - off)");
}

void ExecDisplay::cmdInfo() {
  Serial.print("  ");
  Serial.print(getName());
  Serial.println(" info:");
  cmdDInfo();
}

bool ExecDisplay::cmdDInfo(const char *cmdArgs) {
  const char *indent = cmdArgs ? "" : "    ";
  DisplayTft *tft = DisplayTft::getDisplay();

  Serial.print(indent);
  Serial.print("Display orientation ");
  Serial.print(tft->getRotation());
  Serial.print(", resolution: ");
  Serial.print(tft->sizeX());
  Serial.print("x");
  Serial.println(tft->sizeY());

  return true;
}

bool ExecDisplay::cmdRotation(const char *cmdArgs) {
  size_t pos = 0;
  size_t cmdLen = strlen(cmdArgs);

  long r = strToInt(cmdArgs, pos);
  if (r < 0) {
    Serial.println("Incorrect <r> value");
  } else {
    Serial.print("Set display rotation to ");
    Serial.println(r);
    DisplayTft *tft = DisplayTft::getDisplay();
    tft->setRotation((uint8_t) r);
  }

  return true;
}

bool ExecDisplay::cmdColors(const char *cmdArgs) {
  size_t pos = 0;
  size_t cmdLen = strlen(cmdArgs);

  long r = strToInt(cmdArgs, pos);
  bool show = !r ? false : true;
  Serial.print("Set show display to ");
  Serial.println(show);
  DisplayTft *tft = DisplayTft::getDisplay();
  tft->putColors(show);

  return true;
}

#endif // WITH_DISPLAY
#endif // WITH_SHELL
