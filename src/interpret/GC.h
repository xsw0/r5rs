#ifndef _R5RS_GC_H_
#define _R5RS_GC_H_

#include <atomic>
#include <cassert>

#include "List.h"
#include "Type.h"

namespace r5rs {
  class GC;
  class InternalReference;
  class Reference;

  class InternalReference {
    friend class GC;
    friend class Reference;

  public:
    virtual ~InternalReference();

    InternalReference();

    template <typename T>
    InternalReference(T&& value)
      requires(!std::is_same_v<InternalReference, std::remove_cvref_t<T>>) &&
    (!std::is_same_v<Reference, std::remove_cvref_t<T>>);

    InternalReference(const InternalReference& other);
    InternalReference(InternalReference&& other) noexcept;

    InternalReference& operator=(const InternalReference& other);
    InternalReference& operator=(InternalReference&& other) noexcept;

    Value& operator*();
    const Value& operator*() const;

    Value* operator->();
    const Value* operator->() const;

  protected:
  public:
    explicit InternalReference(GC* obj);
    InternalReference& operator=(nullptr_t);

    void mark();

    GC* obj;
  };

  static_assert(std::is_copy_constructible_v<InternalReference>);
  static_assert(std::is_copy_assignable_v<InternalReference>);

  class GC final : public List<GC> {
    friend class InternalReference;
    friend class Reference;

    constexpr inline static unsigned MARK = 1 << 31;
    inline static size_t capacity = 0x10;

  public:
    static void mark_and_sweep();

    template <typename T>
    explicit GC(T&& value)
      requires std::is_nothrow_constructible_v<Value, T>
    : value{ std::forward<T>(value) }, mask{ 0 } {}

  private:
  public:
    template <typename T>
    static GC* gc(T&& value)
      requires std::is_nothrow_constructible_v<Value, T>;

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
    Value value;
    std::atomic_uint32_t mask;
  };

  class Reference : public InternalReference, public List<Reference> {
  public:
    ~Reference() override;

    Reference();

    template <typename T>
    Reference(T&& value)
      requires(!std::is_same_v<InternalReference, std::remove_cvref_t<T>>) &&
    (!std::is_same_v<Reference, std::remove_cvref_t<T>>);

    Reference(const InternalReference& other) : Reference(other.obj) {}

    Reference(const Reference& other) : Reference(other.obj) {}
    Reference(Reference&& other) : Reference(other.obj) {}

    Reference& operator=(const Reference& other);
    Reference& operator=(Reference&& other) noexcept;

  private:
  public:
    explicit Reference(GC* obj);
    Reference& operator=(nullptr_t);
  };

  static_assert(std::is_copy_constructible_v<Reference>);
  static_assert(std::is_copy_assignable_v<Reference>);

  template <typename T>
  GC* GC::gc(T&& value)
    requires std::is_nothrow_constructible_v<Value, T>
  {
    auto obj = static_cast<GC*>(std::malloc(sizeof(GC)));
    new (obj) GC(std::forward<T>(value));
    add(obj);
    return obj;
  }

  inline r5rs::InternalReference::InternalReference()
    : InternalReference(Value{}) {}

  template <typename T>
  InternalReference::InternalReference(T&& value)
    requires(!std::is_same_v<InternalReference, std::remove_cvref_t<T>>) &&
  (!std::is_same_v<Reference, std::remove_cvref_t<T>>)
    : InternalReference(GC::gc(std::forward<T>(value))) {}

  inline r5rs::Reference::Reference() : Reference(Value{}) {}

  template <typename T>
  Reference::Reference(T&& value)
    requires(!std::is_same_v<InternalReference, std::remove_cvref_t<T>>) &&
  (!std::is_same_v<Reference, std::remove_cvref_t<T>>)
    : Reference(GC::gc(std::forward<T>(value))) {}

  static_assert(std::is_move_constructible_v<Value>);
  static_assert(std::is_move_assignable_v<Value>);
} // namespace r5rs

#endif
