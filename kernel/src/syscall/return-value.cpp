#include <syscall/return-value.hpp>
#include <cstring>

namespace OS {

ReturnValue ReturnValue::NewBool(bool val) {
  ReturnValue v(BooleanType);
  v.value.boolean = val;
  return v;
}

ReturnValue ReturnValue::NewInt(int val) {
  ReturnValue v(IntType);
  v.value.integer = val;
  return v;
}

ReturnValue ReturnValue::NewUInt32(uint32_t val) {
  ReturnValue v(Unsigned32Type);
  v.value.unsigned32 = val;
  return v;
}

ReturnValue ReturnValue::NewUInt64(uint64_t val) {
  ReturnValue v(Unsigned64Type);
  v.value.unsigned64 = val;
  return v;
}

ReturnValue ReturnValue::NewPhysAddr(PhysAddr val) {
  ReturnValue v(PhysicalType);
  v.value.physicalAddr = val;
  return v;
}

ReturnValue ReturnValue::NewVirtAddr(VirtAddr val) {
  ReturnValue v(VirtualType);
  v.value.virtualAddr = val;
  return v;
}

ReturnValue ReturnValue::NewVoid() {
  return ReturnValue(VoidType);
}

ReturnValue::ReturnValue(Type t) : type(t) {
}

ReturnValue::ReturnValue(const ReturnValue & val)
  : type(val.type), value(val.value) {
}

ReturnValue & ReturnValue::operator=(const ReturnValue & val) {
  type = val.type;
  memcpy(&value, &val.value, sizeof(value));
  return *this;
}

}
