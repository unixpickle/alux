namespace OS {

template <class T, LinkedListLink<T> T::*Y>
LinkedList<T, Y>::LinkedList() : first(NULL), last(NULL) {
}

template <class T, LinkedListLink<T> T::*Y>
void LinkedList<T, Y>::PushFront(T * item) {
  (item->*Y).next = first;
  (item->*Y).last = NULL;
  first = item;
  if (!last) {
    last = item;
  }
}

template <class T, LinkedListLink<T> T::*Y>
void LinkedList<T, Y>::PushBack(T * item) {
  (item->*Y).last = last;
  (item->*Y).next = NULL;
  last = item;
  if (!first) {
    first = item;
  }
}

template <class T, LinkedListLink<T> T::*Y>
T * LinkedList<T, Y>::PopFront() {
  if (!first) return NULL;
  T result = first;
  
  first = (first->*Y).next;
  if (!first) {
    last = NULL;
  }
  return result;
}

template <class T, LinkedListLink<T> T::*Y>
T * LinkedList<T, Y>::PopBack() {
  if (!last) return NULL;
  T result = last;
  
  last = (last->*Y).last;
  if (!last) {
    first = NULL;
  }
  return result;
}

template <class T, LinkedListLink<T> T::*Y>
bool LinkedList<T, Y>::Remove(T * item) {
  if (!(item->*Y).next) {
    if (item != last) return false;
    last = (item->*Y).last;
    if (!last) first = NULL;
  } else if (!(item->*Y).last) {
    if (item != first) return false;
    first = (item->*Y).next;
    if (!first) last = NULL;
  } else {
    ((item->*Y).last->*Y).next = (item->*Y).next;
    ((item->*Y).next->*Y).last = (item->*Y).last;
  }
  return true;
}

}
