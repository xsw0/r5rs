#ifndef R5RS_INTERPRETER_H
#define R5RS_INTERPRETER_H

#include "Env.h"
#include "Expressions.h"
#include "GC.h"
#include "Type.h"

namespace r5rs {
  class Interpreter {
  public:
    Interpreter();

    std::shared_ptr<Env> env;
    void push();
    void pop();

    // forward
    GCRef operator()(expression::COD*);
    GCRef operator()(expression::Datum*);
    GCRef operator()(expression::Definition*);
    GCRef operator()(expression::Exp*);

    // cods
    GCRef operator()(expression::CODs*);

    // Datum
    GCRef operator()(expression::SimpleDatum*);
    GCRef operator()(expression::ListDatum*);
    GCRef operator()(expression::VectorDatum*);

    // Definition
    GCRef operator()(expression::Define*);
    GCRef operator()(expression::Definitions*);

    // Exp
    GCRef operator()(expression::Variable*);
    GCRef operator()(expression::Literal*);
    GCRef operator()(expression::Call*);
    GCRef operator()(expression::Lambda*);
    GCRef operator()(expression::Conditional*);
    GCRef operator()(expression::Assignment*);

    GCRef operator()(expression::Body*);
  };
} // namespace r5rs

#endif
