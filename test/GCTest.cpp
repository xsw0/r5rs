#include <iostream>
#include <memory>
#include <thread>
#include <variant>

#include "Type.h"
#include "Value.h"
#include "String.h"
#include "GC.h"
#include "GetRef.h"

using namespace r5rs;

void printObj()
{
  std::cout << "print obj" << std::endl;
  auto * ref = Object::global.next;
  while (ref != &Object::global)
  {
    assert(ref->prev->next == ref);
    assert(ref->next->prev == ref);
    auto r = static_cast<Object *>(ref);
    std::cout << &r->value << "\t" << std::hex << r->mask << "\t" << std::visit(String(), r->value) << std::endl;
    ref = ref->next;
  }
}

void printRef()
{
  std::cout << "print ref" << std::endl;
  auto * ref = Reference::global.next;
  while (ref != &Reference::global)
  {
    assert(ref->prev->next == ref);
    assert(ref->next->prev == ref);
    auto r = static_cast<Reference *>(ref);
    std::cout << &**r << "\t" << std::visit(String(), **r) << std::endl;
    ref = ref->next;
  }
}

int main(int argc, char * argv[])
{
  for (int i = 0; i < 65536; i++)
  {
    auto x = gc(Pair({ i * 6, i * 6 + 1 }));
    auto y = gc(Pair({ i * 6 + 2, i * 6 + 3 }));
    auto z = gc(Pair({ i * 6 + 4, i * 6 + 5 }));
    std::get<Pair>(*x).first = y;
    std::get<Pair>(*y).first = z;
    std::get<Pair>(*z).first = x;

    assert(&*std::get<Pair>(*x).first);
    assert(&*std::get<Pair>(*y).first);
    assert(&*std::get<Pair>(*z).first);
    if (i % 1000 == 0)
    {
      std::cout << i << std::endl;
    }
  }
  GC::mark_and_sweep();
  {
    auto n = gc(5);
    auto p = gc(std::pair<Reference, Reference>({ 2, 3 }));
  }
  // *n = 6;
  // std::cout << std::visit(String(), *n) << std::endl;
  // std::cout << std::visit(String(), *p) << std::endl;

  // Value v;
  // auto w = std::visit(String(), v);
  // auto x = std::visit(GetRef(), v);

  std::cout << "Hello, world!\n";
  return 0;
}
