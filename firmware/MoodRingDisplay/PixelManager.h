#include <Adafruit_NeoPixel.h>
#include "LinkedList.h"

struct Instruction {
  unsigned char R;
  unsigned char G;
  unsigned char B;
  int ticks;
};

class PixelManager {
  public:
    PixelManager(int id);
    void readInstruction(char *instruction);
    void tick(Adafruit_NeoPixel *pixels);

    int pixelId;
    int tickCounter;
    int curNode;

    char curR;
    char curG;
    char curB;
    
    LinkedList<struct Instruction> *nodes;
};


