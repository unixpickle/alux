#ifndef __X64_GDT_HPP__
#define __X64_GDT_HPP__

#include <arch/x64/segments/tss.hpp>
#include <cstddef>

namespace OS {

namespace x64 {

class GDTPointer {
public:
  uint16_t limit;
  uint64_t start;
  
  static GDTPointer GetCurrent(); // uses SGDT, @critical
} OS_PACKED;

class GDT {
public:
  static void Initialize();
  static GDT & GetGlobal();

  GDT(); // do not call this directly
  
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
  
  /**
   * Get the TSS index by looking at the current task register.
   * @critical
   */
  int GetTSSIndex();
  
  /**
   * Get the TSS index of a selector.
   * @ambicritical
   */
  int GetTSSIndex(uint16_t sel);

private:
  uint8_t * buffer;
  size_t offset;
  size_t initOffset;
};

}

}

#endif
