#include "cpu-list-x64.hpp"

namespace OS {
namespace x64 {

static CPUList list;

void CPUList::Initialize(int capacity) {
  new(&list) CPUList(capacity);
}

CPUList & CPUList::GetGlobal() {
  return list;
}

CPUList::CPUList(int _capacity) : capacity(_capacity), count(0) {
  buffer = new uint8_t[sizeof(CPU) * capacity];
}

int CPUList::GetCount() {
  return count;
}

int CPUList::ConstructEntry(uint32_t apicId) {
  assert(capacity > count);
  void * destPtr = (void *)(buffer + sizeof(CPU) * count);
  new(destPtr) CPU(apicId);
  return count++;
}

CPU & CPUList::operator[](int idx) {
  assert(idx >= 0 && idx < count);
  CPU * ptr = (CPU *)buffer;
  return ptr[idx];
}

CPU & CPUList::GetCurrent() {
  int index = GDT::GetGlobal().GetTSSIndex();
  return (*this)[index];
}

}
}
