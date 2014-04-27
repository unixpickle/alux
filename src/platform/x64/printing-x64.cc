/*
 * Copyright (c) 2014, Alex Nichol and Alux contributors.
 * All rights reserved.

 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <platform/printing.h>
#include "common-x64.h"

namespace OS {

static int color = 0xa;
static int x = 0, y = 0;
static uint16_t * buffer = (uint16_t *)(0xb8000);
static uint64_t mainLock = 0;

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
      y++;
      x = 0;
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
  OutB(0x3D4, 0x0f);
  OutB(0x3D5, (unsigned char)(position & 0xff));
  // and now send the `high` byte
  OutB(0x3D4, 0x0e);
  OutB(0x3D5, (unsigned char)((position >> 8) & 0xff));

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