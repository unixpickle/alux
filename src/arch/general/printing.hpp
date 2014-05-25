#ifndef __GENERAL_PRINTING_HPP__
#define __GENERAL_PRINTING_HPP__

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

/**
 * @noncritical
 */
void PrintString(const char * string);

/**
 * @noncritical
 */
void SetColor(int color);

}

#endif
