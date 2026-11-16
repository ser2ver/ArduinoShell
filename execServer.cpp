#include "sysConfig.h"

#ifdef WITH_SHELL
#ifdef WITH_WIFI
#ifdef WITH_ESP_SERVER

#include <Arduino.h>
#include "espServer.h"
#include "execServer.h"
#include "sysUtil.h"

ExecServer::ExecServer() : ExecBase("Server") {}

ExecServer::~ExecServer() {}

void ExecServer::init() {
  EspServer *serv = EspServer::getServer();
  serv->init();
}

void ExecServer::loop() {
  EspServer *serv = EspServer::getServer();
  serv->loop();
}

bool ExecServer::exec(const char *cmdName, const char *cmdArgs) {
  if (!strcmp(cmdName, "servinfo")) {
    return cmdServInfo(cmdArgs);
  } else {
    return false;
  }
}

void ExecServer::cmdHelp() {
  Serial.print("  List of ");
  Serial.print(getName());
  Serial.println(" commands:");
  Serial.println("    servinfo - show web server statistics");
}

void ExecServer::cmdInfo() {
  Serial.print("  ");
  Serial.print(getName());
  Serial.println(" info:");
  cmdServInfo();
}

bool ExecServer::cmdServInfo(const char *cmdArgs) {
  const char *indent = cmdArgs ? "" : "    ";
  EspServer *serv = EspServer::getServer();

  Serial.print(indent);
  Serial.println("Show web server statistics:");
  Serial.printf("%s  Is started     : %s\n", indent, serv->isStarted() ? "ON" : "OFF");
  Serial.printf("%s  Request counter: %lu\n", indent, serv->countRequest());

  return true;
}

#endif // WITH_ESP_SERVER
#endif // WITH_WIFI
#endif // WITH_SHELL
