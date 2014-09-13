#ifndef __ALUX_MESSAGE_HPP__
#define __ALUX_MESSAGE_HPP__

#include <anarch/types>
#include <anarch/stddef>

namespace Alux {

class Message {
public:
  static const uint8_t TypeOpened = 0;
  static const uint8_t TypeData = 1;
  static const uint8_t TypeClosed = 2;
  
  union {
    uint8_t integer8;
    uint16_t integer16;
    uint32_t integer32;
    uint64_t integer64;
    PhysAddr physAddr;
    VirtAddr virtAddr;
    PhysSize physSize;
    size_t virtSize;
  } fields[5];
  
  uint8_t type;
  
  inline static Message Opened() {
    Message m;
    m.type = TypeOpened;
    return m;
  }
  
  inline static Message Closed() {
    Message m;
    m.type = TypeClosed;
    return m;
  }
};

}

#endif
