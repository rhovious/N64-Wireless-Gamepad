// I AM THE SENDER!!!
#include <WiFi.h>
#include <esp_now.h>
#include <AsyncElegantOTA.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "secrets.h"
#include "webFunctions.h"
#include "Arduino.h"

// ==========================================================================
//#define DEBUG
//#define WEBUI

const char *ssid = secret_SSID;   // SSID
const char *password = secret_PW; // Password
AsyncWebServer server(80);

#ifdef WEBUI
// Create an Event Source on /events
AsyncEventSource events("/events");

unsigned long lastTime = 0;
unsigned long timerDelay = 200; // delay between web updates
float temperature;

float x_axis_web;
float y_axis_web;
#endif

typedef struct data_structure
{
    int x;
    int y;
    char buttonCMD[3];
} data_structure;

data_structure sending_data;

// const int PUSH_BUTTON = 18;

char pressedButton[3] = "0";

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

float x_axis_calibration_value;
float y_axis_calibration_value;

int connected = 0;

//=============================================================================
//
#ifdef WEBUI
void getControllerReadings()
{
    // Convert temperature to Fahrenheit
    temperature = (float)random(0, 100);
    x_axis_web = (float)x_axis;
    y_axis_web = (float)y_axis;
}

String processor(const String &var)
{
    getControllerReadings();
    // Serial.println(var);
    if (var == "TEMPERATURE")
    {
        return String(temperature);
    }
    else if (var == "XAXIS")
    {
        return String(x_axis_web);
    }
    else if (var == "YAXIS")
    {
        return String(y_axis_web);
    }
    return String();
}
#endif
// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
    char macStr[18];
#ifdef DEBUG
    Serial.print("Packet to: ");
#endif
    // Copies the sender mac address to a string
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
             mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4],
             mac_addr[5]);
#ifdef DEBUG
    Serial.print(macStr);
    Serial.print(" send status:\t");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success"
                                                  : "Delivery Fail");
#endif
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
        strcpy(pressedButton, "B");
        digitalWrite(ledPin, HIGH);
    }

    else if (currentStartState == HIGH)
    {
#ifdef DEBUG
        Serial.println("START");
#endif
        strcpy(pressedButton, "S");
        digitalWrite(ledPin, HIGH);
    }

    else if (currentAState == HIGH)
    {
#ifdef DEBUG
        Serial.println("A");
#endif
        strcpy(pressedButton, "A");
        // turn LED on:
        digitalWrite(ledPin, HIGH);
    }

    else if (currentSRState == HIGH)
    {
#ifdef DEBUG
        Serial.println("SR");
#endif
        strcpy(pressedButton, "SR");
        // turn LED on:
        digitalWrite(ledPin, HIGH);
    }

    else if (currentSLState == HIGH)
    {
#ifdef DEBUG
        Serial.println("SL");
#endif
        strcpy(pressedButton, "SL");
        // turn LED on:
        digitalWrite(ledPin, HIGH);
    }

    else if (currentZState == HIGH)
    {
#ifdef DEBUG
        Serial.println("Z");
#endif
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
        strcpy(pressedButton, "CU");
        digitalWrite(ledPin, HIGH);
    }

    else if (currentCDState == HIGH)
    {
#ifdef DEBUG
        Serial.println("CD");
#endif
        strcpy(pressedButton, "CD");
        digitalWrite(ledPin, HIGH);
    }

    else if (currentCLState == HIGH)
    {
#ifdef DEBUG
        Serial.println("CL");
#endif
        strcpy(pressedButton, "CL");
        digitalWrite(ledPin, HIGH);
    }
    else if (currentCRState == HIGH)
    {
#ifdef DEBUG
        Serial.println("CR");
#endif
        strcpy(pressedButton, "CR");
        digitalWrite(ledPin, HIGH);
    }

    //~~~~~~~D PAD
    else if (currentDUPState == HIGH)
    {
#ifdef DEBUG
        Serial.println("DU");
#endif
        strcpy(pressedButton, "DU");
        digitalWrite(ledPin, HIGH);
    }

    else if (currentDDState == HIGH)
    {
#ifdef DEBUG
        Serial.println("DD");
#endif
        strcpy(pressedButton, "DD");
        digitalWrite(ledPin, HIGH);
    }

    else if (currentDLState == HIGH)
    {
#ifdef DEBUG
        Serial.println("DL");
#endif
        strcpy(pressedButton, "DL");
        digitalWrite(ledPin, HIGH);
    }
    else if (currentDRState == HIGH)
    {
#ifdef DEBUG
        Serial.println("DR");
#endif
        strcpy(pressedButton, "DR");
        digitalWrite(ledPin, HIGH);
    }

    else
    {
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

void setupWifi()
{
    //~~~~~~~~~~~~~~~~ WIFI STUP
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
#ifdef DEBUG
    Serial.println("");

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
#endif
#ifdef DEBUG
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
#endif
}
void setupESPNOW()
{
#ifdef DEBUG
    if (esp_now_init() != ESP_OK)
    {
        Serial.println("Error initializing ESP-NOW");
        return;
    }
#endif

    esp_now_register_send_cb(OnDataSent);

    // register peer
    esp_now_peer_info_t peerInfo;
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    // register first peer
    memcpy(peerInfo.peer_addr, broadcastAddress1, 6);
#ifdef DEBUG
    if (esp_now_add_peer(&peerInfo) != ESP_OK)
    {
        Serial.println("Failed to connect to ESP client");
        return;
    }
#endif
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

void setup()
{
    setPinModes();
    digitalWrite(ledPin, HIGH);

    setupWifi();
    setStickCalibration();
#ifdef DEBUG
    Serial.begin(115200);
    printCompilationInfi();
#endif
#ifdef WEBUI
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(200, "text/plain", "/n64"); });

    server.on("/n64", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send_P(200, "text/html", index_html, processor); });
    // Handle Web Server Events
    events.onConnect([](AsyncEventSourceClient *client)
                     {
#ifdef DEBUG
    if(client->lastId()){
      Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
    }
#endif
    // send event with message "hello!", id current millis
    // and set reconnect delay to 1 second
    client->send("hello!", NULL, millis(), 10000); });
    server.addHandler(&events);
#endif
    AsyncElegantOTA.begin(&server); // Start ElegantOTA
    server.begin();
#ifdef DEBUG
    Serial.println("HTTP server started");
#endif
    setupESPNOW();

    digitalWrite(ledPin, LOW);
}

//  MAIN LOOP
void loop()
{
#ifdef WEBUI
    if ((millis() - lastTime) > timerDelay)
    {
        getControllerReadings();
        events.send("ping", NULL, millis());
        events.send(String(pressedButton).c_str(), "temperature", millis());
        events.send(String(x_axis_web).c_str(), "XAXIS", millis());
        events.send(String(y_axis_web).c_str(), "YAXIS", millis());

        lastTime = millis();
    }
#endif
    x_axis_read = analogRead(JOY_STICK_VRX);
    y_axis_read = analogRead(JOY_STICK_VRY);

    int axis_max = 4095;
    // Convert joy stick value from -45 to 45
    x_axis = (x_axis_read - axis_max / 2) / 22.76;
    y_axis = (y_axis_read - axis_max / 2) / 22.76;
#ifdef DEBUG
    Serial.print("X_axis: ");
    Serial.print(x_axis, 0); // round off to the first dicimal place
    Serial.print("\n");
    Serial.print("Y-axis: "); // round off to the first dicimal place
    Serial.print(y_axis, 0);
    Serial.print("\n");
    Serial.print("pressedButton: ");
    Serial.print(pressedButton);
    Serial.print("\n\n");
#endif
    sending_data.x = x_axis;
    sending_data.y = y_axis;
    handleButtons(); // sets pressedButton to what is pressed
    strcpy(sending_data.buttonCMD, pressedButton);

    //
    esp_err_t result =
        esp_now_send(0, (uint8_t *)&sending_data, sizeof(data_structure));
#ifdef DEBUG
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
#endif
    delay(100);
}
//=============================================================================
// END
