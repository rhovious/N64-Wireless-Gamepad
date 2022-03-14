#include "Arduino.h"
#include "config.h"
#include <BleGamepad.h>


BleGamepad bleGamepad(secret_BLEDeviceName, secret_BLEDeviceManufacturer, 100);


const int ledPin = 4; // the number of the LED pin

// Define which pins are used for each button
const int BTN_START_PIN = 36; // the number of the pushbutton pin
const int BTN_B_PIN = 14;     // the number of the pushbutton pin
const int BTN_A_PIN = 12;     // the number of the pushbutton pin

const int BTN_CUP_PIN = 19; // the number of the pushbutton pin
const int BTN_CL_PIN = 17;  // the number of the pushbutton pin
const int BTN_CR_PIN = 18;  // the number of the pushbutton pin
const int BTN_CD_PIN = 22;  // the number of the pushbutton pin

const int BTN_DUP_PIN = 25; // the number of the pushbutton pin
const int BTN_DL_PIN = 26;  // the number of the pushbutton pin
const int BTN_DR_PIN = 33;  // the number of the pushbutton pin
const int BTN_DD_PIN = 27;  // the number of the pushbutton pin

const int BTN_SR_PIN = 23; // the number of the pushbutton pin
const int BTN_SL_PIN = 32; // the number of the pushbutton pin

const int BTN_Z_PIN = 16; // the number of the pushbutton pin

// sticks
const int JOY_STICK_VRX = 39;
const int JOY_STICK_VRY = 34;

int previousStartState = HIGH;
int previousZState = HIGH;
int previousLState = HIGH;
int previousBState = HIGH;
int previousAState = HIGH;
int previousRState = HIGH;

int previousCDState = HIGH;
int previousCLState = HIGH;
int previousCUState = HIGH;
int previousCRState = HIGH;

int previousDPadUpState = HIGH;
int previousDPadLeftState = HIGH;
int previousDPadDownState = HIGH;
int previousDPadRightState = HIGH;

float x_axis_read;
float y_axis_read;
float x_axis;
float y_axis;

float x_axis_calibration_value;
float y_axis_calibration_value;

int connected = 0;
int tx_MODE = 0; // 0 = undefinder, 1 = esp-now, 2 = BLE

char pressedButton[3] = "0";

void handleSticks()
{
    int axis_max = 4095;
    x_axis_read = analogRead(JOY_STICK_VRX);
    y_axis_read = analogRead(JOY_STICK_VRY);

    // Convert joy stick value from -45 to 45
    x_axis = (x_axis_read - axis_max / 2) / 22.76;
    y_axis = (y_axis_read - axis_max / 2) / 22.76;

    if (tx_MODE == 2)
    {
        bleGamepad.setLeftThumb(x_axis, y_axis); // or bleGamepad.setX(32767); and bleGamepad.setY(32767);
        bleGamepad.sendReport();
        delay(10);
    }
}

void handleButtons()
{
    int currentStartState = digitalRead(BTN_START_PIN);
    int currentBState = digitalRead(BTN_B_PIN);
    int currentAState = digitalRead(BTN_A_PIN);
    int currentSRState = digitalRead(BTN_SR_PIN);
    int currentSLState = digitalRead(BTN_SL_PIN);
    int currentZState = digitalRead(BTN_Z_PIN);

    int currentCDState = digitalRead(BTN_CD_PIN);
    int currentCLState = digitalRead(BTN_CL_PIN);
    int currentCUPState = digitalRead(BTN_CUP_PIN);
    int currentCRState = digitalRead(BTN_CR_PIN);

    int currentDUPState = digitalRead(BTN_DUP_PIN);
    int currentDLState = digitalRead(BTN_DL_PIN);
    int currentDDState = digitalRead(BTN_DD_PIN);
    int currentDRState = digitalRead(BTN_DR_PIN);

    if (currentBState == HIGH)
    {
#ifdef DEBUG
        Serial.println("B");
#endif
        if (tx_MODE == 2)
        {
            bleGamepad.press(BUTTON_1);
            //bleGamepad.sendReport();
            //delay(100);
            //bleGamepad.sendReport();
        }
        strcpy(pressedButton, "B");
        digitalWrite(ledPin, HIGH);
    }

    else if (currentStartState == HIGH)
    {
#ifdef DEBUG
        Serial.println("START");
#endif
        if (tx_MODE == 2)
        {
            bleGamepad.press(BUTTON_2);
        }
        strcpy(pressedButton, "S");
        digitalWrite(ledPin, HIGH);
    }

    else if (currentAState == HIGH)
    {
#ifdef DEBUG
        Serial.println("A");
#endif
        if (tx_MODE == 2)
        {
            bleGamepad.press(BUTTON_3);
        }
        strcpy(pressedButton, "A");
        // turn LED on:
        digitalWrite(ledPin, HIGH);
    }

    else if (currentSRState == HIGH)
    {
#ifdef DEBUG
        Serial.println("SR");
#endif
        if (tx_MODE == 2)
        {
            bleGamepad.press(BUTTON_4);
        }
        strcpy(pressedButton, "SR");
        // turn LED on:
        digitalWrite(ledPin, HIGH);
    }

    else if (currentSLState == HIGH)
    {
#ifdef DEBUG
        Serial.println("SL");
#endif
        if (tx_MODE == 2)
        {
            bleGamepad.press(BUTTON_5);
        }
        strcpy(pressedButton, "SL");
        // turn LED on:
        digitalWrite(ledPin, HIGH);
    }

    else if (currentZState == HIGH)
    {
#ifdef DEBUG
        Serial.println("Z");
#endif
        if (tx_MODE == 2)
        {
            bleGamepad.press(BUTTON_6);
        }
        strcpy(pressedButton, "Z");
        // turn LED on:
        digitalWrite(ledPin, HIGH);
    }

    //~~~~~~~C PAD
    else if (currentCUPState == HIGH)
    {
#ifdef DEBUG
        Serial.println("CU");
#endif
        if (tx_MODE == 2)
        {
            bleGamepad.press(BUTTON_7);
        }
        strcpy(pressedButton, "CU");
        digitalWrite(ledPin, HIGH);
    }

    else if (currentCDState == HIGH)
    {
#ifdef DEBUG
        Serial.println("CD");
#endif
        if (tx_MODE == 2)
        {
            bleGamepad.press(BUTTON_8);
        }
        strcpy(pressedButton, "CD");
        digitalWrite(ledPin, HIGH);
    }

    else if (currentCLState == HIGH)
    {
#ifdef DEBUG
        Serial.println("CL");
#endif
        if (tx_MODE == 2)
        {
            bleGamepad.press(BUTTON_9);
        }
        strcpy(pressedButton, "CL");
        digitalWrite(ledPin, HIGH);
    }
    else if (currentCRState == HIGH)
    {
#ifdef DEBUG
        Serial.println("CR");
#endif
        if (tx_MODE == 2)
        {
            bleGamepad.press(BUTTON_10);
        }
        strcpy(pressedButton, "CR");
        digitalWrite(ledPin, HIGH);
    }

    //~~~~~~~D PAD
    else if (currentDUPState == HIGH)
    {
#ifdef DEBUG
        Serial.println("DU");
#endif
        if (tx_MODE == 2)
        {
            bleGamepad.press(BUTTON_11);
        }
        strcpy(pressedButton, "DU");
        digitalWrite(ledPin, HIGH);
    }

    else if (currentDDState == HIGH)
    {
#ifdef DEBUG
        Serial.println("DD");
#endif
        if (tx_MODE == 2)
        {
            bleGamepad.press(BUTTON_12);
        }
        strcpy(pressedButton, "DD");
        digitalWrite(ledPin, HIGH);
    }

    else if (currentDLState == HIGH)
    {
#ifdef DEBUG
        Serial.println("DL");
#endif
        if (tx_MODE == 2)
        {
            bleGamepad.press(BUTTON_13);
        }
        strcpy(pressedButton, "DL");
        digitalWrite(ledPin, HIGH);
    }
    else if (currentDRState == HIGH)
    {
#ifdef DEBUG
        Serial.println("DR");
#endif
        if (tx_MODE == 2)
        {
            bleGamepad.press(BUTTON_14);
        }
        strcpy(pressedButton, "DR");
        digitalWrite(ledPin, HIGH);
    }

    else
    {
        if (tx_MODE == 2)
        {

            bleGamepad.release(BUTTON_1);
            bleGamepad.release(BUTTON_2);
            bleGamepad.release(BUTTON_3);
            bleGamepad.release(BUTTON_4);
            bleGamepad.release(BUTTON_5);
            bleGamepad.release(BUTTON_6);
            bleGamepad.release(BUTTON_7);
            bleGamepad.release(BUTTON_8);
            bleGamepad.release(BUTTON_9);
            bleGamepad.release(BUTTON_10);
            bleGamepad.release(BUTTON_11);
            bleGamepad.release(BUTTON_12);
            bleGamepad.release(BUTTON_13);
            bleGamepad.release(BUTTON_14);
        }
        strcpy(pressedButton, "0");
        digitalWrite(ledPin, LOW); // turn LED off:
    }
}
void setPinModes()
{
    pinMode(ledPin, OUTPUT);

    pinMode(BTN_START_PIN, INPUT);
    pinMode(BTN_B_PIN, INPUT_PULLDOWN);
    pinMode(BTN_A_PIN, INPUT_PULLDOWN);

    pinMode(BTN_CUP_PIN, INPUT_PULLDOWN);
    pinMode(BTN_CL_PIN, INPUT_PULLDOWN);
    pinMode(BTN_CR_PIN, INPUT_PULLDOWN);
    pinMode(BTN_CD_PIN, INPUT_PULLDOWN);

    pinMode(BTN_DUP_PIN, INPUT_PULLDOWN);
    pinMode(BTN_DL_PIN, INPUT_PULLDOWN);
    pinMode(BTN_DR_PIN, INPUT_PULLDOWN);
    pinMode(BTN_DD_PIN, INPUT_PULLDOWN);

    pinMode(BTN_SR_PIN, INPUT_PULLDOWN);
    pinMode(BTN_SL_PIN, INPUT_PULLDOWN);
    pinMode(BTN_Z_PIN, INPUT_PULLDOWN);

    pinMode(JOY_STICK_VRX, INPUT);
    pinMode(JOY_STICK_VRY, INPUT);
}

void setStickCalibration()
{
    x_axis_read = analogRead(JOY_STICK_VRX);
    y_axis_read = analogRead(JOY_STICK_VRY);

    int axis_max = 4095;
    // Convert joy stick value from -45 to 45
    x_axis_calibration_value = (x_axis_read - axis_max / 2) / 22.76;
    y_axis_calibration_value = (y_axis_read - axis_max / 2) / 22.76;
#ifdef DEBUG
    Serial.print("X CALIBRATED VALUE: ");
    Serial.println(x_axis_calibration_value);
    Serial.print("Y CALIBRATED VALUE: ");
    Serial.println(y_axis_calibration_value);
#endif
}