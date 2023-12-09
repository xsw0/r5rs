#include <iostream>
#include <variant>
#include <concepts>
#include <type_traits>
#include <string>
#include <vector>
#include <cassert>
#include <utility>
#include <memory>
#include <version>
#include <functional>

#include "value_ref.h"

#include "output.h"
#include <catch2/catch_test_macros.hpp>

class Base
{
public:
  using callback_t = std::function<void(std::vector<std::string>)>;

  ~Base() { callback({ "dtor", name }); }

  Base(callback_t const & callback, std::string name = "")
    : callback{ callback }, name{ std::move(name) }
  {
    callback({ "ctor", this->name });
  }

  Base(Base const & base)
    :callback{ base.callback }, name{ base.name }
  {
    callback({ "copy ctor", name });
  }

  Base(Base && base)
    :callback{ base.callback }, name{ base.name }
  {
    callback({ "move ctor", name });
  }

  Base & operator=(Base const & base)
  {
    callback({ "copy assign", name, base.name });
    return *this;
  }

  Base & operator=(Base && base)
  {
    callback({ "move assign", name, base.name });
    return *this;
  }

  Base & operator=(Base & base)
  {
    callback({ "copy assign", name, base.name });
    return *this;
  }

  Base & operator=(Base const && base)
  {
    callback({ "move assign", name, base.name });
    return *this;
  }

  std::string const name;
  std::function<void(std::vector<std::string>)> const & callback;
};

TEST_CASE("value_ref")
{
  std::vector<std::vector<std::string>> records;

  Base::callback_t record = [&](std::vector<std::string> r) {
    records.push_back(std::move(r));
  };

  auto match = [&](std::vector<std::vector<std::string>> const & rs) {
    auto res = records == rs;
    records.clear();
    return res;
  };

  using namespace r5rs;

  SECTION("ctor & dtor")
  {
    {
      auto r1 = make_value<Base>(record, "1");
      INFO(records);
      REQUIRE(match({ { "ctor", "1" } }));

      auto r2 = r1;
      INFO(records);
      REQUIRE(match({ { "copy ctor", "1" } }));

      auto r3 = std::move(r1);
      INFO(records);
      REQUIRE(match({ { "move ctor", "1" } }));
    }
    INFO(records);
    REQUIRE(match({ { "dtor", "1" }, { "dtor", "1" }, { "dtor", "1" } }));
  }

  SECTION("assign")
  {
    {
      auto r1 = make_value<Base>(record, "1");
      INFO(records);
      REQUIRE(match({ { "ctor", "1" } }));

      auto r2 = make_value<Base>(record, "2");
      INFO(records);
      REQUIRE(match({ { "ctor", "2" } }));

      r1 = r2;
      INFO(records);
      REQUIRE(match({ { "copy assign", "1", "2" } }));

      r1 = std::move(r2);
      INFO(records);
      REQUIRE(match({ { "move assign", "1", "2" } }));

      r1 = make_value<Base>(record, "3");
      INFO(records);
      REQUIRE(match({ { "ctor", "3" }, { "move assign", "1", "3" }, { "dtor", "3" } }));
    }
    INFO(records);
    REQUIRE(match({ { "dtor", "2" }, { "dtor", "1" } }));
  }
}
