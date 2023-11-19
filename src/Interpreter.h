#ifndef _R5RS_INTERPRETER_H_
#define _R5RS_INTERPRETER_H_

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
  Reference operator()(expression::COD *);
  Reference operator()(expression::Datum *);
  Reference operator()(expression::Definition *);
  Reference operator()(expression::Exp *);

  // cods
  Reference operator()(expression::CODs *);

  // Datum
  Reference operator()(expression::SimpleDatum *);
  Reference operator()(expression::ListDatum *);
  Reference operator()(expression::VectorDatum *);

  // Definition
  Reference operator()(expression::Define *);
  Reference operator()(expression::Definitions *);

  // Exp
  Reference operator()(expression::Variable *);
  Reference operator()(expression::Literal *);
  Reference operator()(expression::Call *);
  Reference operator()(expression::Lambda *);
  Reference operator()(expression::Conditional *);
  Reference operator()(expression::Assignment *);

  Reference operator()(expression::Body *);
};
} // namespace r5rs

#endif
