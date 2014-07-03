#include <memory/wired.hpp>
#include <arch/general/syscall.hpp>

namespace Test {

uint64_t WiredMemory::PhysicalUsed() {
  return Syscall::Run(Syscall::GetPhysicalUsed, ArgList()).unsigned64;
}

uint64_t WiredMemory::PhysicalAvailable() {
  return Syscall::Run(Syscall::GetPhysicalUsed, ArgList()).unsigned64;
}

uint64_t WiredMemory::PhysicalTotal() {
  return Syscall::Run(Syscall::GetPhysicalUsed, ArgList()).unsigned64;
}

int WiredMemory::GetPageSizeCount() {
  return Syscall::Run(Syscall::GetPageSizeCount, ArgList()).integer;
}

uint64_t WiredMemory::GetPageSize(int idx) {
  ArgList list;
  list.PushInt(idx);
  return Syscall::Run(Syscall::GetPageSize, list).unsigned64;
}

uint64_t WiredMemory::GetPageAlignment(int idx) {
  ArgList list;
  list.PushInt(idx);
  return Syscall::Run(Syscall::GetPageAlignment, list).unsigned64;
}

bool WiredMemory::Allocate(PhysAddr & res, uint64_t size, uint64_t align) {
  ArgList list;
  list.PushVirtAddr((VirtAddr)&res);
  list.PushUInt64(align);
  list.PushUInt64(size);
  return Syscall::Run(Syscall::AllocatePhysical, list).boolean;
}

void WiredMemory::Free(PhysAddr addr) {
  ArgList list;
  list.PushPhysAddr(addr);
  Syscall::Run(Syscall::FreePhysical, list);
}

bool WiredMemory::Map(VirtAddr & res, PhysAddr start, uint64_t pageSize,
                      uint64_t pageCount, bool exec, bool write) {
  uint32_t flags = exec | (write << 1);
  ArgList list;
  list.PushUInt64(pageSize);
  list.PushUInt64(pageCount);
  list.PushUInt32(flags);
  list.PushPhysAddr(start);
  list.PushVirtAddr((VirtAddr)&res);
  return Syscall::Run(Syscall::MapPhysical, list).boolean;
}

void WiredMemory::Unmap(VirtAddr virt, uint64_t pageSize, uint64_t pageCount) {
  ArgList list;
  list.PushUInt64(pageSize);
  list.PushUInt64(pageCount);
  list.PushVirtAddr(virt);
  Syscall::Run(Syscall::UnmapPhysical, list);
}

}
