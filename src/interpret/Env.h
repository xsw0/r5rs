#ifndef R5RS_ENV_H
#define R5RS_ENV_H

#include <memory>
#include <unordered_map>

#include "Expressions.h"
#include "GC.h"
#include "Type.h"

namespace r5rs {
  class Env : public std::enable_shared_from_this<Env> {
  public:
    std::shared_ptr<Env> parent = nullptr;
    std::unordered_map<std::string, GCRef> variables;
    explicit Env(std::shared_ptr<Env> parent = nullptr) : parent(parent) {}

    explicit Env(const expression::Formals& formals,
      const std::list<GCRef>& args,
      std::shared_ptr<Env> parent = nullptr)
      : parent(parent) {
      if (args.size() < formals.fixed.size()) {
        throw std::runtime_error("Insufficient number of parameters");
      }
      if (!formals.binding && args.size() > formals.fixed.size()) {
        throw std::runtime_error("redundant arguments");
      }
      auto var = formals.fixed.begin();
      auto val = args.begin();
      for (; var != formals.fixed.end(); ++var, ++val) {
        variables.insert({ *var, *val });
      }

      if (formals.binding) {
        GCRef head = nullptr;

        auto it = args.end();
        while (it != val) {
          head = Pair{ *std::prev(it), head };
          --it;
        }

        variables.emplace(*formals.binding, std::move(head));
      }
    }

    void set(const std::string& var, GCRef val) { variables[var] = val; }

    std::optional<GCRef> get(const std::string& var) {
      auto it = variables.find(var);
      if (it != variables.end()) {
        return it->second;
      }
      if (!parent) {
        return std::nullopt;
      }
      return parent->get(var);
    }

    std::string print() const;
  };
} // namespace r5rs

#endif
