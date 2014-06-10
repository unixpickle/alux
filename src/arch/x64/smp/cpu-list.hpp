#ifndef __X64_CPU_LIST_HPP__
#define __X64_CPU_LIST_HPP__

#include <arch/x64/smp/cpu.hpp>
#include <arch/general/hardware-thread-list.hpp>

namespace OS {

namespace x64 {

class CPUList : public HardwareThreadList {
private:
  int capacity;
  int count;
  uint8_t * buffer;

public:
  static void Initialize(int capacity); // @noncritical
  static CPUList & GetGlobal(); // @noncritical

  CPUList(); // do not initialize CPUList yourself
  CPUList(int capacity); // do not initialize CPUList yourself

  virtual int GetCount(); // @ambicritical, unsynchronized
  CPU & AddEntry(uint32_t apicId); // @ambicritical, unsynchronized

  virtual CPU & operator[](int idx); // @ambicritical, unsynchronized
  CPU & GetCurrent(); // @critical
  int GetIndex(CPU & entry);
};

}

}

#endif
