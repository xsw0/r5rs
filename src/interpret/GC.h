#ifndef R5RS_GC_H
#define R5RS_GC_H

#include <atomic>
#include <cassert>

#include "List.h"
#include "Type.h"

namespace r5rs {
  class GC;
  class InternalGCRef;
  class GCRef;

  class InternalGCRef {
    friend class GC;
    friend class GCRef;

  public:
    virtual ~InternalGCRef();

    InternalGCRef();

    template <typename T>
    InternalGCRef(T&& value)
      requires(!std::is_same_v<InternalGCRef, std::remove_cvref_t<T>>) &&
    (!std::is_same_v<GCRef, std::remove_cvref_t<T>>);

    InternalGCRef(const InternalGCRef& other);
    InternalGCRef(InternalGCRef&& other) noexcept;

    InternalGCRef& operator=(const InternalGCRef& other);
    InternalGCRef& operator=(InternalGCRef&& other) noexcept;

    GCValue& operator*();
    const GCValue& operator*() const;

    GCValue* operator->();
    const GCValue* operator->() const;

  protected:
  public:
    explicit InternalGCRef(GC* obj);
    InternalGCRef& operator=(nullptr_t);

    void mark();

    GC* obj;
  };

  static_assert(std::is_copy_constructible_v<InternalGCRef>);
  static_assert(std::is_copy_assignable_v<InternalGCRef>);

  class GC final : public List<GC> {
    friend class InternalGCRef;
    friend class GCRef;

    constexpr inline static unsigned MARK = 1 << 31;
    inline static size_t capacity = 0x10;

  public:
    static void mark_and_sweep();

    template <typename T>
    explicit GC(T&& value)
      requires std::is_nothrow_constructible_v<GCValue, T>
    : value{ std::forward<T>(value) }, mask{ 0 } {}

  private:
  public:
    template <typename T>
    static GC* gc(T&& value)
      requires std::is_nothrow_constructible_v<GCValue, T>;

    static void mark_objects();
    static void sweep_objects();

    static void ref(GC* obj);
    static void unref(GC* obj);

    bool is_marked() const { return mask & MARK; }
    void mark() { mask |= MARK; }
    void unmark() { mask &= ~MARK; }

    unsigned count() const { return mask & ~MARK; }
    void inc() {
      assert(count() + 1 < MARK);
      ++mask;
    }
    void dec() {
      assert(count() > 0);
      --mask;
    }

  public:
    GCValue value;
    std::atomic_uint32_t mask;
  };

  class GCRef : public InternalGCRef, public List<GCRef> {
  public:
    ~GCRef() override;

    GCRef();

    template <typename T>
    GCRef(T&& value)
      requires(!std::is_same_v<InternalGCRef, std::remove_cvref_t<T>>) &&
    (!std::is_same_v<GCRef, std::remove_cvref_t<T>>);

    GCRef(const InternalGCRef& other) : GCRef(other.obj) {}

    GCRef(const GCRef& other) : GCRef(other.obj) {}
    GCRef(GCRef&& other) : GCRef(other.obj) {}

    GCRef& operator=(const GCRef& other);
    GCRef& operator=(GCRef&& other) noexcept;

  private:
  public:
    explicit GCRef(GC* obj);
    GCRef& operator=(nullptr_t);
  };

  static_assert(std::is_copy_constructible_v<GCRef>);
  static_assert(std::is_copy_assignable_v<GCRef>);

  template <typename T>
  GC* GC::gc(T&& value)
    requires std::is_nothrow_constructible_v<GCValue, T>
  {
    auto obj = static_cast<GC*>(std::malloc(sizeof(GC)));
    new (obj) GC(std::forward<T>(value));
    add(obj);
    return obj;
  }

  inline r5rs::InternalGCRef::InternalGCRef()
    : InternalGCRef(GCValue{}) {}

  template <typename T>
  InternalGCRef::InternalGCRef(T&& value)
    requires(!std::is_same_v<InternalGCRef, std::remove_cvref_t<T>>) &&
  (!std::is_same_v<GCRef, std::remove_cvref_t<T>>)
    : InternalGCRef(GC::gc(std::forward<T>(value))) {}

  inline r5rs::GCRef::GCRef() : GCRef(GCValue{}) {}

  template <typename T>
  GCRef::GCRef(T&& value)
    requires(!std::is_same_v<InternalGCRef, std::remove_cvref_t<T>>) &&
  (!std::is_same_v<GCRef, std::remove_cvref_t<T>>)
    : GCRef(GC::gc(std::forward<T>(value))) {}

  static_assert(std::is_move_constructible_v<GCValue>);
  static_assert(std::is_move_assignable_v<GCValue>);
} // namespace r5rs

#endif
