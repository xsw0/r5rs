#ifndef _R5RS_GC_H_
#define _R5RS_GC_H_

#include <mutex>
#include <atomic>

#include "Type.h"
#include "List.h"

namespace r5rs
{
  class GC final
  {
    friend class Reference;
    friend class Object;

    GC() = delete;
    ~GC() = delete;

  public:
    static void mark_and_sweep();

  private:
    static void mark();
    static void sweep();

    inline static std::mutex mutex;
  };

  class Object;

  class Reference final: public List<Reference>
  {
    friend class Object;
    friend class GC;

    inline thread_local static bool external = true;

  public:
    static Reference gc(auto && value)
      requires std::is_nothrow_constructible_v<Value, decltype(value)>
    ;

    ~Reference();
    Reference();
    Reference(auto && value)
      requires (!std::is_same_v<Reference, std::remove_cvref_t<decltype(value)>>)
    ;

    Reference(const Reference & other);
    Reference(Reference && other) noexcept;

    Reference & operator=(nullptr_t);
    Reference & operator=(const Reference & other);
    Reference & operator=(Reference && other) noexcept;

    Value & operator*();
    const Value & operator*() const;

    Value * operator->();
    const Value * operator->() const;

    Reference(Object * obj);

    void mark_rec();
  private:
    Object * obj;
  };

  static_assert(std::is_copy_constructible_v<Reference>);
  static_assert(std::is_nothrow_move_constructible_v<Reference>);
  static_assert(std::is_copy_assignable_v<Reference>);
  static_assert(std::is_nothrow_move_assignable_v<Reference>);

  static_assert(std::is_copy_constructible_v<Value>);
  static_assert(std::is_nothrow_move_constructible_v<Value>);
  static_assert(std::is_copy_assignable_v<Value>);
  static_assert(std::is_nothrow_move_assignable_v<Value>);

  class Object final: public List<Object>
  {
    friend class Reference;
    friend class GC;

    constexpr inline static unsigned MARK = 1 << 31;
    inline static size_t capacity = 0x1000;

    static void ref(Object * obj);
    static void unref(Object * obj);

    static Object * create_object(auto && value)
      requires std::is_nothrow_constructible_v<Value, decltype(value)>;
  public:
    bool is_marked() const { return mask & MARK; }
    void mark() { mask |= MARK; }
    void unmark() { mask &= ~MARK; }
    unsigned count() const { return mask & ~MARK; }
    void inc()
    {
      assert(count() + 1 < MARK);
      ++mask;
    }
    void dec()
    {
      assert(count() > 0);
      --mask;
    }
    Value value;
    std::atomic_uint32_t mask;
  };

  Object * Object::create_object(auto && value)
    requires std::is_nothrow_constructible_v<Value, decltype(value)>
  {
    auto obj = static_cast<Object *>(std::malloc(sizeof(Object)));
    new(static_cast<List<Object> *>(obj)) List<Object>();
    new(&obj->value) Value(std::forward<decltype(value)>(value));
    new(&obj->mask) decltype(obj->mask)();

    add(obj, GC::mutex);

    return obj;
  }

  inline r5rs::Reference::Reference(): Reference(Value{}) {}

  Reference::Reference(auto && value)
    requires (!std::is_same_v<Reference, std::remove_cvref_t<decltype(value)>>)
  : Reference(Object::create_object(std::forward<decltype(value)>(value)))
  {
    if (Object::size > Object::capacity) { GC::mark_and_sweep(); }
  }

  Reference Reference::gc(auto && value)
    requires std::is_nothrow_constructible_v<Value, decltype(value)>
  {
    assert(external);
    external = false;
    auto obj = Object::create_object(std::forward<decltype(value)>(value));
    external = true;
    Reference ref(obj);
    if (Object::size > Object::capacity) { GC::mark_and_sweep(); }
    return ref;
  }

  inline Reference gc(auto && value)
    requires std::is_nothrow_constructible_v<Value, decltype(value)>
  {
    return Reference::gc(std::forward<decltype(value)>(value));
  }

} // namespace r5rs

#endif
