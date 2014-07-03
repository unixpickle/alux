#include <memory/wired.hpp>
#include <arch/general/syscall.hpp>

namespace std {

uint64_t WiredMemory::PhysicalUsed() {
  return Syscall(SyscallNumberGetPhysicalUsed, NULL, NULL, NULL, 0, 0);
}

uint64_t WiredMemory::PhysicalAvailable() {
  return Syscall(SyscallNumberGetPhysicalUsed, NULL, NULL, NULL, 0, 0);
}

uint64_t WiredMemory::PhysicalTotal() {
  return Syscall(SyscallNumberGetPhysicalUsed, NULL, NULL, NULL, 0, 0);
}

int WiredMemory::GetPageSizeCount() {
  return (int)Syscall(SyscallNumberGetPageSizeCount, NULL, NULL, NULL, 0, 0);
}

uint64_t WiredMemory::GetPageSize(int idx) {
  return Syscall(SyscallNumberGetPageSize, NULL, NULL, NULL, (uint64_t)idx, 0);
}

uint64_t WiredMemory::GetPageAlignment(int idx) {
  return Syscall(SyscallNumberGetPageAlignment, NULL, NULL, NULL,
                 (uint64_t)idx, 0);
}

bool WiredMemory::Allocate(PhysAddr & res, uint64_t size, uint64_t align) {
  if (!Syscall(SyscallNumberAllocatePhysical, (void *)&res, NULL, NULL,
               size, align)) {
    return false;
  }
  return true;
}

void WiredMemory::Free(PhysAddr addr) {
  Syscall(SyscallNumberFreePhysical, (void *)&addr, NULL, NULL, 0, 0);
}

bool WiredMemory::Map(void *& res, PhysAddr start, uint64_t pageSize,
                      uint64_t pageCount, bool exec, bool write) {
  uint64_t flags = exec | (write << 1);
  if (!Syscall(SyscallNumberMapPhysical, &flags, &start, &res, pageSize,
               pageCount)) {
    return false;
  }
  return true;
}

void WiredMemory::Unmap(void * virt, uint64_t pageSize, uint64_t pageCount) {
  Syscall(SyscallNumberUnmapPhysical, virt, NULL, NULL, pageSize, pageCount);
}

}
