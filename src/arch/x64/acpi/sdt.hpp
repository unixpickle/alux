#ifndef __X64_SDT_HPP__
#define __X64_SDT_HPP__

#include <common>
#include <cstdint>

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
public:
  static void Initialize();
  static SDT & GetGlobal();

  virtual int GetCount() = 0;
  virtual PhysAddr GetTable(int i) = 0;
};

class RSDT : public SDT {
private:
  void * mappedAddr;

public:
  RSDT(PhysAddr phys);
  virtual int GetCount();
  virtual PhysAddr GetTable(int i);
};

class XSDT : public SDT {
private:
  void * mappedAddr;

public:
  XSDT(PhysAddr phys);
  virtual int GetCount();
  virtual PhysAddr GetTable(int i);
};

}

}

#endif

