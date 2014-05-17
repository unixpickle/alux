#ifndef __PLATFORM_PRINTING_H__
#define __PLATFORM_PRINTING_H__

#include <utilities/lock.hpp>

namespace OS {
  
typedef enum {
  PrintColorBlack = 0,
  PrintColorBlue = 1,
  PrintColorGreen = 2,
  PrintColorCyan = 3,
  PrintColorRed = 4,
  PrintColorMagenta = 5,
  PrintColorBrown = 6,
  PrintColorLightGray = 7,
  PrintColorBrightMask = 8
} PrintColor;

void PrintString(const char * string);
void SetColor(int color);

}

#endif
