#include <WiFi.h>
#include <esp_now.h>
#include <U8g2lib.h>
#include "Arduino.h"

// I AM THE SENDER!!!
// ==========================================================================
// REPLACE WITH YOUR ESP RECEIVER'S MAC ADDRESS
// you the program find_mac_address in this repo to find your ESP32's MAC
// address
// 7C:87:CE:F6:DC:78
uint8_t broadcastAddress1[] = {0x7c, 0x87, 0xCE, 0xF6, 0xDC, 0x78};
// if you have other receivers you can define in the following
// uint8_t broadcastAddress2[] = {0xFF, , , , , };
// uint8_t broadcastAddress3[] = {0xFF, , , , , };

// ==========================================================================
// your data structure (the data block you send to receiver)
// your reciever should use the same data structure
typedef struct data_structure
{
    int x;
    int y;
    // bool isButtonPressed;
    char buttonCMD[3];
} data_structure;

data_structure sending_data;

// const int PUSH_BUTTON = 18;

char pressedButton[3] = "0";

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

//---OLED
#define SCL 22
#define SDA 21

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* clock=*/SCL, /* data=*/SDA, /* reset=*/U8X8_PIN_NONE); // All Boards without Reset of the Display

int xmax = 128;
int ymax = 62;
int xcenter = xmax / 2;
int ycenter = ymax / 2 + 10;
int arc = 31;

float x_axis;
float y_axis;

int connected = 0;

//=============================================================================
//

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

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
    char macStr[18];
    Serial.print("Packet to: ");
    // Copies the sender mac address to a string
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
             mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4],
             mac_addr[5]);
    Serial.print(macStr);
    Serial.print(" send status:\t");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success"
                                                  : "Delivery Fail");
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
        if (currentStartState == LOW)`
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

void setup()
{
    pinMode(JOY_STICK_VRX, INPUT);
    pinMode(JOY_STICK_VRY, INPUT);
    for (int i = 0; i < buttonCount; i++)
    {
        pinMode(buttons[i], INPUT_PULLUP);
    }

    Serial.begin(115200);

    //----OLED SETUP
    Serial.println("OLED SETUP START");
    u8g2.begin();
    u8g2.setFont(u8g2_font_6x10_tf);
    Serial.println("OLED SETUP DONE");

    // Set device as a Wi-Fi Station'
    WiFi.mode(WIFI_STA);

    if (esp_now_init() != ESP_OK)
    {
        Serial.println("Error initializing ESP-NOW");
        return;
    }

    esp_now_register_send_cb(OnDataSent);

    // register peer
    esp_now_peer_info_t peerInfo;
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    // register first peer
    memcpy(peerInfo.peer_addr, broadcastAddress1, 6);
    if (esp_now_add_peer(&peerInfo) != ESP_OK)
    {
        Serial.println("Failed to add peer");
        return;
    }
}

//  MAIN LOOP
void loop()
{
    //----HANDLE OLED
    {
        u8g2.firstPage();
        do
        {
            drawGUI(u8g2);
        } while (u8g2.nextPage());
    }
    x_axis = analogRead(JOY_STICK_VRX);
    y_axis = analogRead(JOY_STICK_VRY);

    int axis_max = 4095;
    // Convert joy stick value from -45 to 45
    x_axis = (x_axis - axis_max / 2) / 22.76;
    y_axis = (y_axis - axis_max / 2) / 22.76;

    Serial.print("X_axis: ");
    Serial.print(x_axis, 0); // round off to the first dicimal place
    Serial.print("\n");
    Serial.print("Y-axis: "); // round off to the first dicimal place
    Serial.print(y_axis, 0);
    Serial.print("\n");
    Serial.print("pressedButton: ");
    Serial.print(pressedButton);
    Serial.print("\n\n");
    sending_data.x = x_axis;
    sending_data.y = y_axis;
    handleButtons(); // sets pressedButton to what is pressed
    strcpy(sending_data.buttonCMD,pressedButton);
    //
    esp_err_t result =
        esp_now_send(0, (uint8_t *)&sending_data, sizeof(data_structure));

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
    delay(100);
}
//=============================================================================
// END
