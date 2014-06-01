#ifndef __X64_STARTUP_CODE_HPP__
#define __X64_STARTUP_CODE_HPP__

#include <cstddef>
#include <cstdint>

namespace OS {

namespace x64 {

class StartupCode {
private:
  void * backup;
  size_t codeSize;

public:
  StartupCode(); // @noncritical
  ~StartupCode(); // @noncritical
  
  void UpdateStack(uint64_t pml4, uint64_t funcAddr);
  uint8_t GetStartupVector();
};

}

}

#endif
