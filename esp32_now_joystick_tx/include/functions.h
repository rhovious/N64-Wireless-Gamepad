#include "Arduino.h"
// Define which pins are used for each button
// Right side pins
#define BTN_START_PIN 36
#define BTN_Z_PIN 16
#define BTN_L_PIN 32
#define BTN_B_PIN 14
#define BTN_A_PIN 12
// Left side pins
#define BTN_R_PIN 23
#define BTN_C_DOWN_PIN 22
#define BTN_C_LEFT_PIN 17
#define BTN_C_UP_PIN 19
#define BTN_C_RIGHT_PIN 18

// D-PAD
#define DUP 32
#define DLEFT 33
#define DDOWN 27
#define DRIGHT 14

// sticks
const int JOY_STICK_VRX = 39;
const int JOY_STICK_VRY = 34;

const int buttonCount = 14;
int buttons[buttonCount] = {BTN_START_PIN, BTN_Z_PIN, BTN_L_PIN, BTN_B_PIN, BTN_A_PIN, BTN_R_PIN, BTN_C_DOWN_PIN, BTN_C_LEFT_PIN, BTN_C_UP_PIN, BTN_C_RIGHT_PIN, DUP, DLEFT, DDOWN, DRIGHT}; // 4 face buttons, 2 joy buttons

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


void printCompilationInfo()
{
    String str, adjStr;
    Serial.print("Arduino IDE version ");
    str = String(ARDUINO);
    adjStr = str.substring(1, str.length() - 5) + ".";
    adjStr = adjStr + str.substring(str.length() - 4, str.length() - 2) + ".";
    adjStr = adjStr + str.substring(str.length() - 2);
    Serial.println(adjStr); // date and time sketch compiled
    Serial.print("Compiler version    ");
    Serial.println(__VERSION__);
    Serial.print("Compiled date       ");
    Serial.println(__DATE__);
    Serial.print("Compiled time       ");
    Serial.println(__TIME__);
    Serial.print("Sketch location     ");
    Serial.println(__FILE__);
    Serial.print("CPU frequency(MHz)  "); // CPU frequency
    Serial.println(F_CPU / 1000000);
    Serial.print("Development board   ");
    Serial.println(ARDUINO_BOARD);
#ifdef __AVR__ // development board
    Serial.print("Microcontroller     ");
    Serial.println(ARDUINO_MCU);
#endif // microcontroller
    Serial.print("SPI MOSI ");
    Serial.println(MOSI); // pin layout SPI
    Serial.print("SPI MISO ");
    Serial.println(MISO);
    Serial.print("SPI SCK  ");
    Serial.println(SCK);
    Serial.print("SPI SS   ");
    Serial.println(SS);
    Serial.print("I2C SDA  ");
    Serial.println(SDA); // pin layout I2C
    Serial.print("I2C SCL  ");
    Serial.println(SCL);
#ifndef ESP32
    Serial.print("LED      ");
    Serial.println(LED_BUILTIN); // built-in LED
#endif                           // Arduino IDE version
}

void drawGUI(U8G2 u8g2)
{

    u8g2.setDrawColor(1); // white

    u8g2.setFont(u8g2_font_6x10_tf);

    // joysticks
    u8g2.drawCircle(xcenter - 45, ycenter - 6, arc - 15, U8G2_DRAW_ALL);
    int mappedJoyLposX = map(x_axis, -32737, 32737, -10, 10);
    int mappedJoyLposy = map(y_axis, -32737, 32737, -10, 10);
    // int mappedJoyLposX = 10; // REMOVE | SIM TESTING ONLY
    // int mappedJoyLposy = 10; // REMOVE | SIM TESTING ONLY
    u8g2.drawBox((xcenter - 45) + mappedJoyLposX, (ycenter - 6) + mappedJoyLposy, 2, 2);

    u8g2.drawCircle(xcenter + 45, ycenter - 6, arc - 15, U8G2_DRAW_ALL);
    int mappedJoyRposX = map(x_axis, -32737, 32737, -10, 10);
    int mappedJoyRposy = map(y_axis, -32737, 32737, -10, 10);
    // int mappedJoyRposX = 10;  // REMOVE | SIM TESTING ONLY
    // int mappedJoyRposy = -10; // REMOVE | SIM TESTING ONLY
    u8g2.drawBox((xcenter + 45) + mappedJoyRposX, (ycenter - 6) + mappedJoyRposy, 2, 2);

    // bottom border
    u8g2.drawBox(0, 54, 128, 10);

    // show label
    u8g2.drawStr(45, 10, "esp32BLE");

    // CONNECTED INDICATER
    u8g2.setDrawColor(0); // black
    if (connected == 0)
    {
        u8g2.drawStr(5, 62, "dc");
    }
    if (connected == 1)
    {
        u8g2.drawStr(5, 62, "c");
    }

    // prints out voltage and percentage to OLED

    // u8g2.setCursor(20, 20);  // COMMENT OUT WHEN SIMULATING
    // u8g2.print(voltage);     // COMMENT OUT WHEN SIMULATING
    u8g2.drawStr(120, 62, "%");
}

void handleButtons()
{

    int currentStartState = digitalRead(BTN_START_PIN);
    int currentZState = digitalRead(BTN_Z_PIN);
    int currentLState = digitalRead(BTN_L_PIN);
    int currentBState = digitalRead(BTN_B_PIN);
    int currentAState = digitalRead(BTN_A_PIN);
    int currentRState = digitalRead(BTN_R_PIN);

    int currentCDState = digitalRead(BTN_C_DOWN_PIN);
    int currentCLState = digitalRead(BTN_C_LEFT_PIN);
    int currentCUState = digitalRead(BTN_C_UP_PIN);
    int currentCRState = digitalRead(BTN_C_RIGHT_PIN);

    int currentDPadUpState = digitalRead(DUP);
    int currentDPadLeftState = digitalRead(DLEFT);
    int currentDPadDownState = digitalRead(DDOWN);
    int currentDPadRightState = digitalRead(DRIGHT);

    //-------------------HANDLE BUTTONS

    //--RIGHT SIDE BUTTONS; ZBXY
    if (currentStartState != previousStartState)
    {
        if (currentStartState == LOW)
        {
            Serial.println(F("START"));
            
            strcpy(pressedButton,"S");

        }
    }
    previousStartState = currentStartState;

    if (currentZState != previousZState)
    {
        if (currentZState == LOW)
        {
            Serial.println(F("Z"));
            strcpy(pressedButton,"Z");
            
        }
    }
    previousZState = currentZState;

    if (previousLState != currentLState)
    {
        if (currentLState == LOW)
        {
            Serial.println(F("L"));
            strcpy(pressedButton,"L");
            
        }
    }
    previousLState = currentLState;

    if (previousBState != currentBState)
    {
        if (currentBState == LOW)
        {
            Serial.println(F("B"));
            strcpy(pressedButton,"B");
            
        }
    }
    previousBState = currentBState;

    if (currentAState != previousAState)
    {
        if (currentAState == LOW)
        {
            Serial.println(F("A"));
            strcpy(pressedButton,"A");
            
        }
    }
    previousRState = currentRState;

    if (currentRState != previousRState)
    {
        if (currentRState == LOW)
        {
            Serial.println(F("R"));
            strcpy(pressedButton,"R");
         
        }
    }
    previousRState = currentRState;

    //----------C-PAD

    if (currentCDState != previousCDState)
    {
        if (currentCDState == LOW)
        {
            Serial.println(F("C DOWN"));
            strcpy(pressedButton,"CD");

        }
    }
    previousCDState = currentCDState;

    if (currentCLState != previousCLState)
    {
        if (currentCLState == LOW)
        {
            Serial.println(F("C LEFT"));
            strcpy(pressedButton,"CL");

        }
    }
    previousCLState = currentCLState;

    if (currentCUState != previousCUState)
    {
        if (currentCUState == LOW)
        {
            Serial.println(F("C UP"));
            strcpy(pressedButton,"CU");

        }
    }
    previousCUState = currentCUState;

    if (currentCRState != previousCRState)
    {
        if (currentCRState == LOW)
        {
            Serial.println(F("C RIGHT"));
            strcpy(pressedButton,"CR");

        }
    }
    previousCRState = currentCRState;

    //----------D-PAD
    if (currentDPadUpState != previousDPadUpState)
    {
        if (currentDPadUpState == LOW)
        {
            Serial.println(F("DPAD UP"));
            strcpy(pressedButton,"DU");

        }
    }
    previousDPadUpState = currentDPadUpState;

    if (currentDPadLeftState != previousDPadLeftState)
    {
        if (currentDPadLeftState == LOW)
        {
            Serial.println(F("DPAD_LEFT"));
            strcpy(pressedButton,"DL");
   
        }
    }
    previousDPadLeftState = currentDPadLeftState;

    if (currentDPadDownState != previousDPadDownState)
    {
        if (currentDPadDownState == LOW)
        {
            Serial.println(F("DPAD_DOWN"));
            strcpy(pressedButton,"DD");
    
        }
    }
    previousDPadDownState = currentDPadDownState;

    if (currentDPadRightState != previousDPadRightState)
    {
        if (currentDPadRightState == LOW)
        {
            Serial.println(F("DPAD_RIGHT"));
            strcpy(pressedButton,"DR");
   
        }
    }
    previousDPadRightState = currentDPadRightState;

    // handles no buttons pressed
    if (currentStartState == HIGH && currentDPadRightState == HIGH)
    {
        Serial.println(F("NONE"));
        strcpy(pressedButton,"0");

    }
}
