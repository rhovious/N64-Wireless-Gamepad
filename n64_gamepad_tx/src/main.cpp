#include <WiFi.h>
#include <AsyncElegantOTA.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <BleGamepad.h>

#include "secrets.h"
#include "config.h"
#include "espNowHelpers.h"
#include "controllerFunctions.h"
#include "webHelperFunctions.h"
#include "Arduino.h"

// ==========================================================================

extern float x_axis_web;
extern float y_axis_web;
extern int tx_MODE_web;
extern float buttonPressedWeb;

extern long x_axis_adjusted;
extern long y_axis_adjusted;
extern int tx_MODE;

extern data_structure_espNOW sending_data_espNow;

//=============================================================================

void printInfo()
{
    Serial.print("MODE: ");
    Serial.println(tx_MODE); // 0 = undefinder, 1 = esp-now, 2 = BLE
    Serial.print("X_axis: ");
    Serial.print(x_axis_adjusted, 0); // round off to the first dicimal place
    Serial.print("\n");
    Serial.print("Y-axis: "); // round off to the first dicimal place
    Serial.print(y_axis_adjusted, 0);
    Serial.print("\n");
    Serial.print("pressedButton: ");
    Serial.print(pressedButton);
    Serial.print("\n\n");
}

void setup()
{
    setPinModes();
    digitalWrite(ledPin, HIGH);
    setupWifi();
    setStickCalibration();
    Serial.begin(115200);

    setupWebUI();

    AsyncElegantOTA.begin(&server); // Start ElegantOTA
    server.begin();
    Serial.println("HTTP server started");

    setMode();

    digitalWrite(ledPin, LOW);
}

//  MAIN LOOP
void loop()
{
    updateWeb(); // updates webUI

    handleButtons(); // sets pressedButton to what is pressed
    handleSticks();  // sets sticks to value

    //printInfo(); // prints out to serial

    delay(100);
}