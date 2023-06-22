#include <iostream>
#include <memory>
#include <thread>
#include <variant>
#include <chrono>

#include "Type.h"
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
  for (int i = 0; i < 100; i++)
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
    // std::cout << "\033[2J";
    // printRef();
    // printObj();
    // std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
  GC::mark_and_sweep();

  std::cout << "Hello, world!\n";
  return 0;
}
