#include "printing-x64.hpp"
#include "common-x64.hpp"

namespace OS {

static int color = 0xa;
static int x = 0, y = 0;
static uint16_t * buffer = (uint16_t *)(0xb8000);
static uint64_t mainLock OS_ALIGNED(8) = 0;

static void SetPosition(unsigned short x, unsigned short y);
static void ScrollUp();

void InitializePrinting() {
  int i;
  for (i = 0; i < 80 * 25; i++) {
    buffer[i] = 0;
  }
}

void PrintString(const char * string) {
  ScopeLock scope(&mainLock);
  while (*string) {
    unsigned char theChar = *(string++);
    if (theChar == '\n') {
      if (x != 0) {
        y++;
        x = 0;
      }
    } else if (theChar == '\b') {
      if (x == 0) {
        if (y != 0) {
          y--;
          // find the last character on this line
          x = 80 - 1;
          int i, newLoc = (80 * y);
          for (i = 0; i < 80; i++) {
            if (!buffer[newLoc + i]) {
              x = i;
              break;
            }
          }
        }
      } else x--;
      int loc = x + (80 * y);
      buffer[loc] = color << 8;
    } else {
      int loc = x + (80 * y);
      buffer[loc] = (uint16_t)theChar | (color << 8);
      x++;
    }
    if (x >= 80) {
      x = 0;
      y++;
    }
    while (y >= 25) {
      ScrollUp();
      y--;
    }
  }
  SetPosition(x, y);
}

void SetColor(int _color) {
  color = _color;
}

static void SetPosition(unsigned short x, unsigned short y) {
  unsigned short position = (y * 80) + x;
  // tell the VGA index register we are sending the `low` byte
  x64::OutB(0x3D4, 0x0f);
  x64::OutB(0x3D5, (unsigned char)(position & 0xff));
  // and now send the `high` byte
  x64::OutB(0x3D4, 0x0e);
  x64::OutB(0x3D5, (unsigned char)((position >> 8) & 0xff));

  buffer[position] = color << 8;
}

static void ScrollUp() {
  // copy the buffer into itself, one line up
  int i;
  for (i = 0; i < 80 * (25 - 1); i++) {
    buffer[i] = buffer[i + 80];
  }
  
  // clear the bottom line
  for (; i < 80 * 25; i++) {
    buffer[i] = 0;
  }
}

}
