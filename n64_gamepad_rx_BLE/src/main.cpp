#include <Arduino.h>
#include <BleGamepad.h>
#include <FastLED.h>
#include <U8g2lib.h>

//---OLED
#define SCL 22
#define SDA 21

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* clock=*/SCL, /* data=*/SDA, /* reset=*/U8X8_PIN_NONE); // All Boards without Reset of the Display

int xmax = 128;
int ymax = 62;
int xcenter = xmax / 2;
int ycenter = ymax / 2 + 10;
int arc = 31;


//---LED
CRGB leds[6];

//---GAMEPAD

BleGamepad bleGamepad("ESP Controller", "Rhovious", 100);

// LEFT-JOYSTICK
#define LH 34
#define LV 35
#define LS 15

// RIGHT-JOYSTICK
#define RH 39
#define RV 36
#define RS 19

// D-PAD
#define L1 32
#define L2 33
#define L3 27
#define L4 14

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

// int buttons[2] = {15, 19}; // joystick buttons only

int buttons[10] = {32, 33, 27, 14, 18, 5, 17, 16, 15, 19}; // 4 face buttons, 2 joy buttons

int period = 10000;
unsigned long time_now = 0;

const int numberOfPotSamples = 5;     // Number of pot samples to take (to smooth the values)
const int delayBetweenSamples = 2;    // Delay in milliseconds between pot samples
const int delayBetweenHIDReports = 5; // Additional delay in milliseconds between HID reports
const int debounceDelay = 10;         // Delay in milliseconds between button press
float voltage = 0.0;
int percentage = 0;

//---VARS
int connected = 0;

//---------------------------------------------------------FUNCTIONS
void batteryLevel()
{
  int sensorValue = analogRead(ADC);
  voltage = sensorValue * (5.12 / 4095.0);
  Serial.print(voltage);
  Serial.print("V ||");
  percentage = (voltage / 4.2) * 100;
  Serial.print(percentage);
  Serial.println("%");
  bleGamepad.setBatteryLevel(percentage);
  if (percentage < 60)
  {
    Serial.println("LOW battery");
    for (uint8_t i = 30; i < 220; i++)
    {
      for (int j = 0; j < 6; j++)
      {
        leds[j] = CHSV(HUE_RED, 255, i);
      }
      FastLED.show();
      // delay(15);
    }
    for (uint8_t i = 220; i > 30; i--)
    {
      for (int j = 0; j < 6; j++)
      {
        leds[j] = CHSV(HUE_RED, 255, i);
      }
      FastLED.show();
      // delay(15);
    }
  }
}

// FUNCTIONS ------------------------------------------------------------------------------------

void drawGUI(U8G2 u8g2)
{

  u8g2.setDrawColor(1); // white
  // u8g2.setFont(u8g2_font_chikita_tr);
  u8g2.setFont(u8g2_font_6x10_tf);

  // joysticks
  u8g2.drawCircle(xcenter - 50, ycenter - 3, arc - 20, U8G2_DRAW_ALL);
   int mappedJoyLposX = map(analogRead(LH), 0, 4095, 5, -5);
   int mappedJoyLposy = map(analogRead(LV), 0, 4095, 5, -5);
  //int mappedJoyLposX = 0; // REMOVE
  //int mappedJoyLposy = 0; // REMOVE
  u8g2.drawBox((xcenter - 50) + mappedJoyLposX, (ycenter - 3) + mappedJoyLposy, 2, 2);

  u8g2.drawCircle(xcenter + 50, ycenter - 3, arc - 20, U8G2_DRAW_ALL);
  // int mappedJoyRposX = map(analogRead(RH), 0, 4095, 5, -5);
  // int mappedJoyRposy = map(analogRead(RV), 0, 4095, 5, -5);
  int mappedJoyRposX = 0; // REMOVE
  int mappedJoyRposy = 0; // REMOVE
  u8g2.drawBox((xcenter + 50) + mappedJoyRposX, (ycenter - 3) + mappedJoyRposy, 2, 2);

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

  int voltage = 0;
  // u8g2.setCursor(20, 20);
  // u8g2.print(voltage);
  u8g2.drawStr(120, 61, "v");

  // u8g2.setCursor(105, 50);
  // u8g2.print(percentage);
  u8g2.drawStr(100, 60, "%");
}
//---------------------------------------------------------SETUP
void setup()
{
  //----FASTLED SETUP
  Serial.begin(115200);
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, 6);
  FastLED.setBrightness(64);
  for (int i = 0; i < 12; i++)
  {
    pinMode(buttons[i], INPUT_PULLUP);
  }
  //----GAMEPAD SETUP
  bleGamepad.begin();
  bleGamepad.setAutoReport(false); // to disable auto reporting, and then use bleGamepad.sendReport(); as needed
  Serial.println("Starting BLE work!");

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
  u8g2.setFont(u8g2_font_chikita_tr);
  Serial.println("OLED SETUP DONE");
}

//---------------------------------------------------------LOOP
void loop()
{
  //----OLED LOOP

  {
    u8g2.firstPage();
    do
    {
      
      drawGUI(u8g2);
    } while (u8g2.nextPage());
  }

  if (bleGamepad.isConnected())
  {
    connected = 1;
    for (int i = 0; i < 6; i++)
    {
      leds[i] = CHSV(HUE_GREEN, 255, 64);
    }
    FastLED.show();

    while (millis() > time_now + period)
    {
      Serial.println("Checking Battery Level");
      batteryLevel();
      time_now = millis();
    }
    /*
        if (digitalRead(LT) == LOW)
        {
          Serial.println(F("Left Trigger"));
          bleGamepad.press(BUTTON_7);
          bleGamepad.sendReport();
          delay(debounceDelay);
          bleGamepad.release(BUTTON_7);
          bleGamepad.sendReport();
        }
        if (digitalRead(RT) == LOW)
        {
          Serial.println(F("Right Trigger"));
          bleGamepad.press(BUTTON_8);
          bleGamepad.sendReport();
          delay(debounceDelay);
          bleGamepad.release(BUTTON_8);
          bleGamepad.sendReport();
        }
    */
    if (digitalRead(LS) == LOW)
    {
      Serial.println(F("Left Joystick"));

      bleGamepad.press(BUTTON_14);
      bleGamepad.sendReport();
      delay(debounceDelay);
      bleGamepad.release(BUTTON_14);
      bleGamepad.sendReport();
    }
    if (digitalRead(RS) == LOW)
    {
      Serial.println(F("Right Joystick"));
      bleGamepad.press(BUTTON_15);
      bleGamepad.sendReport();
      delay(debounceDelay);
      bleGamepad.release(BUTTON_15);
      bleGamepad.sendReport();
    }

    //----------RIGHT SIDE BUTTONS; ZBXY

    if (digitalRead(R1) == LOW)
    {
      Serial.println(F("Right Button 1"));
      bleGamepad.press(BUTTON_5);
      bleGamepad.sendReport();
      delay(debounceDelay);
      bleGamepad.release(BUTTON_5);
      bleGamepad.sendReport();
    }
    if (digitalRead(R2) == LOW)
    {
      Serial.println(F("Right Button 2"));
      bleGamepad.press(BUTTON_2);
      bleGamepad.sendReport();
      delay(debounceDelay);
      bleGamepad.release(BUTTON_2);
      bleGamepad.sendReport();
    }
    if (digitalRead(R3) == LOW)
    {
      Serial.println(F("Right Button 3"));
      bleGamepad.press(BUTTON_1);
      bleGamepad.sendReport();
      delay(debounceDelay);
      bleGamepad.release(BUTTON_1);
      bleGamepad.sendReport();
    }
    if (digitalRead(R4) == LOW)
    {
      Serial.println(F("Right Button 4"));
      bleGamepad.press(BUTTON_4);
      bleGamepad.sendReport();
      delay(debounceDelay);
      bleGamepad.release(BUTTON_4);
      bleGamepad.sendReport();
    }

    //----------D-PAD

    if (digitalRead(L1) == LOW)
    {
      Serial.println(F("Left Button 1"));
      bleGamepad.setHat1(DPAD_UP);
      bleGamepad.sendReport();
      delay(debounceDelay);
      bleGamepad.setHat1();
      bleGamepad.sendReport();
    }
    if (digitalRead(L2) == LOW)
    {
      Serial.println(F("Left Button 2"));
      bleGamepad.setHat1(DPAD_LEFT);
      bleGamepad.sendReport();
      delay(debounceDelay);
      bleGamepad.setHat1();
      bleGamepad.sendReport();
    }
    if (digitalRead(L3) == LOW)
    {
      Serial.println(F("Left Button 3"));
      bleGamepad.setHat1(DPAD_DOWN);
      bleGamepad.sendReport();
      delay(debounceDelay);
      bleGamepad.setHat1();
      bleGamepad.sendReport();
    }
    if (digitalRead(L4) == LOW)
    {
      Serial.println(F("Left Button 4"));
      bleGamepad.setHat1(DPAD_RIGHT);
      bleGamepad.sendReport();
      delay(debounceDelay);
      bleGamepad.setHat1();
      bleGamepad.sendReport();
    }

    // Map analog reading from 0 ~ 4095 to 32737 ~ -32737 for use as an axis reading
    int leftJoyHoriz = map(analogRead(LH), 0, 4095, 32737, -32737);
    int leftJoyVert = map(analogRead(LV), 0, 4095, 32737, -32737);
    bleGamepad.setLeftThumb(leftJoyHoriz, leftJoyVert);
    // Serial.println(leftJoyHoriz);
    // Serial.println(leftJoyVert);

    int rightJoyHoriz = map(analogRead(RH), 0, 4095, 0, 65475);
    int rightJoyVert = map(analogRead(RV), 0, 4095, 0, 65475);
    bleGamepad.setRightThumb(rightJoyHoriz, rightJoyVert);
    // Serial.println(rightJoyHoriz);
    // Serial.println(rightJoyVert);

    // Send the gamepad report
    bleGamepad.sendReport();
    delay(1);
  }
  else
  {
    connected = 0;
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
    // for (int i = 0; i < 6; i++)
    //  {
    //      leds[i] = CHSV(0, 0, 0);
    //   }
    //    FastLED.show();
  }
}
