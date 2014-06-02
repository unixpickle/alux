#ifndef __UTILITIES_INDEX_SET_HPP__
#define __UTILITIES_INDEX_SET_HPP__

#include <cstdint>

namespace OS {

class IndexSet {
public:
  class Node {
    static const int Max = 0xfe;
    
    Node * next;
    int count;
    uint64_t indexes[0xfe];
  };

  IndexSet(); // @noncritical
  ~IndexSet(); // @noncritical
  
  uint64_t Pop(); // @noncritical
  void Push(uint64_t); // @noncritical

private:
  uint64_t lock OS_ALIGNED(8);
  Node * firstNode;
  uint64_t numUsed;
  
};

}

#endif
