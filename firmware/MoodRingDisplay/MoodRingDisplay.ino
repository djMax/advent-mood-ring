#include <string.h>
#include <ESP8266WiFi.h>          //ESP8266 Core WiFi Library (you most likely already have this in your sketch)
#include <ESP8266WiFiMulti.h>

#include <Adafruit_NeoPixel.h>

#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>     //Local WebServer used to serve the configuration portal
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic

#include <WebSocketsClient.h>
#include <Hash.h>
#include "PixelManager.h"

ESP8266WiFiMulti WiFiMulti;
WebSocketsClient webSocket;

// #define DEVMODE
#define PIN 12
#define NUMPIXELS 24

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
PixelManager manager[24] {
  { 0 }, { 1 }, { 2 }, { 3 }, { 4 }, { 5 }, { 6 }, { 7 }, { 8 }, { 9 },
  { 10 }, { 11 }, { 12 }, { 13 }, { 14 }, { 15 }, { 16 }, { 17 }, { 18 }, { 19 },
  { 20 }, { 21 }, { 22 }, { 23 }
};

int delayval = 500;

void setup() {
  pixels.begin();

  for (int p = 0; p < NUMPIXELS; p++) {
    pixels.setPixelColor(p, pixels.Color(25, 25, 25));
    pixels.show();
    delay(100);
  }
  //delay(5000);
  for (int p = 0; p < NUMPIXELS; p++) {
    delay(100);
    pixels.setPixelColor(p, pixels.Color(0, 0, 0));
    pixels.show();
  }

#ifdef DEVMODE
  Serial.begin(115200);
  Serial.setDebugOutput(true);
#endif
  WiFiManager wifiManager;
  wifiManager.autoConnect("AdventMoodRing");

#ifdef DEVMODE
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
#endif

  webSocket.begin("adventmoodring.herokuapp.com", 80);
//  webSocket.begin("192.168.112.135", 3000);
  webSocket.onEvent(webSocketEvent);
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
    switch(type) {
        case WStype_DISCONNECTED:
#ifdef DEVMODE
            Serial.printf("[WSc] Disconnected!\n");
#endif
            delay(5000);
            break;
        case WStype_CONNECTED:
            {
#ifdef DEVMODE
              Serial.printf("[WSc] Connected to url: %s\n",  payload);
#endif
              // send message to server when Connected
              webSocket.sendTXT("DISPLAY");
            }
            break;
        case WStype_TEXT:
#ifdef DEVMODE
            Serial.printf("[WSc] get text: %s\n", payload);
#endif
            // send message to server
            // webSocket.sendTXT("message here");
            readInstructions((char *) payload);
            break;
        case WStype_BIN:
#ifdef DEVMODE
            Serial.printf("[WSc] get binary length: %u\n", length);
            hexdump(payload, length);
#endif
            // send data to server
            // webSocket.sendBIN(payload, length);
            break;
    }

}

void readInstructions(char *instructions) {
  char *end_str;
  char *instruction = strtok_r(instructions, "\n", &end_str);
  while (instruction != NULL) {
#ifdef DEVMODE
    Serial.printf("[Mood] Read instruction %s\n", instruction);
#endif
    int pixel = atoi(instruction);
    instruction = strtok_r(NULL, "\n", &end_str);
    if (pixel < NUMPIXELS) {
      manager[pixel].readInstruction(instruction);
    }
    instruction = strtok_r(NULL, "\n", &end_str);
  }
}

void loop() {
  for (int i = 0; i < NUMPIXELS; i++) {
    manager[i].tick(&pixels);
  }
  pixels.show();
  webSocket.loop();
  delay(50);
}

