#include "Env.h"

#include <iomanip>
#include <iostream>
#include <sstream>

#include "GC.h"
#include "String.h"
#include "Type.h"

namespace {
  using namespace r5rs;
  std::string printObj(GCRef ref) {
    std::string res;

    std::stringstream stream;
    stream << std::hex << ref.obj;

    res += '"' + stream.str() + '"' + " -> " + '"' + std::visit(String(), *ref) +
      '"';

    return res;
  }

  std::string subgraph(const Env* env, const std::string& name) {
    std::string res;
    res += "subgraph " + name +
      " {"
      "\n";

    for (auto&& [var, val] : env->variables) {
      res += "\t\"" + var + "\" -> " + printObj(val) + "\n";
    }

    res += "}"
      "\n";
    return res;
  }
} // namespace

std::string r5rs::Env::print() const {
  std::string res;
  res += "digraph Env {"
    "\n";

  int i = 0;

  auto env = this;
  while (env) {
    res += subgraph(env, "Env" + std::to_string(i));

    env = env->parent.get();
    ++i;
  }

  res += "}"
    "\n";
  return res;
}
