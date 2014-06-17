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
  static void InitGlobal(); // @noncritical
  static CPUList & GetGlobal(); // @noncritical
  
  virtual void Initialize();
  virtual DepList GetDependencies();
  virtual DepList GetSuperDependencies();

  virtual int GetCount(); // @ambicritical, unsynchronized
  CPU & AddEntry(uint32_t apicId); // @noncritical, unsynchronized

  virtual CPU & operator[](int idx); // @ambicritical, unsynchronized

  int GetIndex(CPU & entry);
  virtual int IndexOf(HardwareThread & th);
};

}

}

#endif
