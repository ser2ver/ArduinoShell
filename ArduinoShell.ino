#include "espShell.h"
#include "displayTft.h"

EspShell *sh;

#ifdef WITH_DISPLAY
DisplayTft *tft;
#endif // WITH_DISPLAY

void setup() {
  Serial.begin(115200);
  Serial.println("---------------------");
  Serial.println("File manager started!");

  sh = EspShell::getShell();

#ifdef WITH_DISPLAY
  tft = DisplayTft::getDisplay();
#endif // WITH_DISPLAY

//  sh->exec("h");
}

void loop() {
  const char *cmd = sh->readCommand(Serial);
  if (cmd && (cmd[0] != '\0')) {
    sh->exec(cmd);
  }

#ifdef WITH_DISPLAY
  tft->show();
#endif // WITH_DISPLAY

  delay(100);
}
