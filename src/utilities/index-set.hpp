#ifndef __UTILITIES_INDEX_SET_HPP__
#define __UTILITIES_INDEX_SET_HPP__

#include <memory/slab.hpp>
#include <cstdint>
#include <macros>

namespace OS {

template<int Max, size_t SlabCount = 0x10>
class IndexSet {
public:
  class Node {
  public:
    Node * next;
    int count;
    uint64_t indexes[Max];
  };

  IndexSet(); // @noncritical
  ~IndexSet(); // @noncritical
  
  uint64_t Pop(); // @noncritical
  void Push(uint64_t); // @noncritical

private:
  uint64_t lock OS_ALIGNED(8);
  Node * firstNode;
  uint64_t numUsed;
  
  static uint64_t initLock OS_ALIGNED(8);
  static bool slabInitialized;
  static Slab<Node, SlabCount> slab;
  
  static Node * AllocNode();
  static void FreeNode(Node * n);
};

}

#include <utilities/index-set-public.hpp>

#endif
