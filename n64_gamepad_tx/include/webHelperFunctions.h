
#include "Arduino.h"
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>
#include "webPage.h"
// WIFI SETUP
const char *ssid = secret_SSID;   // SSID
const char *password = secret_PW; // Password
AsyncWebServer server(80);

float buttonPressedWeb;

float x_axis_web;
float y_axis_web;
int tx_MODE_web;

unsigned long lastTime = 0;
unsigned long timerDelay = 200; // delay between web updates

// Create an Event Source on /events
AsyncEventSource events("/events1");

void setupWifi()
{
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
}

void getControllerReadingsforWeb()
{
    //buttonPressedWeb = (float)random(0, 100);
    x_axis_web = (float)x_axis_adjusted;
    y_axis_web = (float)y_axis_adjusted;
    tx_MODE_web = (int)tx_MODE;
}

void updateWeb()
{
    if ((millis() - lastTime) > timerDelay)
    {
        getControllerReadingsforWeb();
        events.send("ping", NULL, millis());
        events.send(String(pressedButton).c_str(), "BUTTONPRESSED", millis());
        events.send(String(x_axis_web).c_str(), "XAXIS", millis());
        events.send(String(y_axis_web).c_str(), "YAXIS", millis());
        events.send(String(tx_MODE_web).c_str(), "TX_MODE", millis());

        lastTime = millis();
    }
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