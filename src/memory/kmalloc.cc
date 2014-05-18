#include <memory/kmalloc.hpp>
#include <scheduler/scheduler-init.hpp>

namespace OS {

static MallocRegion * firstRegion = NULL;
static uint64_t mallocLock OS_ALIGNED(8) = 0;
static size_t chunkAlignment = 0;

static void * AllocatePermanentRegion(size_t * realSize);
static void GenerateNewRegion();

void InitializeMalloc() {
  // calculate the proper chunk alignment to use with 2MB
  for (int i = 0; i < KernMap::GetNumPageSizes(); i++) {
    size_t s = KernMap::GetPageSize(i);
    if (s <= 0x200000) {
      chunkAlignment = KernMap::GetPageAlignment(i);
    }
  }
}

void * Malloc(size_t size) {
  ScopeLock scope(&mallocLock);
  AssertNoncritical();
  assert(size <= 0x100000);
  
  MallocRegion * reg = firstRegion;
  while (reg) {
    void * ptr = reg->AllocBuf(size);
    if (ptr) return ptr;
    reg = reg->next;
  }
  
  GenerateNewRegion();
  return firstRegion->AllocBuf(size);
}

void Free(void * ptr) {
  ScopeLock scope(&mallocLock);
  AssertNoncritical();
  MallocRegion * reg = firstRegion;
  while (reg) {
    if (reg->OwnsPointer(ptr)) {
      reg->FreeBuf(ptr);
      return;
    }
    reg = reg->next;
  }
  cerr << "attempt to free unowned pointer " << (uintptr_t)ptr << endl;
  Panic("Free(): fatal warning!");
}

/***********
 * Private *
 ***********/

static void * AllocatePermanentRegion(size_t * realSize) {
  PhysAddr addr;
  bool res = PhysicalAlign(0x200000, chunkAlignment, addr, realSize);
  assert(res);
  
  // map it in
  VirtAddr vAddr;
  bool result = KernMap::Map(addr, *realSize, vAddr);
  assert(result);
  
  // use our new chunk
  return (void *)vAddr;
}

static void GenerateNewRegion() {
  size_t realSize;
  void * reg = AllocatePermanentRegion(&realSize);
  MallocRegion * last = firstRegion;
  firstRegion = new(reg) MallocRegion(reg, realSize, sizeof(MallocRegion));
  firstRegion->next = last;
}

}
