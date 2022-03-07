/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp-now-esp32-arduino-ide/

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/
//7C:87:CE:F6:DC:78 <---adafruit board esp32


#include <esp_now.h>
#include <WiFi.h>
#include <FastLED.h>
#include <U8g2lib.h>

// REPLACE WITH YOUR RECEIVER MAC Address
// {0x30, 0xAE, 0xA4, 0x07, 0x0D, 0x64}; FOR 30:AE:A4:07:0D:64
uint8_t broadcastAddress[] = {0x7C, 0x87, 0xCE, 0xF6, 0xDC, 0x78};

// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
  char buttonCMD[32];
  int leftStickX;
  int leftStickY;
  int RightStickX;
  int RightStickY;
} struct_message;

// Create a struct_message called myData
struct_message myData;

//---OLED
#define SCL 22
#define SDA 21

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* clock=*/SCL, /* data=*/SDA, /* reset=*/U8X8_PIN_NONE); // All Boards without Reset of the Display

int xmax = 128;
int ymax = 62;
int xcenter = xmax / 2;
int ycenter = ymax / 2 + 10;
int arc = 31;

//---LEDs
CRGB leds[6];
// LEFT-JOYSTICK
#define LH 34
#define LV 35
#define LS 15
int leftJoyVert = 0;
int leftJoyHoriz = 0;
int previousLSState = HIGH;

// RIGHT-JOYSTICK
#define RH 39
#define RV 36
#define RS 19
int rightJoyHoriz = 0;
int rightJoyVert = 0;
int previousRSState = HIGH;

// D-PAD
#define DUP 32
#define DLEFT 33
#define DDOWN 27
#define DRIGHT 14

// RIGHT SIDE BUTTONS; ZBXY
#define R1 18
#define R2 5
#define R3 17
#define R4 16

// BATTERY VOLTAGE
#define ADC 4

// NEOPIXEL
#define DATA_PIN 13

// PERIFERALS
//  #define MIC 25
//  #define SPEAKER 26


//--------VARS

int previousButton1State = HIGH;
int previousButton2State = HIGH;
int previousButton3State = HIGH;
int previousButton4State = HIGH;
int previousDPadUpState = HIGH;
int previousDPadLeftState = HIGH;
int previousDPadDownState = HIGH;
int previousDPadRightState = HIGH;

const int  buttonCount = 10;
int buttons[buttonCount] = {32, 33, 27, 14, 18, 5, 17, 16, 15, 19}; // 4 face buttons, 2 joy buttons


const int numberOfPotSamples = 5;     // Number of pot samples to take (to smooth the values)
const int delayBetweenSamples = 2;    // Delay in milliseconds between pot samples
const int delayBetweenHIDReports = 2000; // Additional delay in milliseconds between HID reports      // Delay in milliseconds between button press

int connected = 0;
float voltage = 0.0;
int percentage = 0;

unsigned long time_now = 0;

//---------------------------------------------------------FUNCTIONS


void drawGUI(U8G2 u8g2)
{

  u8g2.setDrawColor(1); // white

  u8g2.setFont(u8g2_font_6x10_tf);

  // joysticks
  u8g2.drawCircle(xcenter - 45, ycenter - 6, arc - 15, U8G2_DRAW_ALL);
  int mappedJoyLposX = map(leftJoyHoriz, -32737, 32737, -10, 10);
  int mappedJoyLposy = map(leftJoyVert, -32737, 32737, -10, 10);
  // int mappedJoyLposX = 10; // REMOVE | SIM TESTING ONLY
  // int mappedJoyLposy = 10; // REMOVE | SIM TESTING ONLY
  u8g2.drawBox((xcenter - 45) + mappedJoyLposX, (ycenter - 6) + mappedJoyLposy, 2, 2);

  u8g2.drawCircle(xcenter + 45, ycenter - 6, arc - 15, U8G2_DRAW_ALL);
  int mappedJoyRposX = map(rightJoyHoriz, -32737, 32737, -10, 10);
  int mappedJoyRposy = map(rightJoyVert, -32737, 32737, -10, 10);
  // int mappedJoyRposX = 10;  // REMOVE | SIM TESTING ONLY
  // int mappedJoyRposy = -10; // REMOVE | SIM TESTING ONLY
  u8g2.drawBox((xcenter + 45) + muiappedJoyRposX, (ycenter - 6) + mappedJoyRposy, 2, 2);

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
void sendTheReport()
{

//Serial.println("Sending Report");
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
    if (result == ESP_OK) {
      Serial.println("Sent with success");
      connected = 1;
    }
    else {
      Serial.println("Error sending the data");
      connected = 0;
    }
    
}
// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}
//---------------------------------------------------------SETUP
void setup() {
  //----FASTLED SETUP
  Serial.begin(115200);
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, 6);
  FastLED.setBrightness(64);

  for (int i = 0; i < buttonCount; i++)
  {
    pinMode(buttons[i], INPUT_PULLUP);
  }
  //LEDs setup
  leds[0] = CRGB::Red;
  leds[5] = CRGB::Red;
  FastLED.show();
  delay(500);
  leds[1] = CRGB::Green;
  leds[4] = CRGB::Green;
  FastLED.show();
  delay(500);
  leds[2] = CRGB::Blue;
  leds[3] = CRGB::Blue;
  FastLED.show();
  delay(1000);
  FastLED.clear();
  FastLED.show();

  //----OLED SETUP
  Serial.println("OLED SETUP START");
  u8g2.begin();
  u8g2.setFont(u8g2_font_6x10_tf);
  Serial.println("OLED SETUP DONE");

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);

  // Register peer
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
}

void loop() {
  //----HANDLE OLED
  {
    u8g2.firstPage();
    do
    {
      drawGUI(u8g2);
    } while (u8g2.nextPage());
  }
  // Set values to send
  strcpy(myData.buttonCMD, " ");
   myData.leftStickX = leftJoyHoriz;
  myData.leftStickY = leftJoyVert;
  myData.RightStickX = rightJoyHoriz;
  myData.RightStickY = rightJoyVert;


  // Send message via ESP-NOW
  //esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));


  handleButtons();
  handleJoysticks();

  // Send the gamepad report
  //while (millis() > time_now + delayBetweenHIDReports)
  //{
    //sendTheReport();
    time_now = millis();
    
  //}
  //delay(2000);
}
