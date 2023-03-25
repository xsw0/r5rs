#include <iostream>
#include <memory>
#include <thread>
#include <variant>
#include <vector>
#include <random>

#include "Type.h"
#include "Value.h"
#include "String.h"
#include "GC.h"
#include "GetRef.h"

using namespace std;
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

std::mutex mtx;
vector<Reference> vec;
random_device rd;
default_random_engine re(rd());

void foo()
{
  for (int i = 0; i < 30; ++i)
  {
    if (re() % 2 == 0)
    {
      lock_guard lock(mtx);
      if (!vec.empty())
      {
        vec.pop_back();
      }
    }
    auto x = gc(Pair{ (int64_t)re(), (int64_t)re() });
    assert(&*std::get<Pair>(*x).first);
    assert(&*std::get<Pair>(*x).second);
    lock_guard lock(mtx);
    if (!vec.empty())
    {
      std::get<Pair>(*x).first = vec[re() % vec.size()];
      std::get<Pair>(*x).second = vec[re() % vec.size()];
    }
    vec.push_back(x);
  }
}

int main(int argc, char * argv[])
{
  vector<thread> ts;
  for (int i = 0; i < 33; i++)
  {
    ts.emplace_back(foo);
  }
  for (auto && t : ts)
  {
    t.join();
  }

  for (auto && p : vec)
  {
    assert(&*std::get<Pair>(*p).first);
    assert(&*std::get<Pair>(*p).second);
  }
  std::cout << "Hello, world!\n";
  return 0;
}