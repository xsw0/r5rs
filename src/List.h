#ifndef _R5RS_LIST_H_
#define _R5RS_LIST_H_

#include <mutex>

namespace r5rs {
template <typename T> class List {
public:
  static void add(List<T> *element);
  static List *rem(List<T> *element);
  static bool exist(List<T> *element);

  inline static List global;
  inline static size_t size;

  mutable List *prev = this;
  mutable List *next = this;
};

template <typename T> void List<T>::add(List<T> *element) {
  assert(size != std::numeric_limits<decltype(size)>::max());
  assert(element->prev == element);
  assert(element->next == element);

  ++size;

  auto next = global.next;

  element->next = next;
  element->prev = &global;

  next->prev = element;
  global.next = element;
}

template <typename T> List<T> *List<T>::rem(List<T> *element) {
  assert(size > 0);
  assert(element->prev != element);
  assert(element->next != element);

  --size;

  auto prev = element->prev;
  auto next = element->next;

  prev->next = next;
  next->prev = prev;

  element->prev = element;
  element->next = element;

  return next;
}

template <typename T> inline bool List<T>::exist(List<T> *element) {
  assert((element->next == element) == (element->prev == element));
  return element->prev != element;
}
} // namespace r5rs

#endif
