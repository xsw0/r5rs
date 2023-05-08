#include "Expressions.h"

#include <memory>

using namespace r5rs;
using namespace r5rs::exp;
using namespace r5rs::def;

r5rs::Literal::Literal(Quotation quotation): value(quotation.datum) {}

r5rs::Literal::Literal(Token tok)
{
  assert(!std::holds_alternative<nullptr_t>(tok.value));
  value = std::visit(
    [] <typename T> (T && val) -> value_t { return std::forward<T>(val); },
    tok.value
  );
}

Parser<Token, Variable> r5rs::exp::variable()
{
  static auto parser = match<TokenType::variable>().as<Variable>();
  return parser;
}

Parser<Token, Expression> r5rs::exp::expression()
{
  static auto parser =
    variable().as<Expression>() ||
    literal().as<Expression>() ||
    procedureCall().as<Expression>() ||
    lambdaExpression().as<Expression>() ||
    conditional().as<Expression>() ||
    assignment().as<Expression>() ||
    derivedExpression().as<Expression>() ||
    macroUse().as<Expression>() ||
    macroBlock().as<Expression>();
  return parser;
}

Parser<Token, Literal> r5rs::exp::literal()
{
  static auto parser = quotation().as<Literal>() || self_evaluating().as<Literal>();
  return parser;
}

Parser<Token, Token> r5rs::exp::self_evaluating()
{
  static auto parser =
    match<TokenType::boolean, TokenType::character, TokenType::number, TokenType::string>();
  return parser;
}

Parser<Token, Quotation> r5rs::exp::quotation()
{
  static auto parser = (
    select<1>(
      match<TokenType::quote_symbol>(),
      datum()
    ) ||
    select<2>(
      match<TokenType::left_paren>(),
      match<TokenType::quote>(),
      datum(),
      match<TokenType::right_paren>()
    )
  ).as<Quotation>();
  return parser;
}

Parser<Token, ProcedureCall> r5rs::exp::procedureCall()
{
  static auto pc = make_function(
    [](Expression op, std::list<Expression> ops) -> ProcedureCall {
      ProcedureCall call;
      call.op = std::make_shared<Expression>(op);
      call.operands = std::make_shared<std::list<Expression>>(ops);
      return call;
    }
  );

  static auto parser = select<1>(
    match<TokenType::left_paren>(),
    combine(
      pc,
      expression(),
      expression().many()
    ),
    match<TokenType::right_paren>()
  );
  return parser;
}

Parser<Token, LambdaExpression> r5rs::exp::lambdaExpression()
{
  // (lambda <formals> <body>)
  static auto cmb = make_function(
    [](Formals formals, Body body) -> LambdaExpression {}
  );
  static auto parser = select<2>(
    match<TokenType::left_paren>(),
    match<TokenType::lambda>(),
    combine(cmb, formals(), body()),
    match<TokenType::right_paren>()
  );
  return parser;
}

// Parser<Token, Formals> r5rs::exp::formals()
// {
//   static auto parser =
//   return parser;
// }

Parser<Token, Program> r5rs::program()
{
  static auto parser = cod().many();
  return parser;
}

Parser<Token, COD> r5rs::def::cod()
{
  static auto parser =
    command().as<COD>() ||
    definition().as<COD>() ||
    select<2>(
      match<TokenType::left_paren>(),
      match<TokenType::begin>(),
      cod().many().as<CODs>().shared(),
      match<TokenType::right_paren>()
    ).as<COD>();
  return parser;
}

Parser<Token, Command> r5rs::exp::command()
{
  static auto parser = expression().as<Command>();
  return parser;
}

Parser<Token, Definition> r5rs::exp::definition()
{
  static auto f = make_function(
    [](Variable v, DefFormals df, Token, Body body) -> Definition {
      return Definition{
        Definition::Func{v, std::make_shared<DefFormals>(df), std::make_shared<Body>(body)}
      };
    }
  );

  static auto fun = select<2>(
    match<TokenType::define>(),
    match<TokenType::left_paren>(),
    combine(
      f,
      variable(),
      defFormals(),
      match<TokenType::right_paren>(),
      body()
    )
  );

  static auto v = make_function(
    [](Variable v, Expression exp) -> Definition {
      return Definition{ Definition::Var{v, exp} };
    }
  );

  static auto var = select<1>(
    match<TokenType::define>(),
    combine(
      v,
      variable(),
      expression()
    )
  );

  static auto defs = select<1>(
    match<TokenType::begin>(),
    definition().many().as<Definitions>().shared().as<Definition>()
  );

  static auto parser = select<1>(
    match<TokenType::left_paren>(),
    defs || fun || var,
    match<TokenType::right_paren>()
  );

  return parser;
}

Parser<Token, DefFormals> r5rs::exp::defFormals()
{
  static auto cmb = make_function(
    [](std::list<Variable> vars, std::optional<Variable> var) {
      return DefFormals{ vars, var };
    }
  );
  static auto parser = combine(
    cmb,
    variable().many(),
    variable().maybe()
  );

  return parser;
}
