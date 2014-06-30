#ifndef __X64_TEXT_CONSOLE_HPP__
#define __X64_TEXT_CONSOLE_HPP__

#include <arch/general/console.hpp>
#include <cstdint>
#include <macros>

namespace OS {

namespace x64 {

class TextConsole : public Console {
public:
  static void InitGlobal();
  static TextConsole & GetGlobal();
  
  virtual void PrintString(const char * string);
  virtual void SetColor(Color color, bool bright);
  
protected:
  virtual void Initialize();
  virtual DepList GetDependencies();
  
private:
  uint8_t color = 0xa;
  int x = 0, y = 0;
  uint16_t * buffer = (uint16_t *)0xb8000;
  uint64_t mainLock OS_ALIGNED(8) = 0; // @critical
  
  void SetPosition(uint16_t x, uint16_t y);
  void ScrollUp();
};

}

}

#endif
