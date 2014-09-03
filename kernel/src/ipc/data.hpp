#ifndef __ALUX_IPC_DATA_HPP__
#define __ALUX_IPC_DATA_HPP__

#include <anarch/types>
#include <anarch/stdint>
#include <anarch/stddef>

namespace Alux {

union Datum {
  size_t vSize;
  PhysSize pSize;
  
  VirtAddr vAddr;
  PhysAddr pAddr;
  
  int integer;
  uint64_t integer64;
};

struct Data {
  Datum data[2];
  
  bool connected = true;
  
  /**
   * Create a piece of data whose first datum contains a specified integer
   * value.
   * @ambicritical
   */
  inline static Data Integer(int value, bool connected = true) {
    Data result;
    result.data[0].integer = value;
    result.connected = connected;
    return result;
  }
  
  /**
   * Create a piece of data with an optional connected status.
   * @ambicritical
   */
  inline static Data Empty(bool connected = true) {
    Data result;
    result.connected = connected;
    return result;
  }
};

}

#endif
