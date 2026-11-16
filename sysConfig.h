#ifndef _SYS_CONFIG_H_
#define _SYS_CONFIG_H_

#define WITH_SHELL 1

#define WITH_SYSTEM 1
#define WITH_TIME 1

#define WITH_FS 1
#define WITH_SD 1

#define WITH_WIFI 1

#define WITH_LED_RGB 1
#define LED_PIN_RED 4
#define LED_PIN_GREEN 16
#define LED_PIN_BLUE 17

#define WITH_TEMP 1
#define TEMP_PIN 21
#define TEMP_PERIOD 3000

#define TEMP_DEV_NO_INDEX 1000   // The device is not connected
#define TEMP_NO_DEV_INDEX 1001   // The device is not in the selected list

#define WITH_ESP_SERVER 1
#define ESP_SERVER_PORT 80

#define WITH_DISPLAY 1

#endif // _SYS_CONFIG_H_
