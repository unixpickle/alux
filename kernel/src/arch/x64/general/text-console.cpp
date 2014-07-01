#include <arch/x64/general/text-console.hpp>
#include <arch/x64/common.hpp>
#include <new>
#include <lock>
#include <macros>
#include <cstdint>
#include <critical>

namespace OS {

Console & Console::GetGlobal() {
  return x64::TextConsole::GetGlobal();
}

namespace x64 {

static TextConsole gConsole;

void TextConsole::InitGlobal() {
  new(&gConsole) TextConsole();
}

TextConsole & TextConsole::GetGlobal() {
  return gConsole;
}

void TextConsole::Initialize() {
  for (int i = 0; i < 80 * 25; i++) {
    buffer[i] = 0;
  }
}

DepList TextConsole::GetDependencies() {
  return DepList();
}

void TextConsole::PrintString(const char * string) {
  ScopeCritical critical;
  ScopeCriticalLock scope(&mainLock);
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

void TextConsole::SetColor(TextConsole::Color _color, bool bright) {
  ScopeCritical critical;
  ScopeCriticalLock scope(&mainLock);
  color = (uint8_t)_color | (bright ? 8 : 0);
}

void TextConsole::SetPosition(uint16_t _x, uint16_t _y) {
  uint16_t position = (_y * 80) + _x;
  // tell the VGA index register we are sending the `low` byte
  x64::OutB(0x3D4, 0x0f);
  x64::OutB(0x3D5, (unsigned char)(position & 0xff));
  // and now send the `high` byte
  x64::OutB(0x3D4, 0x0e);
  x64::OutB(0x3D5, (unsigned char)((position >> 8) & 0xff));

  buffer[position] = (uint16_t)color << 8;
}

void TextConsole::ScrollUp() {
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

}
