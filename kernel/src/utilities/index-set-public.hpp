#include <lock>
#include <critical>
#include <new>

#define _INDEX_SET_TEMPLATE_ template<int Max, size_t SlabCount>
#define _INDEX_SET_CLASS_ IndexSet<Max, SlabCount>

namespace OS {

_INDEX_SET_TEMPLATE_
uint64_t _INDEX_SET_CLASS_::initLock = 0;

_INDEX_SET_TEMPLATE_
bool _INDEX_SET_CLASS_::slabInitialized = false;

_INDEX_SET_TEMPLATE_
Slab<typename _INDEX_SET_CLASS_::Node, SlabCount> _INDEX_SET_CLASS_::slab;

_INDEX_SET_TEMPLATE_
_INDEX_SET_CLASS_::IndexSet() {
  AssertNoncritical();
  firstNode = AllocNode();
  firstNode->next = NULL;
  firstNode->count = 0;
}

_INDEX_SET_TEMPLATE_
_INDEX_SET_CLASS_::~IndexSet() {
  AssertNoncritical();
  Node * node = firstNode;
  while (node) {
    Node * next = node->next;
    FreeNode(node);
    node = next;
  }
}

_INDEX_SET_TEMPLATE_
uint64_t _INDEX_SET_CLASS_::Pop() {
  AssertNoncritical();
  ScopeLock scope(&lock);
  if (firstNode->count) {
    return firstNode->indexes[--firstNode->count];
  }
  if (firstNode->next) {
    Node * rem = firstNode;
    firstNode = firstNode->next;
    FreeNode(rem);
    
    // don't hold the lock while we do this
    LockRelease(&lock);
    uint64_t result = Pop();
    LockHold(&lock);
    return result;
  }
  return numUsed++;
}

_INDEX_SET_TEMPLATE_
void _INDEX_SET_CLASS_::Push(uint64_t idx) {
  AssertNoncritical();
  ScopeLock scope(&lock);
  if (firstNode->count < Max) {
    firstNode->indexes[firstNode->count++] = idx;
    return;
  }
  Node * node = AllocNode();
  node->next = firstNode;
  node->count = 1;
  node->indexes[0] = idx;
  firstNode = node;
}

_INDEX_SET_TEMPLATE_
typename _INDEX_SET_CLASS_::Node * _INDEX_SET_CLASS_::AllocNode() {
  AssertNoncritical();
  {
    ScopeLock scope(&initLock);
    if (!slabInitialized) {
      slabInitialized = true;
      new(&slab) Slab<Node, SlabCount>();
    }
  }
  return slab.New();
}

_INDEX_SET_TEMPLATE_
void _INDEX_SET_CLASS_::FreeNode(_INDEX_SET_CLASS_::Node * n) {
  AssertNoncritical();
  slab.Delete(n);
}

}

#undef _INDEX_SET_TEMPLATE_
#undef _INDEX_SET_CLASS_
