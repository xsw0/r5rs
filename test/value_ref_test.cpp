#include <iostream>
#include <variant>
#include <concepts>
#include <type_traits>
#include <string>
#include <vector>
#include <cassert>
#include <utility>
#include <memory>

#include "Record.h"
#include "value_ref.h"

using namespace r5rs;

class Base
{
public:
  inline static unsigned n = 0;

  ~Base() { Record::add({ "dtor", name }); }

  Base(std::string name = "")
    : name{ std::move(name) }
  {
    Record::add({ "ctor", this->name });
  }

  Base(Base const & base): name{ base.name } { Record::add({ "copy ctor", name }); }
  Base(Base && base): name{ base.name } { Record::add({ "move ctor", name }); }
  Base & operator=(Base const & base)
  {
    Record::add({ "copy assign", name, base.name });
    return *this;
  }
  Base & operator=(Base && base)
  {
    Record::add({ "move assign", name, base.name });
    return *this;
  }

  std::string const name;
};

int main()
{
  {
    {
      value_ref<Base> v{ "1" };
      Record::match({ { "ctor", "1" } });

      value_ref<Base> v2 = Base{ "2" };
      Record::match({ { "ctor", "2" }, { "move ctor", "2" }, { "dtor", "2" } });

      value_ref<Base> v3 = value_ref<Base>{ "3" };
      Record::match({ { "ctor", "3" } });

      value_ref<Base> v4 = v;
      Record::match({ { "copy ctor", "1" } });

      value_ref<Base> v5 = v2;
      Record::match({ { "copy ctor", "2" } });

      value_ref<Base> v6 = std::move(v);
      Record::match({ { "move ctor", "1" } });

      value_ref<Base> v7 = std::move(v2);
      Record::match({ { "move ctor", "2" } });

      v2 = v;
      Record::match({ { "copy assign", "2", "1" } });

      v3 = v2;
      Record::match({ { "copy assign", "3", "2" } });

      v4 = std::move(v);
      Record::match({ { "move assign", "1", "1" } });

      v5 = std::move(v2);
      Record::match({ { "move assign", "2", "2" } });

      v = v2;
      Record::match({ { "copy assign", "1", "2" } });
    }
    Record::match({
      { "dtor", "2" },
      { "dtor", "1" },
      { "dtor", "2" },
      { "dtor", "1" },
      { "dtor", "3" },
      { "dtor", "2" },
      { "dtor", "1" },
    });
  }

  std::cout << "done." << std::endl;
  return 0;
}
