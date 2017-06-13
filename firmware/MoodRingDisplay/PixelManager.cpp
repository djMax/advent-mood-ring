#include <ESP8266WiFi.h>
#include <string.h>

#include "PixelManager.h"

PixelManager::PixelManager(int id) {
  this->pixelId = id;
  this->tickCounter = 0;
  this->curNode = 0;
}

void PixelManager::readInstruction(char *instruction) {
  LinkedList<struct Instruction> *newInstructions = new LinkedList<struct Instruction>();
  
  char *end_str;
  char *part = strtok_r(instruction, ",", &end_str);
  while (part != NULL) {
    struct Instruction parsed; 
    for (int i = 0; i < 4 && part != NULL; i++) {
      switch (i) {
        case 0:
          parsed.R = atoi(part);
          break;
        case 1:
          parsed.G = atoi(part);
          break;
        case 2:
          parsed.B = atoi(part);
          break;
        case 3:
          parsed.ticks = atoi(part);
          break;
      }
      part = strtok_r(NULL, ",", &end_str);
    }
    newInstructions->add(parsed);
    // Serial.printf("[Mood] %u show %u,%u,%u for %u\n", this->pixelId, parsed.R, parsed.G, parsed.B, parsed.ticks);
  }

  if (this->nodes) {
    delete this->nodes;
  }
  this->tickCounter = 0;
  this->curNode = 0;
  this->nodes = newInstructions;
}

void PixelManager::tick(Adafruit_NeoPixel *pixels) {
  if (this->nodes != NULL) {
    struct Instruction current = this->nodes->get(this->curNode);
    int nextNode = (this->curNode + 1) % this->nodes->size();

    int slopeR, slopeG, slopeB;

    if (nextNode == this->curNode) {
      slopeR = current.R;
      slopeG = current.G;
      slopeB = current.B;
    } else {
      struct Instruction next = this->nodes->get(nextNode);
      this->tickCounter = this->tickCounter + 1;
      if (this->tickCounter >= current.ticks) {
        this->tickCounter = -1;
        this->curNode = nextNode;
        this->tick(pixels);
        return;
      }
      float r = this->tickCounter * (next.R - current.R);
      float g = this->tickCounter * (next.G - current.G);
      float b = this->tickCounter * (next.B - current.B);
      
      slopeR = current.R + (int) (r / current.ticks);
      slopeG = current.G + (int) (g / current.ticks);
      slopeB = current.B + (int) (b / current.ticks);
    }
    
    if (this->curR != slopeR || this->curG != slopeG || this->curB != slopeB) {
      pixels->setPixelColor(this->pixelId, pixels->Color(slopeR, slopeG, slopeB));
      this->curR = slopeR;
      this->curG = slopeG;
      this->curB = slopeB;
    }
  }
}

