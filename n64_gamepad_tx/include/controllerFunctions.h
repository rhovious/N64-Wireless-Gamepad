#include "Arduino.h"
#include <BleGamepad.h>

BleGamepad bleGamepad(secret_BLEDeviceName, secret_BLEDeviceManufacturer, 100);

typedef struct data_structure_espNOW
{
    int x;
    int y;
    char buttonCMD[3];
} data_structure_espNOW;

data_structure_espNOW sending_data_espNow;

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

long x_axis_adjusted;
long y_axis_adjusted;
extern int x_raw_min;
extern int x_raw_max;
extern int y_raw_min;
extern int y_raw_max;

extern int x_deadzone_high;
extern int x_deadzone_low;
extern int y_deadzone_high;
extern int y_deadzone_low;

float x_axis_calibration_value;
float y_axis_calibration_value;

int connected = 0;
int tx_MODE = 0; // 0 = undefinder, 1 = esp-now, 2 = BLE

char pressedButton[3] = "0";

void setMode()
{
    int currentBState = digitalRead(BTN_B_PIN);
    int currentAState = digitalRead(BTN_A_PIN);

    if (currentAState == HIGH && currentBState == LOW) // SETS TO ESP-NOW MODE
    {
        tx_MODE = 1;
    }
    else if (currentBState == HIGH && currentAState == LOW)
    {
        tx_MODE = 2; // SETS TO BLE MODE
    }
    else // NO TRANSMIT
    {
        tx_MODE = 1; //FIX THIS
    }

    if (tx_MODE == 2)
    {
        bleGamepad.begin(14, 0, true, true, false, false, false, false, false, false, false, false, false, false, false);
        // The default bleGamepad.begin() above is the same as bleGamepad.begin(16, 1, true, true, true, true, true, true, true, true, false, false, false, false, false);
        // which enables a gamepad with 16 buttons, 1 hat switch, enabled x, y, z, rZ, rX, rY, slider 1, slider 2 and disabled rudder, throttle, accelerator, brake, steering
        // Auto reporting is enabled by default.
        // Use bleGamepad.setAutoReport(false); to disable auto reporting, and then use bleGamepad.sendReport(); as needed
        Serial.println("Starting BLE work!");
        digitalWrite(ledPin, LOW);
        delay(100);
        digitalWrite(ledPin, HIGH);
        delay(500);
        digitalWrite(ledPin, LOW);
        delay(100);
        digitalWrite(ledPin, HIGH);
        delay(500);
        digitalWrite(ledPin, LOW);
    }

    if (tx_MODE == 1)
    {
        setupESPNOW();
        Serial.println("Starting ESP NOW");
        digitalWrite(ledPin, LOW);
        delay(100);
        digitalWrite(ledPin, HIGH);
        delay(500);
        digitalWrite(ledPin, LOW);
    }
}

void handleSticks()
{
    long x_axis_raw = analogRead(JOY_STICK_VRX);
    long y_axis_raw = analogRead(JOY_STICK_VRY);

    // map(value, fromLow, fromHigh, toLow, toHigh)
    float x_axis_mapped = map(x_axis_raw, x_raw_min, x_raw_max, -32767, 32767);
    float y_axis_mapped = map(y_axis_raw, y_raw_min, y_raw_max, -32767, 32767);

    //~~sets as raw readings
    // x_axis_adjusted = x_axis_raw;
    // y_axis_adjusted = y_axis_raw;

    //~~maps based on calibration
    y_axis_adjusted = y_axis_mapped - y_axis_calibration_value;
    x_axis_adjusted = x_axis_mapped - x_axis_calibration_value;

    //~~adjusts based on deadzones

    if (x_axis_mapped > x_deadzone_low && x_axis_mapped < x_deadzone_high)
    {
        x_axis_adjusted = 0;
    }
    else
    {
        x_axis_adjusted = x_axis_adjusted;
    }

    if (y_axis_mapped > y_deadzone_low && y_axis_mapped < y_deadzone_high)
    {
        y_axis_adjusted = 0;
    }
    else
    {
        y_axis_adjusted = y_axis_adjusted;
    }

    if (tx_MODE == 1) // ESP-NOW
    {
        sending_data_espNow.x = x_axis_adjusted;
        sending_data_espNow.y = y_axis_adjusted;
        strcpy(sending_data_espNow.buttonCMD, pressedButton);
        esp_err_t result =
            esp_now_send(0, (uint8_t *)&sending_data_espNow, sizeof(data_structure_espNOW));

        if (result == ESP_OK)
        {
            Serial.println("Sent with success");
            connected = 1;
        }
        else
        {
            Serial.println("Error sending the data");
            connected = 0;
        }
    }

    if (tx_MODE == 2)
    {
        bleGamepad.setLeftThumb(x_axis_adjusted, y_axis_adjusted); // or bleGamepad.setX(32767); and bleGamepad.setY(32767);
        // bleGamepad.sendReport();
        // delay(10);
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
        Serial.println("B");
        if (tx_MODE == 2)
        {
            bleGamepad.press(BUTTON_1);
        }
        strcpy(pressedButton, "B");
        digitalWrite(ledPin, HIGH);
    }

    else if (currentStartState == HIGH)
    {
        Serial.println("START");
        if (tx_MODE == 2)
        {
            bleGamepad.press(BUTTON_2);
        }
        strcpy(pressedButton, "S");
        digitalWrite(ledPin, HIGH);
    }

    else if (currentAState == HIGH)
    {
        Serial.println("A");
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
        Serial.println("SR");
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
        Serial.println("SL");
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
        Serial.println("Z");
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
        Serial.println("CU");
        if (tx_MODE == 2)
        {
            bleGamepad.press(BUTTON_7);
        }
        strcpy(pressedButton, "CU");
        digitalWrite(ledPin, HIGH);
    }

    else if (currentCDState == HIGH)
    {
        Serial.println("CD");
        if (tx_MODE == 2)
        {
            bleGamepad.press(BUTTON_8);
        }
        strcpy(pressedButton, "CD");
        digitalWrite(ledPin, HIGH);
    }

    else if (currentCLState == HIGH)
    {
        Serial.println("CL");
        if (tx_MODE == 2)
        {
            bleGamepad.press(BUTTON_9);
        }
        strcpy(pressedButton, "CL");
        digitalWrite(ledPin, HIGH);
    }
    else if (currentCRState == HIGH)
    {
        Serial.println("CR");
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
        Serial.println("DU");
        if (tx_MODE == 2)
        {
            bleGamepad.press(BUTTON_11);
        }
        strcpy(pressedButton, "DU");
        digitalWrite(ledPin, HIGH);
    }

    else if (currentDDState == HIGH)
    {
        Serial.println("DD");
        if (tx_MODE == 2)
        {
            bleGamepad.press(BUTTON_12);
        }
        strcpy(pressedButton, "DD");
        digitalWrite(ledPin, HIGH);
    }

    else if (currentDLState == HIGH)
    {
        Serial.println("DL");
        if (tx_MODE == 2)
        {
            bleGamepad.press(BUTTON_13);
        }
        strcpy(pressedButton, "DL");
        digitalWrite(ledPin, HIGH);
    }
    else if (currentDRState == HIGH)
    {
        Serial.println("DR");
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
    float x_axis_raw = analogRead(JOY_STICK_VRX);
    float y_axis_raw = analogRead(JOY_STICK_VRY);

    // int axis_max = 4095;
    //  Convert joy stick value from -45 to 45
    //  x_axis_calibration_value = (x_axis_raw - axis_max / 2) / 22.76;
    //  y_axis_calibration_value = (y_axis_raw - axis_max / 2) / 22.76;

    x_axis_calibration_value = map(x_axis_raw, x_raw_min, x_raw_max, -32767, 32767);
    y_axis_calibration_value = map(y_axis_raw, y_raw_min, y_raw_max, -32767, 32767);

    Serial.print("X CALIBRATED VALUE: ");
    Serial.println(x_axis_calibration_value);
    Serial.print("Y CALIBRATED VALUE: ");
    Serial.println(y_axis_calibration_value);
}