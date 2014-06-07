#ifndef __X64_CPU_LIST_HPP__
#define __X64_CPU_LIST_HPP__

#include <arch/x64/smp/cpu.hpp>

namespace OS {

namespace x64 {

class CPUList {
private:
  int capacity;
  int count;
  uint8_t * buffer;

public:
  static void Initialize(int capacity); // @noncritical
  static CPUList & GetGlobal(); // @noncritical

  CPUList(); // do not initialize CPUList yourself
  CPUList(int capacity); // do not initialize CPUList yourself

  int GetCount(); // @ambicritical, unsynchronized
  CPU & AddEntry(uint32_t apicId); // @ambicritical, unsynchronized

  CPU & operator[](int idx); // @ambicritical, unsynchronized
  CPU & GetCurrent(); // @critical
  int GetIndex(CPU & entry);
};

}

}

#endif
