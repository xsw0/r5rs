#include <chrono>
#include <iostream>
#include <memory>
#include <thread>
#include <variant>
#include "GC.h"
#include "GetRef.h"
#include "String.h"
#include "Type.h"

using namespace r5rs;

void printObj() {
  std::cout << "print obj" << std::endl;
  auto* ref = GC::global.next;
  while (ref != &GC::global) {
    assert(ref->prev->next == ref);
    assert(ref->next->prev == ref);
    auto r = static_cast<GC*>(ref);
    std::cout << &r->value << "\t" << std::hex << r->mask << "\t"
      << std::visit(String(), r->value) << std::endl;
    ref = ref->next;
  }
}

void printRef() {
  std::cout << "print ref" << std::endl;
  auto* ref = GCRef::global.next;
  while (ref != &GCRef::global) {
    assert(ref->prev->next == ref);
    assert(ref->next->prev == ref);
    auto r = static_cast<GCRef*>(ref);
    std::cout << &**r << "\t" << std::visit(String(), **r) << std::endl;
    ref = ref->next;
  }
}

int main(int argc, char* argv[]) {
  for (int i = 0; i < 100; i++) {
    auto x = GCRef(Pair({ i * 6, i * 6 + 1 }));
    assert(x.obj->count() == 1);
    auto y = GCRef(Pair({ i * 6 + 2, i * 6 + 3 }));
    assert(y.obj->count() == 1);
    auto z = GCRef(Pair({ i * 6 + 4, i * 6 + 5 }));
    assert(z.obj->count() == 1);
    std::get<Pair>(*x).first = y;
    std::get<Pair>(*y).first = z;
    std::get<Pair>(*z).first = x;

    assert(&*std::get<Pair>(*x).first);
    assert(&*std::get<Pair>(*y).first);
    assert(&*std::get<Pair>(*z).first);

    std::cout << "\033[2J";
    printObj();
    printRef();
  }
  GC::mark_and_sweep();

  std::cout << "Hello, world!\n";
  return 0;
}
