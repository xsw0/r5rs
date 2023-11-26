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
  auto r = GCRef::global.next;

  while (r != &GCRef::global) {
    static_cast<GCRef *>(r)->mark();
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

r5rs::InternalGCRef::~InternalGCRef() {
  if (obj) {
    GC::unref(obj);
  }
}

r5rs::InternalGCRef::InternalGCRef(const InternalGCRef &other)
    : InternalGCRef(other.obj) {}
r5rs::InternalGCRef::InternalGCRef(InternalGCRef &&other) noexcept
    : InternalGCRef(other.obj) {}

InternalGCRef &
r5rs::InternalGCRef::operator=(const InternalGCRef &other) {
  GC::ref(other.obj);
  GC::unref(obj);
  obj = other.obj;
  return *this;
}

InternalGCRef &
r5rs::InternalGCRef::operator=(InternalGCRef &&other) noexcept {
  GC::ref(other.obj);
  GC::unref(obj);
  obj = other.obj;
  return *this;
}

GCValue &r5rs::InternalGCRef::operator*() { return obj->value; }

const GCValue &r5rs::InternalGCRef::operator*() const { return obj->value; }

GCValue *r5rs::InternalGCRef::operator->() { return &obj->value; }

const GCValue *r5rs::InternalGCRef::operator->() const { return &obj->value; }

r5rs::InternalGCRef::InternalGCRef(GC *obj) : obj(obj) { GC::ref(obj); }

InternalGCRef &r5rs::InternalGCRef::operator=(nullptr_t) {
  assert(obj);
  obj = nullptr;
  return *this;
}

void r5rs::InternalGCRef::mark() {
  if (!obj->is_marked()) {
    obj->mark();
    for (auto &&child : std::visit(GetRef(), obj->value)) {
      child->mark();
    }
  }
}

GCRef::~GCRef() {
  rem(this);
  if (GC::size > GC::capacity) {
    GC::mark_and_sweep();
  }
}

GCRef &GCRef::operator=(const GCRef &other) {
  InternalGCRef::operator=(other);
  return *this;
}

GCRef &GCRef::operator=(GCRef &&other) noexcept {
  InternalGCRef::operator=(std::move(other));
  return *this;
}

GCRef::GCRef(GC *obj) : InternalGCRef(obj) { add(this); }

GCRef &GCRef::operator=(nullptr_t) {
  InternalGCRef::operator=(nullptr);
  return *this;
}
