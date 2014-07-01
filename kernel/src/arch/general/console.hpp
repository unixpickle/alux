#ifndef __GENERAL_CONSOLE_HPP__
#define __GENERAL_CONSOLE_HPP__

#include <module/module.hpp>

namespace OS {

class Console : public Module {
public:
  static Console & GetGlobal();
  
  typedef enum {
    ColorBlack = 0,
    ColorBlue = 1,
    ColorGreen = 2,
    ColorCyan = 3,
    ColorRed = 4,
    ColorMagenta = 5,
    ColorBrown = 6,
    ColorLightGray = 7
  } Color;

  /**
   * @ambicritical
   */
  virtual void PrintString(const char * string) = 0;

  /**
   * @ambicritical
   */
  virtual void SetColor(Color color, bool bright) = 0;
};

}

#endif
