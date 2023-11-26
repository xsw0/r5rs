#ifndef _R5RS_TEST_RECORD_H_
#define _R5RS_TEST_RECORD_H_

#include <cassert>
#include <iostream>
#include <list>
#include <vector>
#include <string>

struct Record
{
  static std::list<Record> records;
  std::vector<std::string> args;

  static void empty() { assert(records.empty()); }
  static void clear() { records.clear(); }

  static void match(std::vector<Record> const & rds)
  {
    for (auto && r : rds)
    {
      if (records.front() != r)
      {
        std::cout << r.string() << std::endl;
        std::cout << records.front().string() << std::endl;
        std::cout << std::endl;
        assert(0);
      }
      records.pop_front();
    }
    clear();
  }

  static void add(Record rd) { records.push_back(rd); }

  explicit Record(std::vector<std::string> args = {}): args{ std::move(args) } {}
  Record(std::initializer_list<std::string> l): Record{ std::vector<std::string>{l} } {}

  operator std::string() const
  {
    if (args.empty()) { return "{}"; }
    std::string res;
    res += "{ ";
    for (size_t i = 0; i < args.size(); ++i)
    {
      if (i != 0) { res += ", "; }
      res += args[i];
    }
    res += " }";
    return res;
  }

  std::string string() const
  {
    return static_cast<std::string>(*this);
  }

  bool operator==(Record const &) const = default;
};

#endif
