#include <string.h>
#include <ESP8266WiFi.h>          //ESP8266 Core WiFi Library (you most likely already have this in your sketch)
#include <ESP8266WiFiMulti.h>

#include <Adafruit_NeoPixel.h>

#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>     //Local WebServer used to serve the configuration portal
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic
#include <WebSocketsClient.h>

ESP8266WiFiMulti WiFiMulti;
WebSocketsClient webSocket;

//#define DEVMODE
#define PIN 12

uint8_t colors[255][3] = {
};

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(1, PIN, NEO_GRB + NEO_KHZ800);
String macAddress = WiFi.macAddress();

void setup() {

#ifdef DEVMODE
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println("Setup complete.");
#endif

  // put your setup code here, to run once:
  pixels.begin();
  pixels.setPixelColor(0, pixels.Color(25, 25, 25));
  pixels.show();

  WiFiManager wifiManager;
  wifiManager.autoConnect("AdventMoodRing");

  webSocket.begin("adventmoodring.herokuapp.com", 80);
//    webSocket.begin("192.168.112.135", 3000);
  webSocket.onEvent(webSocketEvent);

#ifdef DEVMODE
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("MAC: ");
  Serial.println(macAddress);
  Serial.println("Setup complete.");
#endif
}

bool startsWith(const char *pre, const char *str)
{
    size_t lenpre = strlen(pre),
           lenstr = strlen(str);
    return lenstr < lenpre ? false : strncmp(pre, str, lenpre) == 0;
}

bool connected = false;
bool setting = false;

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
    switch(type) {
        case WStype_DISCONNECTED:
        #ifdef DEVMODE
            Serial.printf("[WSc] Disconnected!\n");
        #endif
            delay(5000);
            connected = false;
            break;
        case WStype_CONNECTED:
            {
        #ifdef DEVMODE
              Serial.printf("[WSc] Connected to url: %s\n",  payload);
        #endif
              connected = true;
              // send message to server when Connected
              webSocket.sendTXT(String("KNOB ") + macAddress);
            }
            break;
        case WStype_TEXT:
        #ifdef DEVMODE
            Serial.printf("[WSc] get text: %s\n", payload);
        #endif
            if (startsWith("colorTable", (const char *) payload)) {
              readColorTable((char *)payload);
            } else if (payload[0] == '!' && !setting) {
              if (payload[1]) {
                uint8_t spot = (uint8_t) atoi(((char*)payload) + 1);
                pixels.setPixelColor(0, pixels.Color(colors[spot][0],colors[spot][1],colors[spot][2]));
              } else {
                pixels.setPixelColor(0, pixels.Color(0, 0, 0));                
              }
              pixels.show();
            }
            // send message to server
            // webSocket.sendTXT("message here");
            // readInstructions((char *) payload);
            break;
        case WStype_BIN:
        #ifdef DEVMODE
            Serial.printf("[WSc] get binary length: %u\n", length);
        #endif
            // send data to server
            // webSocket.sendBIN(payload, length);
            break;
    }

}

uint8_t hexToI(char c) {
  if (c >= '0' && c <= '9') {
    return c - '0';  
  }
  if (c >= 'a' && c <= 'f') {
    return (c - 'a') + 10;
  }
  if (c >= 'A' && c <= 'F') {
    return (c - 'A') + 10;
  }
}

void readColorTable(char *payload) {
  int spot = 0;
  char *part = strtok(payload, "\n");
  for (int spot = 0; spot < 256 && part; spot ++) {
    char *line = strtok(NULL, "\n");
    if (strlen(line) == 6) {
      colors[spot][0] = (hexToI(line[0]) << 4 | hexToI(line[1]));
      colors[spot][1] = (hexToI(line[2]) << 4 | hexToI(line[3]));
      colors[spot][2] = (hexToI(line[4]) << 4 | hexToI(line[5]));
    }
  }
}

int existingValue;
int unchangedCount;

void loop() {
  // Normalize the input value to one byte
  int val = analogRead(A0);
  if (val > 1000) {
    val = 1000; // for some reason min() isn't available
  }
  uint8_t normal = (uint8_t) round((255.0 * val / 1000.0));
  if (abs(existingValue-normal) > 10) {
    unchangedCount = 0;
    setting = true;
    pixels.setPixelColor(0, pixels.Color(colors[normal][0],colors[normal][1],colors[normal][2]));
    pixels.show();
    existingValue = normal;
    if (connected) {
      char ix[16];
      sprintf(ix, "K %d", normal);
      webSocket.sendTXT(ix);
    }
  } else {
    unchangedCount ++;
    if (unchangedCount > 20) {
      setting = false;
    }
    if (unchangedCount == 50) {
      if (connected) {
        char ix[16];
        sprintf(ix, "S %d", normal);
        webSocket.sendTXT(ix);
      }
    }
    if (unchangedCount > 100 && unchangedCount <= 110) {
      uint8_t fadeR = (colors[existingValue][0] * (110 - unchangedCount)) / 10;
      uint8_t fadeG = (colors[existingValue][1] * (110 - unchangedCount)) / 10;
      uint8_t fadeB = (colors[existingValue][2] * (110 - unchangedCount)) / 10;
      pixels.setPixelColor(0, pixels.Color(fadeR, fadeG, fadeB));      
      pixels.show();
    }
  }

  webSocket.loop();
  delay(50);
}
