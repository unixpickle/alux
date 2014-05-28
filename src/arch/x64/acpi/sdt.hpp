#ifndef __X64_SDT_HPP__
#define __X64_SDT_HPP__

#include <common>
#include <cstdint>
#include <memory/easy-map.hpp>

namespace OS {

namespace x64 {

class RSDP {
public:
  uint64_t signature;
  uint8_t checksum;
  char oemid[6];
  uint8_t revision;
  uint32_t ptrRSDT;
  uint32_t length;
  uint64_t ptrXSDT;
  uint8_t xChecksum;
  char reserved[3];
} OS_PACKED;

class SDT {
protected:
  EasyMap * map;
  size_t tableSize;
  
public:
  static void Initialize();
  static SDT & GetGlobal();

  SDT(PhysAddr phys);
  ~SDT();
  virtual int GetCount() = 0; // @noncritical
  virtual PhysAddr GetTable(int i) = 0; // @noncritical
  
  /**
   * Find a table with a given 4 letter name.
   * @param A pointer to some 4-character name
   * @return -1 on not found, otherwise an index.
   * @noncritical
   */
  int FindTable(const char * name);
};

class RSDT : public SDT {
public:
  RSDT(PhysAddr phys) : SDT(phys) {}
  virtual int GetCount();
  virtual PhysAddr GetTable(int i);
};

class XSDT : public SDT {
public:
  XSDT(PhysAddr phys) : SDT(phys) {}
  virtual int GetCount();
  virtual PhysAddr GetTable(int i);
};

}

}

#endif
