#ifndef __UTILITIES_LINKED_LIST_HPP__
#define __UTILITIES_LINKED_LIST_HPP__

#include <cstddef>

namespace OS {

template <class T>
class LinkedListLink {
public:
  T * next;
  T * last;
  LinkedListLink() : next(NULL), last(NULL) {}
};

template <class T, LinkedListLink<T> T::*ptr>
class LinkedList {
public:
  T * first;
  T * last;
  
  LinkedList();
  void PushFront(T *);
  void PushBack(T *);
  T * PopFront();
  T * PopBack();
  bool Remove(T *);
};

}

#include <utilities/linked-list-public.hpp>

#endif
