// Complete Instructions to Get and Change ESP MAC Address: https://RandomNerdTutorials.com/get-change-esp32-esp8266-mac-address-arduino/
//use this on the  client - the device we are conencting to the ESP32 receiver.
//7C:87:CE:F6:DC:78 <---adafruit board esp32
//C8:C9:A3:F9:09:AC<---board used for testing controller (conencted to screen, jpystick, etc)



#include "WiFi.h"

void setup(){
  Serial.begin(115200);
  WiFi.mode(WIFI_MODE_STA);
  Serial.println(WiFi.macAddress());
}

void loop(){

}
