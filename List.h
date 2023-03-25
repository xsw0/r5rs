#ifndef _R5RS_LIST_H_
#define _R5RS_LIST_H_

#include <mutex>

namespace r5rs
{
  template<typename T>
  class List
  {
  public:
    static void add(T * element);
    static void rem(T * element);
    static bool exist(T * element);

    static void add(T * element, std::mutex & mutex);
    static void rem(T * element, std::mutex & mutex);
    static bool exist(T * element, std::mutex & mutex);

    inline static List global;
    inline static size_t size;

    mutable List * prev = this;
    mutable List * next = this;
  };

  template<typename T>
  void List<T>::add(T * element)
  {
    assert(element->prev == element);
    assert(element->next == element);
    ++size;
    global.next->prev = element;
    element->next = global.next;
    global.next = element;
    element->prev = &global;
  }

  template<typename T>
  void List<T>::rem(T * element)
  {
    assert(element->prev != element);
    assert(element->next != element);
    assert(size > 0);
    --size;
    element->prev->next = element->next;
    element->next->prev = element->prev;
  }

  template<typename T>
  inline bool List<T>::exist(T * element)
  {
    assert((element->next == element) == (element->prev == element));
    return element->prev != element;
  }

  template<typename T>
  inline void List<T>::add(T * element, std::mutex & mutex)
  {
    std::lock_guard<std::mutex> lock(mutex);
    add(element);
  }

  template<typename T>
  inline void List<T>::rem(T * element, std::mutex & mutex)
  {
    std::lock_guard<std::mutex> lock(mutex);
    rem(element);
  }

  template<typename T>
  inline bool List<T>::exist(T * element, std::mutex & mutex)
  {
    std::lock_guard<std::mutex> lock(mutex);
    return exist(element);
  }
}

#endif
