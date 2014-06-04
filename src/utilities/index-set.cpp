#include <utilities/index-set.hpp>
#include <utilities/lock.hpp>
#include <memory/slab.hpp>

extern "C" {
#include <anlock.h>
}

namespace OS {

static uint64_t slabInitLock OS_ALIGNED(8);
static Slab<IndexSet::Node, 0x10> slab;
static bool initialized;

IndexSet::IndexSet() : numUsed(0) {
  anlock_lock(&slabInitLock);
  if (!initialized) {
    new(&slab) Slab<IndexSet::Node, 0x10>();
  }
  anlock_unlock(&slabInitLock);
  
  firstNode = slab.New();
  firstNode->next = NULL; // this is redundant, but it's for show
  firstNode->count = 0;
}

IndexSet::~IndexSet() {
  Node * node = firstNode;
  while (node) {
    Node * next = node->next;
    slab.Delete(node);
    node = next;
  }
}

uint64_t IndexSet::Pop() {
  ScopeLock scope(&lock);
  if (firstNode->count) {
    return firstNode->indexes[--firstNode->count];
  }
  if (firstNode->next) {
    Node * rem = firstNode;
    firstNode = firstNode->next;
    slab.Delete(rem);
    return Pop();
  }
  return numUsed++;
}

void IndexSet::Push(uint64_t idx) {
  ScopeLock scope(&lock);
  if (firstNode->count < Node::Max) {
    firstNode->indexes[firstNode->count++] = idx;
    return;
  }
  Node * node = slab.New();
  node->next = firstNode;
  node->count = 1;
  node->indexes[0] = idx;
  firstNode = node;
}

}
