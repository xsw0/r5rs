#include "Interpreter.h"

#include <fstream>

using namespace r5rs;
using namespace expression;

namespace {
  template <size_t n> void check_num(std::list<GCRef>& args) {
    if (args.size() != n) {
      throw std::runtime_error("Argument number error!");
    }
  }

  template <typename T> void check_type(GCRef arg) {
    if (!std::holds_alternative<T>(*arg)) {
      throw std::runtime_error("Argument type error!");
    }
  }

  template <typename T> T cast(GCRef arg) {
    if (!std::holds_alternative<T>(*arg)) {
      throw std::runtime_error("Argument type error!");
    }
    return std::get<T>(*arg);
  }

  template <size_t I> GCRef index(std::list<GCRef>& args) {
    auto it = args.begin();
    std::advance(it, I);
    return *it;
  }

  GCRef add(std::list<GCRef>& args) {
    int64_t sum = 0;
    for (auto&& arg : args) {
      sum += cast<int64_t>(arg);
    }
    return sum;
  }

  GCRef mul(std::list<GCRef>& args) {
    int64_t prod = 1;
    for (auto&& arg : args) {
      prod *= cast<int64_t>(arg);
    }
    return prod;
  }

  GCRef is_empty(std::list<GCRef>& args) {
    check_num<1>(args);
    if (std::holds_alternative<nullptr_t>(*index<0>(args))) {
      return true;
    }
    return false;
  }

  GCRef cons(std::list<GCRef>& args) {
    check_num<2>(args);
    return Pair{ index<0>(args), index<1>(args) };
  }

  GCRef car(std::list<GCRef>& args) {
    check_num<1>(args);
    return cast<Pair>(index<0>(args)).first;
  }

  GCRef cdr(std::list<GCRef>& args) {
    check_num<1>(args);
    return cast<Pair>(index<0>(args)).second;
  }

  GCRef Or(std::list<GCRef>& args) {
    auto visitor = overloaded{
        [](nullptr_t) -> bool { return false; },
        [](bool b) -> bool { return b; },
        [](auto&&) -> bool { return true; },
    };
    for (auto&& arg : args) {
      if (std::visit(visitor, *arg)) {
        return true;
      }
    }
    return false;
  }

  GCRef And(std::list<GCRef>& args) {
    auto visitor = overloaded{
        [](nullptr_t) -> bool { return false; },
        [](bool b) -> bool { return b; },
        [](auto&&) -> bool { return true; },
    };
    for (auto&& arg : args) {
      if (!std::visit(visitor, *arg)) {
        return false;
      }
    }
    return true;
  }

  GCRef eqv(std::list<GCRef>& args) {
    check_num<2>(args);
    return cast<int64_t>(index<0>(args)) == cast<int64_t>(index<1>(args));
  }

  GCRef less(std::list<GCRef>& args) {
    check_num<2>(args);
    return cast<int64_t>(index<0>(args)) < cast<int64_t>(index<1>(args));
  }

  GCRef greater(std::list<GCRef>& args) {
    check_num<2>(args);
    return cast<int64_t>(index<0>(args)) > cast<int64_t>(index<1>(args));
  }

  GCRef read(std::list<GCRef>& args) {
    check_num<1>(args);
    auto name = cast<std::string>(index<0>(args));

    std::ifstream file(name);

    std::vector<int64_t> v;
    int64_t n;
    while (file >> n) {
      v.push_back(n);
    }

    GCRef list = nullptr;
    for (auto it = v.rbegin(); it != v.rend(); ++it) {
      list = Pair{ *it, list };
    }

    return list;
  }
} // namespace

r5rs::Interpreter::Interpreter() : env{ std::make_shared<Env>() } {
  env->set("+", &add);
  env->set("*", &mul);
  env->set("cons", &cons);
  env->set("car", &car);
  env->set("cdr", &cdr);
  env->set("empty?", &is_empty);
  env->set("read", &read);
  env->set("eqv?", &eqv);
  env->set("<", &less);
  env->set(">", &greater);
  env->set("or", &Or);
  env->set("and", &And);
}

void r5rs::Interpreter::push() {
  assert(env);
  env = std::make_shared<Env>(env);
}

void r5rs::Interpreter::pop() {
  assert(env);
  assert(env->parent);
  env = env->parent;
}

GCRef r5rs::Interpreter::operator()(expression::COD* cod) {
  return std::visit(*this, cod->cod_type());
}

GCRef r5rs::Interpreter::operator()(expression::Datum* datum) {
  return std::visit(*this, datum->datum_type());
}

GCRef r5rs::Interpreter::operator()(expression::Definition* def) {
  return std::visit(*this, def->def_type());
}

GCRef r5rs::Interpreter::operator()(expression::Exp* exp) {
  return std::visit(*this, exp->exp_type());
}

GCRef r5rs::Interpreter::operator()(expression::CODs* cods) {
  assert(!cods->cods.empty());
  auto start = cods->cods.begin();
  auto back = std::prev(cods->cods.end());
  while (start != back) {
    std::visit(*this, (*start)->cod_type());
    ++start;
  }
  return std::visit(*this, (*start)->cod_type());
}

GCRef r5rs::Interpreter::operator()(expression::SimpleDatum* datum) {
  return std::visit([](auto v) -> GCValue { return std::move(v); }, datum->value);
}

GCRef r5rs::Interpreter::operator()(expression::ListDatum* list) {
  GCRef head = nullptr;
  for (auto it = list->list.rbegin(); it != list->list.rend(); ++it) {
    head = Pair{ std::invoke(*this, it->get()), head };
  }
  return head;
}

GCRef r5rs::Interpreter::operator()(expression::VectorDatum* vec) {
  Vector res;
  for (auto&& datum : vec->list) {
    res.push_back(std::invoke(*this, datum.get()));
  }
  return std::move(res);
}

GCRef r5rs::Interpreter::operator()(expression::Define* def) {
  auto&& vars = env->variables;

  if (vars.find(def->variable) != vars.end()) {
    throw std::runtime_error("redefine variable '" + def->variable + "'!");
  }

  vars.insert({ def->variable, std::invoke(*this, def->exp.get()) });

  return nullptr;
}

GCRef r5rs::Interpreter::operator()(expression::Definitions* defs) {
  push();
  for (auto&& def : defs->defs) {
    std::invoke(*this, def.get());
  }
  pop();
  return nullptr;
}

GCRef r5rs::Interpreter::operator()(expression::Variable* var) {
  auto&& res = env->get(var->id);
  if (!res) {
    throw std::runtime_error("variable " + var->id + " is not defined!");
  }
  return *res;
}

GCRef r5rs::Interpreter::operator()(expression::Literal* literal) {
  auto visitor = overloaded{ [this](DatumPtr datum) -> GCRef {
                              return std::invoke(*this, datum.get());
                            },
                            [](auto v) -> GCRef { return std::move(v); } };

  return std::visit(visitor, literal->value);
}

GCRef r5rs::Interpreter::operator()(expression::Call* call) {
  auto op = std::invoke(*this, call->op.get());

  std::list<GCRef> args;
  for (auto&& exp : call->operands) {
    args.push_back(std::invoke(*this, exp.get()));
  }

  auto visitor = overloaded{
      [&args](Primitive primitive) -> GCRef {
        return std::invoke(*primitive, args);
      },
      [&args, this](ClosureLambda lambda) -> GCRef {
        auto e = this->env;

        this->env = lambda.env;
        this->env = std::make_shared<Env>(*lambda.lambda->formals, args, env);
        auto res = std::invoke(*this, lambda.lambda->body.get());

        this->env = e;
        return res;
      },
      [](auto&&) -> GCRef {
        throw std::runtime_error("expression is not a function");
      } };

  return std::visit(visitor, *op);
}

GCRef r5rs::Interpreter::operator()(expression::Lambda* lambda) {
  return ClosureLambda{ lambda, env };
}

GCRef r5rs::Interpreter::operator()(expression::Conditional* condition) {
  auto cond = std::invoke(*this, condition->test.get());
  auto visitor = overloaded{
      [](nullptr_t) -> bool { return false; },
      [](bool b) -> bool { return b; },
      [](auto&&) -> bool { return true; },
  };

  if (std::visit(visitor, *cond)) {
    return std::invoke(*this, condition->consequent.get());
  }
  else {
    return std::invoke(*this, condition->alternate.get());
  }
}

GCRef r5rs::Interpreter::operator()(expression::Assignment* assign) {
  auto var = env->get(assign->variable);
  if (!var) {
    throw std::runtime_error("variable not found");
  }
  **var = *std::invoke(*this, assign->exp.get());
  return nullptr;
}

GCRef r5rs::Interpreter::operator()(expression::Body* body) {
  if (body->exps.empty()) {
    throw std::runtime_error("empty body!");
  }

  for (auto&& def : body->defs) {
    std::invoke(*this, def.get());
  }

  auto it = body->exps.begin();
  auto back = std::prev(body->exps.end());

  for (; it != back; ++it) {
    std::invoke(*this, it->get());
  }

  return std::invoke(*this, back->get());
}
