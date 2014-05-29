#ifndef __X64_CPU_LIST_HPP__
#define __X64_CPU_LIST_HPP__

#include <cstdint>

namespace OS {

namespace x64 {

class CPUList {
private:
  int capacity;
  int count;
  uint8_t * buffer;

public:
  static void Initialize(int capacity);
  static CPUList & GetGlobal();

  CPUList(); // do not initialize CPUList yourself
  CPUList(int capacity); // do not initialize CPUList yourself

  int GetCount();
  CPU & AddEntry(uint32_t apicId);

  CPU & operator[](int idx);
  CPU * GetCurrent();
};

}

}

#endif
