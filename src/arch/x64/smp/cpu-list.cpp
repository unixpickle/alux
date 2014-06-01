#include <arch/x64/smp/cpu-list.hpp>
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

CPU * CPUList::GetCurrent() {
  AssertCritical();
  // TODO: here, use the GDT to get the current TSS index
  return NULL;
}

}

}
