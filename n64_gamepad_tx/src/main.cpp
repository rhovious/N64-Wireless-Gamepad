// I AM THE SENDER!!!
#include <WiFi.h>
#include <esp_now.h>
#include <AsyncElegantOTA.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <BleGamepad.h>
#include "secrets.h"
#include "webFunctions.h"
#include "controllerFunctions.h"
#include "Arduino.h"
#include "config.h"

// ==========================================================================
// BLE GAMEPAD

// WIFI SETUP
const char *ssid = secret_SSID;   // SSID
const char *password = secret_PW; // Password
AsyncWebServer server(80);

#ifdef WEBUI
// Create an Event Source on /events
AsyncEventSource events("/events1");

unsigned long lastTime = 0;
unsigned long timerDelay = 200; // delay between web updates
float buttonPressedWeb;

float x_axis_web;
float y_axis_web;
int tx_MODE_web;
#endif

typedef struct data_structure_espNOW
{
    int x;
    int y;
    char buttonCMD[3];
} data_structure_espNOW;

data_structure_espNOW sending_data_espNow;

// const int PUSH_BUTTON = 18;

//=============================================================================
//
#ifdef WEBUI
void getControllerReadingsforWeb()
{
    // Convert temperature to Fahrenheit
    buttonPressedWeb = (float)random(0, 100);
    x_axis_web = (float)x_axis;
    y_axis_web = (float)y_axis;
    tx_MODE_web = (int)tx_MODE;
}

String processor(const String &var)
{
    getControllerReadingsforWeb();
    // Serial.println(var);
    if (var == "buttonPressedWeb")
    {
        return String(buttonPressedWeb);
    }
    else if (var == "XAXIS")
    {
        return String(x_axis_web);
    }
    else if (var == "YAXIS")
    {
        return String(y_axis_web);
    }
    else if (var == "TX_MODE")
    {
        return String(tx_MODE_web);
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
        tx_MODE = 0;
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

void setupWebUI()
{
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(200, "text/plain", "/n64"); });

    server.on("/n64", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send_P(200, "text/html", index_html, processor); });
    // Handle Web Server Events
    events.onConnect([](AsyncEventSourceClient *client)
                     {

    if(client->lastId()){
      Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
    }
    // send event with message "hello!", id current millis
    // and set reconnect delay to 1 second
    client->send("hello!", NULL, millis(), 10000); });
    server.addHandler(&events);
}

void setupModeBLEGamepad()
{
}

void setup()
{
    setPinModes();
    digitalWrite(ledPin, HIGH);

    setupWifi();
    setStickCalibration();
    Serial.begin(115200);

#ifdef WEBUI
    setupWebUI();
#endif
    AsyncElegantOTA.begin(&server); // Start ElegantOTA
    server.begin();
    Serial.println("HTTP server started");

    setMode();

    digitalWrite(ledPin, LOW);
}

//  MAIN LOOP
void loop()
{
#ifdef WEBUI
    if ((millis() - lastTime) > timerDelay)
    {
        getControllerReadingsforWeb();
        events.send("ping", NULL, millis());
        events.send(String(pressedButton).c_str(), "temperature", millis());
        events.send(String(x_axis_web).c_str(), "XAXIS", millis());
        events.send(String(y_axis_web).c_str(), "YAXIS", millis());
        events.send(String(tx_MODE_web).c_str(), "TX_MODE", millis());

        lastTime = millis();
    }
#endif

    if (tx_MODE == 1) // ESP-NOW
    {
        sending_data_espNow.x = x_axis;
        sending_data_espNow.y = y_axis;
        handleButtons(); // sets pressedButton to what is pressed
        handleSticks();  // sets sticks to value
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
    if (tx_MODE == 2) // BLE
    {
        handleButtons(); // sets pressedButton to what is pressed
        handleSticks();  // sets sticks to value
    }
#ifdef DEBUG
    Serial.print("MODE: ");
    Serial.println(tx_MODE); // 0 = undefinder, 1 = esp-now, 2 = BLE
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
    delay(100);
}
//=============================================================================