#include <arch/x64/smp/cpu-list.hpp>
#include <arch/x64/segments/gdt.hpp>
#include <arch/general/failure.hpp>
#include <utilities/critical.hpp>
#include <cassert>
#include <new>

namespace OS {

namespace x64 {

static CPUList globalList;

void CPUList::Initialize(int capacity) {
  new(&globalList) CPUList(capacity);
}

CPUList & CPUList::GetGlobal() {
  return globalList;
}

CPUList::CPUList() {
  Panic("CPUList::CPUList() - method for compiler satisfaction only");
}

CPUList::CPUList(int _capacity) : capacity(_capacity), count(0) {
  buffer = new uint8_t[sizeof(CPU) * capacity];
}

int CPUList::GetCount() {
  return count;
}

CPU & CPUList::AddEntry(uint32_t apicId) {
  assert(count < capacity);
  void * addr = (void *)(buffer + sizeof(CPU) * count);
  new(addr) CPU(apicId);
  return (*this)[count++];
}

CPU & CPUList::operator[](int idx) {
  return *((CPU *)buffer + idx);
}

int CPUList::GetIndex(CPU & entry) {
  uintptr_t ptr = (uintptr_t)&entry;
  uintptr_t diff = ptr - (uintptr_t)buffer;
  return (int)(diff / sizeof(CPU));
}

CPU & CPUList::GetCurrent() {
  AssertCritical();
  int index = GDT::GetGlobal().GetTSSIndex();
  assert(index >= 0 && index < count);
  return (*this)[index];
}

}

}
