#include "GC.h"

#include <cstdlib>

#include "GetRef.h"

using namespace r5rs;

void r5rs::GC::ref(GC *obj) { obj->inc(); }

void r5rs::GC::unref(GC *obj) {
  obj->dec();

  if (obj->count() == 0) {
    rem(obj);
    obj->~GC();
    std::free(obj);
  }
}

void r5rs::GC::mark_objects() {
  auto r = Reference::global.next;

  while (r != &Reference::global) {
    static_cast<Reference *>(r)->mark();
    r = r->next;
  }
}

void r5rs::GC::sweep_objects() {
  auto obj = global.next;
  while (obj != &global) {
    auto o = static_cast<GC *>(obj);
    obj = obj->next;
    if (o->is_marked()) {
      o->unmark();
    } else {
      for (auto &&child : std::visit(GetRef(), o->value)) {
        *child = nullptr;
      }
      rem(o);
      o->~GC();
      std::free(o);
    }
  }
}

void r5rs::GC::mark_and_sweep() {
  mark_objects();
  sweep_objects();
  auto new_size = GC::size + GC::size / 2;
  GC::capacity = std::max(new_size, GC::capacity);
}

r5rs::InternalReference::~InternalReference() {
  if (obj) {
    GC::unref(obj);
  }
}

r5rs::InternalReference::InternalReference(const InternalReference &other)
    : InternalReference(other.obj) {}
r5rs::InternalReference::InternalReference(InternalReference &&other) noexcept
    : InternalReference(other.obj) {}

InternalReference &
r5rs::InternalReference::operator=(const InternalReference &other) {
  GC::ref(other.obj);
  GC::unref(obj);
  obj = other.obj;
  return *this;
}

InternalReference &
r5rs::InternalReference::operator=(InternalReference &&other) noexcept {
  GC::ref(other.obj);
  GC::unref(obj);
  obj = other.obj;
  return *this;
}

Value &r5rs::InternalReference::operator*() { return obj->value; }

const Value &r5rs::InternalReference::operator*() const { return obj->value; }

Value *r5rs::InternalReference::operator->() { return &obj->value; }

const Value *r5rs::InternalReference::operator->() const { return &obj->value; }

r5rs::InternalReference::InternalReference(GC *obj) : obj(obj) { GC::ref(obj); }

InternalReference &r5rs::InternalReference::operator=(nullptr_t) {
  assert(obj);
  obj = nullptr;
  return *this;
}

void r5rs::InternalReference::mark() {
  if (!obj->is_marked()) {
    obj->mark();
    for (auto &&child : std::visit(GetRef(), obj->value)) {
      child->mark();
    }
  }
}

Reference::~Reference() {
  rem(this);
  if (GC::size > GC::capacity) {
    GC::mark_and_sweep();
  }
}

Reference &Reference::operator=(const Reference &other) {
  InternalReference::operator=(other);
  return *this;
}

Reference &Reference::operator=(Reference &&other) noexcept {
  InternalReference::operator=(std::move(other));
  return *this;
}

Reference::Reference(GC *obj) : InternalReference(obj) { add(this); }

Reference &Reference::operator=(nullptr_t) {
  InternalReference::operator=(nullptr);
  return *this;
}
