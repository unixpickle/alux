#ifndef __SHARED_SYSCALL_RETURN_VALUE_HPP__
#define __SHARED_SYSCALL_RETURN_VALUE_HPP__

#include <cstdint>
#include <arch-specific/types.hpp>

namespace OS {

class ReturnValue {
public:
  enum Type {
    BooleanType,
    IntType,
    Unsigned32Type,
    Unsigned64Type,
    PhysicalType,
    VirtualType,
    VoidType
  };
  
  static ReturnValue NewBool(bool val);
  static ReturnValue NewInt(int val);
  static ReturnValue NewUInt32(uint32_t val);
  static ReturnValue NewUInt64(uint64_t val);
  static ReturnValue NewPhysAddr(PhysAddr val);
  static ReturnValue NewVirtAddr(VirtAddr val);
  static ReturnValue NewVoid();
  
  ReturnValue(Type t);
  ReturnValue(const ReturnValue & val);
  ReturnValue & operator=(const ReturnValue & val);
  
  Type type;
  
  union {
    bool boolean;
    int integer;
    uint32_t unsigned32;
    uint64_t unsigned64;
    PhysAddr physicalAddr;
    VirtAddr virtualAddr;
  } value;
};

}

#endif
