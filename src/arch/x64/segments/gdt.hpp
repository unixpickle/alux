#ifndef __X64_GDT_HPP__
#define __X64_GDT_HPP__

#include <arch/x64/segments/tss.hpp>
#include <cstddef>
#include <module/module.hpp>

namespace OS {

namespace x64 {

class GDTPointer {
public:
  uint16_t limit;
  uint64_t start;
  
  static GDTPointer GetCurrent(); // uses SGDT, @critical
} OS_PACKED;

class GDT : public Module {
public:
  static void InitGlobal();
  static GDT & GetGlobal();
  
  /**
   * Run LGDT to load this GDT.
   * @critical
   */
  void Set();
  
  /**
   * Add a new TSS descriptor and return its selector
   * @ambicritical - not synchronized
   */
  uint16_t AddTSS(TSS * aTSS);
  
  /**
   * Get the pointer to a TSS object given a TSS selector in this GDT.
   * @ambicritical - not synchronized
   */
  TSS * GetTSS(uint16_t sel);
  
  /**
   * Get the first TSS selector.
   * @ambicritical
   */
  uint16_t GetFirstTSS();

protected:
  virtual void Initialize();
  virtual DepList GetDependencies();

private:
  uint8_t * buffer;
  size_t offset;
  size_t initOffset;
};

}

}

#endif
