#include "Expressions.h"

#include "Lex.h"

using namespace r5rs;
using namespace expression;

Parser<Token, nullptr_t> r5rs::expression::match(Keyword keyword)
{
  return match<std::string>(TokenType::identifier).filter(
    make_function(
      [=](std::string id) -> bool {
        return to_string(keyword) == id;
      }
    )
  ).as(nullptr);
}

Parser<Token, std::string> r5rs::expression::variable()
{
  static auto parser = match<std::string>(TokenType::identifier).filter(
    make_function(
      [=](std::string id) -> bool {
        return keywords().find(id) == keywords().end();
      }
    )
  );
  return parser;
}

Parser<Token, DatumPtr> r5rs::expression::simpleDatum()
{
  static auto parser =
    (
      match<bool>(TokenType::boolean).as<SimpleDatum>() ||
      match<int64_t>(TokenType::number).as<SimpleDatum>() ||
      match<char>(TokenType::character).as<SimpleDatum>() ||
      match<std::string>(TokenType::string).as<SimpleDatum>() ||
      match<std::string>(TokenType::identifier).as<Datum::Symbol>().as<SimpleDatum>()
    )
    .shared().as<DatumPtr>();
  return parser;
}

Parser<Token, DatumPtr> r5rs::expression::listDatum()
{
  static auto parser =
    select<1>(
      match<>(TokenType::left_paren),
      datum().many(),
      match<>(TokenType::right_paren)
    )
    .as<ListDatum>().shared().as<DatumPtr>();
  return parser;
}

Parser<Token, DatumPtr> r5rs::expression::vectorDatum()
{
  static auto parser =
    select<1>(
      match<>(TokenType::vector_paren),
      datum().many(),
      match<>(TokenType::right_paren)
    )
    .as<VectorDatum>().shared().as<DatumPtr>();
  return parser;
}

Parser<Token, std::shared_ptr<Datum>> r5rs::datum()
{
  return simpleDatum() || listDatum() || vectorDatum();
}

Parser<Token, std::shared_ptr<Exp>> r5rs::exp()
{
  using ExpPtr = std::shared_ptr<Exp>;
  static auto parser =
    variable().as<Variable>().shared().as<ExpPtr>() ||
    literal().shared().as<ExpPtr>() ||
    call().shared().as<ExpPtr>() ||
    lambda().shared().as<ExpPtr>() ||
    conditional().shared().as<ExpPtr>() ||
    assignment().shared().as<ExpPtr>();
  return parser;
}

Parser<Token, Program> r5rs::program()
{
  static auto parser = select<0>(cod().many().as<CODs>(), match<>(TokenType::eof));
  return parser;
}

Parser<Token, DatumPtr> r5rs::expression::quotation()
{
  static auto parser =
    select<1>(
      match(TokenType::quote_symbol),
      datum()
    ) ||
    select<2>(
      match(TokenType::left_paren),
      match(Keyword::quote),
      datum(),
      match(TokenType::right_paren)
    );
  return parser;
}

Parser<Token, Literal> r5rs::expression::literal()
{
  static auto parser =
    match<bool>(TokenType::boolean).as<Literal>() ||
    match<int64_t>(TokenType::number).as<Literal>() ||
    match<char>(TokenType::character).as<Literal>() ||
    match<std::string>(TokenType::string).as<Literal>() ||
    quotation().as<Literal>();
  return parser;
}

Parser<Token, Call> r5rs::expression::call()
{
  static auto ctor = make_function(
    [](nullptr_t, ExpPtr op, std::list<ExpPtr> ops, nullptr_t) {
      return Call{ op, ops };
    }
  );
  static auto parser =
    combine(
      ctor,
      match<>(TokenType::left_paren),
      exp(),
      exp().many(),
      match<>(TokenType::right_paren)
    );
  return parser;
}

Parser<Token, Formals> r5rs::expression::formals()
{
  static auto fixed_ctor = make_function(
    [](std::list<std::string> fixed) {
      return Formals{ std::move(fixed) };
    }
  );
  static auto binding_ctor = make_function(
    [](std::string binding) {
      return Formals{ {}, std::move(binding) };
    }
  );
  static auto both_ctor = make_function(
    [](
      nullptr_t,
      std::list<std::string> fixed,
      nullptr_t,
      std::string binding,
      nullptr_t
      ) {
        return Formals{ std::move(fixed), std::move(binding) };
    }
  );
  static auto parser =
    select<1>(
      match(TokenType::left_paren),
      variable().many(),
      match(TokenType::right_paren)
    ).map(fixed_ctor) ||
    variable().map(binding_ctor) ||
    combine(
      both_ctor,
      match(TokenType::left_paren),
      variable().some(),
      match(TokenType::dot),
      variable(),
      match(TokenType::right_paren)
    );
  return parser;
}

Parser<Token, Formals> r5rs::expression::defFormals()
{
  static auto ctor = make_function(
    [](
      std::list<std::string> fixed,
      std::optional<std::string> binding
      ) {
        return Formals{ std::move(fixed), std::move(binding) };
    }
  );
  static auto parser =
    combine(
      ctor,
      variable().many(),
      select<1>(
        match(TokenType::dot),
        variable()
      ).as<std::optional<std::string>>().otherwise(std::nullopt)
    );
  return parser;
}

Parser<Token, Body> r5rs::expression::body()
{
  static auto ctor = make_function(
    [](std::list<DefinitionPtr> defs, std::list<ExpPtr> exps) {
      return Body{ std::move(defs), std::move(exps) };
    }
  );
  static auto parser =
    combine(
      ctor,
      definition().many(),
      exp().some()
    );
  return parser;
}

Parser<Token, std::shared_ptr<Definition>> r5rs::definition()
{
  static auto var_ctor = make_function(
    [](std::string name, ExpPtr exp) {
      return Define{ std::move(name), exp };
    }
  );

  static auto fun_ctor = make_function(
    [](
      nullptr_t,
      std::string name,
      Formals formals,
      nullptr_t,
      Body body
      ) {
        auto lambda = std::make_shared<Lambda>(
          std::make_shared<Formals>(std::move(formals)),
          std::make_shared<Body>(std::move(body))
        );
        return Define{ std::move(name), lambda };
      }
  );

  static auto fun_parser =
    combine(
      fun_ctor,
      match(TokenType::left_paren),
      variable(),
      defFormals(),
      match(TokenType::right_paren),
      body()
    );

  static auto var_parser =
    combine(
      var_ctor,
      variable(),
      exp()
    );

  static auto parser =
    select<2>(
      match(TokenType::left_paren),
      match(Keyword::define),
      (fun_parser || var_parser).shared().as<DefinitionPtr>(),
      match(TokenType::right_paren)
    ) || definitions();
  return parser;
}

Parser<Token, Lambda> r5rs::expression::lambda()
{
  static auto ctor = make_function(
    [](Formals formals, Body body) {
      return Lambda{
        std::make_shared<Formals>(std::move(formals)),
        std::make_shared<Body>(std::move(body))
      };
    }
  );
  static auto parser =
    select<2>(
      match(TokenType::left_paren),
      match(Keyword::lambda),
      combine(
        ctor,
        formals(),
        body()
      ),
      match(TokenType::right_paren)
    );
  return parser;
}

Parser<Token, Conditional> r5rs::expression::conditional()
{
  static auto ctor = make_function(
    [](ExpPtr test, ExpPtr consequent, ExpPtr alternate) {
      return Conditional{ test, consequent, alternate };
    }
  );
  static auto parser =
    select<2>(
      match(TokenType::left_paren),
      match(Keyword::If),
      combine(
        ctor,
        exp(),
        exp(),
        exp().otherwise(nullptr)
      ),
      match(TokenType::right_paren)
    );
  return parser;
}

Parser<Token, Assignment> r5rs::expression::assignment()
{
  static auto ctor = make_function(
    [](std::string var, ExpPtr exp) {
      return Assignment{ std::move(var), exp };
    }
  );
  static auto parser =
    select<2>(
      match(TokenType::left_paren),
      match(Keyword::set_),
      combine(
        ctor,
        variable(),
        exp()
      ),
      match(TokenType::right_paren)
    );
  return parser;
}

Parser<Token, DefinitionPtr> r5rs::expression::definitions()
{
  static auto parser =
    select<2>(
      match(TokenType::left_paren),
      match(Keyword::begin),
      definition().many(),
      match(TokenType::right_paren)
    ).as<Definitions>().shared().as<DefinitionPtr>();
  return parser;
}

Parser<Token, CODPtr> r5rs::expression::cod()
{
  static auto parser =
    exp().as<CODPtr>() ||
    definition().as<CODPtr>() ||
    cods().shared().as<CODPtr>();
  return parser;
}

Parser<Token, CODs> r5rs::expression::cods()
{
  static auto parser =
    select<2>(
      match(TokenType::left_paren),
      match(Keyword::begin),
      cod().many(),
      match(TokenType::right_paren)
    ).as<CODs>();
  return parser;
}
