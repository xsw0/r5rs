#ifndef _R5RS_EXPRESSIONS_H_
#define _R5RS_EXPRESSIONS_H_

#include <string>
#include <vector>
#include <list>
#include <optional>

#include "Token.h"
#include "Parse.h"

namespace r5rs
{
  class Begin;
  class Definition;
  class DefFormals;
  class SyntaxDefinition;

  class Datum;
  class Symbol;

  class Variable;
  class Identifier;

  class Expression;

  class Literal;
  class SelfEvaluating;
  class Quotation;
  class ProcedureCall;

  class LambdaExpression;
  class Formals;
  class Body;
  class Sequence;
  class Command;

  class Conditional;
  class Test;
  class Consequent;
  class Alternate;

  class Assignment;
  class DerivedExpression;

  class CondClause;
  class Recipient;
  class CaseClause;
  class BindingSpec;
  class IterationSpec;
  class Init;
  class Step;
  class DoResult;
  class MacroUse;
  class Keyword;
  class MacroBlock;
  class SyntaxSpec;
  class Symbol
  {
  public:
    std::string name;
  };

  class Datum
  {
  public:
    using value_t = std::variant<
      bool,
      char,
      int64_t,
      std::string,
      Symbol,
      std::shared_ptr<std::vector<Datum>>,
      std::shared_ptr<std::list<Datum>>
    >;
    value_t value;
  };

  class Expression
  {
  public:
    virtual ~Expression() = default;
  };

  class Variable: public Expression
  {
  public:
    Variable(Token token)
    {
      assert(token.type == TokenType::variable);
      name = std::get<std::string>(token.value);
    }
    std::string name;
  };

  class Literal: public Expression
  {
  public:
    using value_t = std::variant<
      nullptr_t,
      bool,
      char,
      int64_t,
      std::string,
      Datum
    >;

    Literal(Quotation quotation);
    Literal(Token tok);

    value_t value;
  };

  class ProcedureCall: public Expression
  {
  public:
    std::shared_ptr<Expression> op;
    std::shared_ptr<std::list<Expression>> operands;
  };

  class LambdaExpression: public Expression
  {
  };

  class Formals
  {
  public:
    std::list<Variable> variables;
    std::optional<Variable> var;
  };

  class Conditional: public Expression
  {
  };

  class Assignment: public Expression
  {
  };

  class DerivedExpression: public Expression
  {
  };

  class MacroUse: public Expression
  {
  };

  class MacroBlock: public Expression
  {
  };

  class Body {};

  class Definitions;

  class Definition
  {
  public:
    struct Func
    {
      Variable var;
      std::shared_ptr<DefFormals> formals;
      std::shared_ptr<Body> body;
    };
    struct Var
    {
      Variable var;
      Expression value;
    };
    using value_t = std::variant<Var, Func, std::shared_ptr<Definitions>>;
    value_t value;
  };

  class Definitions
  {
  public:
    std::list<Definition> defs;
  };

  class DefFormals
  {
  public:
    std::list<Variable> variables;
    std::optional<Variable> var;
  };

  class SyntaxDefinition
  {

  };

  class Quotation
  {
  public:
    Datum datum;
  };

  class SelfEvaluating
  {
  public:
    using value_t = std::variant<bool, char, int64_t, std::string>;
    value_t value;
  };

  class Command
  {
  public:
    Expression exp;
  };

  class Begin {};

  class CODs;

  using COD = std::variant<
    Command,
    Definition,
    SyntaxDefinition,
    std::shared_ptr<CODs>
  >;

  class CODs
  {
  public:
    std::list<COD> cods;
  };

  using Program = std::list<COD>;

  Parser<Token, Program> program();
  IStream<Expression> expressions();

  namespace def
  {
    Parser<Token, COD> cod();
  } // namespace def

  namespace exp
  {
    template<TokenType ... T>
    inline Parser<Token, Token> match()
    {
      return make_function(
        [=](IStream<Token> input) -> ParserResult<Token, Token> {
          if (input.eof()) { return Error{ "eof", input.current() }; }
          if ((... || (T == input[0]->type)))
          {
            return std::make_pair(*input[0], input + 1);
          }
          { return Error{ "match fail.", input.current() }; }
        }
      );
    }

    Parser<Token, Datum> datum();
    Parser<Token, Variable> variable();

    Parser<Token, Expression> expression();

    Parser<Token, Literal> literal();
    Parser<Token, Token> self_evaluating();
    Parser<Token, Quotation> quotation();
    Parser<Token, ProcedureCall> procedureCall();

    Parser<Token, LambdaExpression> lambdaExpression();
    Parser<Token, Formals> formals();
    Parser<Token, Conditional> conditional();
    Parser<Token, Assignment> assignment();
    Parser<Token, DerivedExpression> derivedExpression();
    Parser<Token, MacroUse> macroUse();
    Parser<Token, MacroBlock> macroBlock();
    Parser<Token, Command> command();
    Parser<Token, Definition> definition();
    Parser<Token, DefFormals> defFormals();
    Parser<Token, Body> body();

  } // namespace exp

} // namespace r5rs

#endif
