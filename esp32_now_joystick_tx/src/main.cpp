// I AM THE SENDER!!!
#include <WiFi.h>
#include <esp_now.h>
#include <AsyncElegantOTA.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "secrets.h"
#include "Arduino.h"

// ==========================================================================

const char *ssid = secret_SSID;   // SSID
const char *password = secret_PW; // Password
AsyncWebServer server(80);

// Create an Event Source on /events
AsyncEventSource events("/events");

unsigned long lastTime = 0;
unsigned long timerDelay = 200; //delay between web updates
float temperature;
float x_axis_web;
float y_axis_web;

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>n64 Controller Debug</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <link rel="icon" href="data:,">
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    p { font-size: 1.2rem;}
    body {  margin: 0;}
    .topnav { overflow: hidden; background-color: #50B8B4; color: white; font-size: 1rem; }
    .content { padding: 20px; }
    .card { background-color: white; box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5); }
    .cards { max-width: 800px; margin: 0 auto; display: grid; grid-gap: 2rem; grid-template-columns: repeat(auto-fit, minmax(200px, 1fr)); }
    .reading { font-size: 1.4rem; }
  </style>
</head>
<body>
  <div class="topnav">
    <h1>n64 Controller Debug</h1>
  </div>
  <div class="content">
    <div class="cards">
      <div class="card">
        <p><i class="fas fa-angle-double-down" style="color:#059e8a;"></i> BUTTON</p><p><span class="reading"><span id="temp">%TEMPERATURE%</span> </span></p>
      </div>
      <div class="card">
        <p><i class="fas fa-gamepad" style="color:#00add6;"></i> x</p><p><span class="reading"><span id="hum">%XAXIS%</span></span></p>
      </div>
      <div class="card">
        <p><i class="fas fa-gamepad" style="color:#e1e437;"></i> y</p><p><span class="reading"><span id="pres">%YAXIS%</span></span></p>
      </div>
    </div>
  </div>
<script>
if (!!window.EventSource) {
 var source = new EventSource('/events');
 
 source.addEventListener('open', function(e) {
  console.log("Events Connected");
 }, false);
 source.addEventListener('error', function(e) {
  if (e.target.readyState != EventSource.OPEN) {
    console.log("Events Disconnected");
  }
 }, false);
 
 source.addEventListener('message', function(e) {
  console.log("message", e.data);
 }, false);
 
 source.addEventListener('temperature', function(e) {
  console.log("temperature", e.data);
  document.getElementById("temp").innerHTML = e.data;
 }, false);
 
 source.addEventListener('XAXIS', function(e) {
  console.log("XAXIS", e.data);
  document.getElementById("hum").innerHTML = e.data;
 }, false);
 
 source.addEventListener('YAXIS', function(e) {
  console.log("YAXIS", e.data);
  document.getElementById("pres").innerHTML = e.data;
 }, false);
}
</script>
</body>
</html>)rawliteral";

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

float x_axis;
float y_axis;

int connected = 0;

//=============================================================================
//

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

    //-------------------HANDLE BUTTONS

    /*
        //--RIGHT SIDE BUTTONS; ZBXY
        if (currentStartState != previousStartState)
        {
            if (currentStartState == HIGH)
            {
                Serial.println(F("START"));

                strcpy(pressedButton, "S");
            }
            previousStartState = currentStartState;
        }

        else if (currentZState != previousZState)
        {
            if (currentZState == LOW)
            {
                Serial.println(F("Z"));
                strcpy(pressedButton, "Z");
            }
            previousZState = currentZState;
        }

        else if (previousLState != currentLState)
        {
            if (currentLState == LOW)
            {
                Serial.println(F("L"));
                strcpy(pressedButton, "L");
            }
            previousLState = currentLState;
        }

        else if (currentBState != previousBState)
        {
            if (currentBState == LOW)
            {
                Serial.println(F("B"));
                strcpy(pressedButton, "B");
            }
            previousBState = currentBState;
        }

        else if (currentAState != previousAState)
        {
            if (currentAState == LOW)
            {
                Serial.println(F("A"));
                strcpy(pressedButton, "A");
            }
            previousAState = currentAState;
        }

        // handles no buttons pressed

        else if (currentRState != previousRState)
        {
            if (currentRState == LOW)
            {
                Serial.println(F("R"));
                strcpy(pressedButton, "R");
            }
            previousRState = currentRState;
        }

        //----------C-PAD

        else if (currentCDState != previousCDState)
        {
            if (currentCDState == LOW)
            {
                Serial.println(F("C DOWN"));
                strcpy(pressedButton, "CD");
            }
            previousCDState = currentCDState;
        }

        else if (currentCLState != previousCLState)
        {
            if (currentCLState == LOW)
            {
                Serial.println(F("C LEFT"));
                strcpy(pressedButton, "CL");
            }
            previousCLState = currentCLState;
        }

        else if (currentCUState != previousCUState)
        {
            if (currentCUState == LOW)
            {
                Serial.println(F("C UP"));
                strcpy(pressedButton, "CU");
            }
            previousCUState = currentCUState;
        }

        else if (currentCRState != previousCRState)
        {
            if (currentCRState == LOW)
            {
                Serial.println(F("C RIGHT"));
                strcpy(pressedButton, "CR");
            }
            previousCRState = currentCRState;
        }

        //----------D-PAD
        else if (currentDPadUpState != previousDPadUpState)
        {
            if (currentDPadUpState == LOW)
            {
                Serial.println(F("DPAD UP"));
                strcpy(pressedButton, "DU");
            }
            previousDPadUpState = currentDPadUpState;
        }

        else if (currentDPadLeftState != previousDPadLeftState)
        {
            if (currentDPadLeftState == LOW)
            {
                Serial.println(F("DPAD_LEFT"));
                strcpy(pressedButton, "DL");
            }
            previousDPadLeftState = currentDPadLeftState;
        }

        else if (currentDPadDownState != previousDPadDownState)
        {
            if (currentDPadDownState == LOW)
            {
                Serial.println(F("DPAD_DOWN"));
                strcpy(pressedButton, "DD");
            }
            previousDPadDownState = currentDPadDownState;
        }

        else if (currentDPadRightState != previousDPadRightState)
        {
            if (currentDPadRightState == LOW)
            {
                Serial.println(F("DPAD_RIGHT"));
                strcpy(pressedButton, "DR");
            }
            previousDPadRightState = currentDPadRightState;
        }
        else
        {
            // turn LED off:
            strcpy(pressedButton, "0");
            digitalWrite(ledPin, LOW);
        }

    */

    if (currentBState == HIGH)
    {
        Serial.println("B");
        strcpy(pressedButton, "B");
        digitalWrite(ledPin, HIGH);
    }

    else if (currentStartState == HIGH)
    {
        Serial.println("START");
        strcpy(pressedButton, "S");
        digitalWrite(ledPin, HIGH);
    }

    else if (currentAState == HIGH)
    {
        Serial.println("A");
        strcpy(pressedButton, "A");
        // turn LED on:
        digitalWrite(ledPin, HIGH);
    }

    else if (currentSRState == HIGH)
    {
        Serial.println("SR");
        strcpy(pressedButton, "SR");
        // turn LED on:
        digitalWrite(ledPin, HIGH);
    }

    else if (currentSLState == HIGH)
    {
        Serial.println("SL");
        strcpy(pressedButton, "SL");
        // turn LED on:
        digitalWrite(ledPin, HIGH);
    }

    else if (currentZState == HIGH)
    {
        Serial.println("Z");
        strcpy(pressedButton, "Z");
        // turn LED on:
        digitalWrite(ledPin, HIGH);
    }

    //~~~~~~~C PAD
    else if (currentCUPState == HIGH)
    {
        Serial.println("CU");
        strcpy(pressedButton, "CU");
        digitalWrite(ledPin, HIGH);
    }

    else if (currentCDState == HIGH)
    {
        Serial.println("CD");
        strcpy(pressedButton, "CD");
        digitalWrite(ledPin, HIGH);
    }

    else if (currentCLState == HIGH)
    {
        Serial.println("CL");
        strcpy(pressedButton, "CL");
        digitalWrite(ledPin, HIGH);
    }
    else if (currentCRState == HIGH)
    {
        Serial.println("CR");
        strcpy(pressedButton, "CR");
        digitalWrite(ledPin, HIGH);
    }

    //~~~~~~~D PAD
    else if (currentDUPState == HIGH)
    {
        Serial.println("DU");
        strcpy(pressedButton, "DU");
        digitalWrite(ledPin, HIGH);
    }

    else if (currentDDState == HIGH)
    {
        Serial.println("DD");
        strcpy(pressedButton, "DD");
        digitalWrite(ledPin, HIGH);
    }

    else if (currentDLState == HIGH)
    {
        Serial.println("DL");
        strcpy(pressedButton, "DL");
        digitalWrite(ledPin, HIGH);
    }
    else if (currentDRState == HIGH)
    {
        Serial.println("DR");
        strcpy(pressedButton, "DR");
        digitalWrite(ledPin, HIGH);
    }

    else
    {
        strcpy(pressedButton, "0");
        // turn LED off:
        digitalWrite(ledPin, LOW);
    }
}

void setup()
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

    digitalWrite(ledPin, HIGH);
    Serial.begin(115200);
    //~~~~~~~~~~~~~~~~ WIFI STUP
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.println("");

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

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

    AsyncElegantOTA.begin(&server); // Start ElegantOTA
    server.begin();
    Serial.println("HTTP server started");

    // Set device as a Wi-Fi Station'

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
        Serial.println("Failed to connect to ESP client");
        return;
    }
    digitalWrite(ledPin, LOW);
}

//  MAIN LOOP
void loop()
{

    if ((millis() - lastTime) > timerDelay)
    {
        getControllerReadings();
        events.send("ping", NULL, millis());
        events.send(String(pressedButton).c_str(), "temperature", millis());
        events.send(String(x_axis_web).c_str(), "XAXIS", millis());
        events.send(String(y_axis_web).c_str(), "YAXIS", millis());

        lastTime = millis();
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
    strcpy(sending_data.buttonCMD, pressedButton);

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
