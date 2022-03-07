#include "Arduino.h"

String __version__ = "0.0.2";
String ARDUINO_MCU = "esp32dev";
String ARDUINO_BOARD = "esp32dev";

//~~~ FEATURE SETTINGS ~~~~~~~~~~~~~~~~~
#define DEBUG
#define WEBUI
//#define BLE_ENABLE
#define ESP_NOW_ENABLE