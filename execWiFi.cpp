#include "sysConfig.h"

#ifdef WITH_SHELL
#ifdef WITH_WIFI

#include <Arduino.h>
#include <WiFi.h>
#include "sysUtil.h"
#include "execWiFi.h"
#include "wifiCore.h"
#include "espShell.h"

ExecWiFi::ExecWiFi() : ExecBase("WiFi") {}

ExecWiFi::~ExecWiFi() {}

void ExecWiFi::init() {
  EspShell *sh = EspShell::getShell();
  if (sh) {
    sh->execCommand("do wifi.cmd");
  }
}

bool ExecWiFi::exec(const char *cmdName, const char *cmdArgs) {
  if (!strcmp(cmdName, "wscan")) {
    return cmdScan(cmdArgs);
  } else if (!strcmp(cmdName, "wconn")) {
    return cmdConnect(cmdArgs);
  } else if (!strcmp(cmdName, "wdisconn")) {
    return cmdDisconnect(cmdArgs);
  } else if (!strcmp(cmdName, "wstat")) {
    return cmdStatus(cmdArgs);
  } else {
    return false;
  }
}

void ExecWiFi::cmdHelp() {
  Serial.print("  List of ");
  Serial.print(getName());
  Serial.println(" commands:");
  Serial.println("    wscan               - scan and list nearby access points");
  Serial.println("    wconn <ssid> <pass> - connect to wifi access point");
  Serial.println("    wdisconn            - disconnect wifi");
  Serial.println("    wstat               - show wifi connection status");
}

void ExecWiFi::cmdInfo() {
  Serial.print("  ");
  Serial.print(getName());
  Serial.println(" info:");
  cmdStatus();
}

bool ExecWiFi::cmdScan(const char *cmdArgs) {
  Serial.println("Scanning WiFi access points:");
  WiFiCore *wifi = WiFiCore::getWiFi();
  WiFiScan scanRes = wifi->scan();
  int n = scanRes.getNumItems();

  if (n == 0) {
    Serial.println("  No networks found.");
    return true;;
  }

  Serial.println("  #  |               SSID               |       BSSID       | RSSI | Ch |   Encryption");
  Serial.println("-----+----------------------------------+-------------------+------+----+----------------");

  for (int i = 0; i < n; i++) {
    Serial.printf("%4d | %-32s | %-11s | %4d |%3d | %s\n", i + 1, scanRes.ssid(i).c_str(), scanRes.bssid(i).c_str(), scanRes.rssi(i), scanRes.channel(i), scanRes.encryptionStr(i));
  }
  return true;
}

bool ExecWiFi::cmdConnect(const char *cmdArgs) {
  size_t lenVal = 64;
  char ssid[lenVal + 1];
  char pass[lenVal + 1];
  size_t lenCmd = strlen(cmdArgs);
  size_t pos = 0, p;

  for ( ; pos < lenCmd; ++pos) {
    if (!isspace(cmdArgs[pos])) {
      break;
    }
  }

  for (p = 0; p < lenVal && pos < lenCmd; ++p, ++pos) {
    if (isspace(cmdArgs[pos])) {
      break;
    }
    ssid[p] = cmdArgs[pos];
  }
  ssid[p] = '\0';

  for ( ; pos < lenCmd; ++pos) {
    if (!isspace(cmdArgs[pos])) {
      break;
    }
  }

  for (p = 0; p < lenVal && pos < lenCmd; ++p, ++pos) {
    if (isspace(cmdArgs[pos])) {
      break;
    }
    pass[p] = cmdArgs[pos];
  }
  pass[p] = '\0';

  Serial.print("WiFi connecting to ");
  Serial.println(ssid);

  WiFiCore *wifi = WiFiCore::getWiFi();
  wifi->connect(ssid, pass);

  return true;
}

bool ExecWiFi::cmdDisconnect(const char *cmdArgs) {
  WiFiCore *wifi = WiFiCore::getWiFi();

  Serial.println("Disconnecting WiFi");
  wifi->disconnect();

  return true;
}

bool ExecWiFi::cmdStatus(const char *cmdArgs) {
  const char *indent = cmdArgs ? "" : "    ";
  WiFiCore *wifi = WiFiCore::getWiFi();

  Serial.print(indent);
  Serial.println("Show WiFi connection info:");

  Serial.print(indent);
  Serial.print("  Status     : ");
  Serial.println(wifi->statusStr());

  Serial.print(indent);
  Serial.print("  MAC Address: ");
  Serial.println(wifi->macAddress());

  if (wifi->status() == WL_CONNECTED) {
    Serial.printf("%s  SSID       : %s\n", indent, wifi->ssid().c_str());
    Serial.printf("%s  BSSID      : %s\n", indent, wifi->bssid().c_str());
    Serial.printf("%s  RSSI       : %d dBm\n", indent, wifi->rssi());
    Serial.printf("%s  IP         : %s\n", indent, wifi->localIP().c_str());
    Serial.printf("%s  Gateway    : %s\n", indent, wifi->gatewayIP().c_str());
    Serial.printf("%s  DNS        : %s\n", indent, wifi->dnsIP().c_str());
    Serial.printf("%s  Subnet     : %s\n", indent, wifi->subnetMask().c_str());
  }

  return true;
}

#endif // WITH_WIFI
#endif // WITH_SHELL
