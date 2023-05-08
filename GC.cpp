#include "GC.h"

#include <cstdlib>

#include "GetRef.h"

using namespace r5rs;

void r5rs::Object::ref(Object *obj) {
  obj->inc();
}

void r5rs::Object::unref(Object *obj) {
  obj->dec();

  if (obj->count() == 0) {
    rem(obj, GC::mutex);
    obj->value.~Value();
    std::free(obj);
  }
}

Value *r5rs::Reference::operator->() {
  return &obj->value;
}

const Value *r5rs::Reference::operator->() const {
  return &obj->value;
}

r5rs::Reference::Reference(Object *obj) : obj(obj) {
  Object::ref(obj);
  if (external) {
    add(this, GC::mutex);
  }
}

void r5rs::Reference::mark_rec() {
  if (!obj->is_marked()) {
    obj->mark();
    for (auto &&child : std::visit(GetRef(), obj->value)) {
      child->mark_rec();
    }
  }
}

r5rs::Reference::Reference(const Reference &other) : Reference(other.obj) {}
r5rs::Reference::Reference(Reference &&other) noexcept: Reference(other.obj) {}

Reference &r5rs::Reference::operator=(nullptr_t) {
  assert(obj);
  obj = nullptr;
  return *this;
}

Reference &r5rs::Reference::operator=(const Reference &other) {
  Object::ref(const_cast<Object *>(other.obj));
  Object::unref(obj);
  obj = other.obj;
  return *this;
}

Reference &r5rs::Reference::operator=(Reference &&other) noexcept {
  Object::ref(const_cast<Object *>(other.obj));
  Object::unref(obj);
  obj = other.obj;
  return *this;
}

Value &r5rs::Reference::operator*() {
  return obj->value;
}

const Value &r5rs::Reference::operator*() const {
  return obj->value;
}

r5rs::Reference::~Reference() {
  if (obj) {
    Object::unref(obj);
    if (exist(this)) {
      rem(this, GC::mutex);
    }
  } else {
    assert(!exist(this));
  }
}

void r5rs::GC::mark() {
  List<Reference> *ref = Reference::global.next;
  while (ref != &Reference::global) {
    auto r = static_cast<Reference *>(ref);
    r->mark_rec();
    ref = ref->next;
  }
}

void r5rs::GC::sweep() {
  List<Object> *obj = Object::global.next;
  while (obj != &Object::global) {
    auto o = static_cast<Object *>(obj);
    obj = obj->next;
    if (o->is_marked()) {
      o->unmark();
    } else {
      for (auto &&child : std::visit(GetRef(), o->value)) {
        *child = nullptr;
      }
      o->value.~Value();
      Object::rem(o);
      free(o);
    }
  }
}

void r5rs::GC::mark_and_sweep() {
  std::lock_guard<std::mutex> lock(GC::mutex);
  mark();
  sweep();
  auto new_size = Object::size + Object::size / 2;
  Object::capacity = std::max(new_size, Object::capacity);
}
