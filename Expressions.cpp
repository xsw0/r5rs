#include "Expressions.h"

#include <mutex>
#include "Lex.h"

using namespace r5rs;
using namespace expression;

ParserPtr<Token, std::string> r5rs::expression::variable()
{
  static auto parser = make_parser<Token, std::string>();
  return parser;
}

ParserPtr<Token, DatumPtr> r5rs::expression::simpleDatum()
{
  static auto parser = make_parser<Token, DatumPtr>();
  return parser;
}

ParserPtr<Token, DatumPtr> r5rs::expression::listDatum()
{
  static auto parser = make_parser<Token, DatumPtr>();
  return parser;
}

ParserPtr<Token, DatumPtr> r5rs::expression::vectorDatum()
{
  static auto parser = make_parser<Token, DatumPtr>();
  return parser;
}

ParserPtr<Token, std::shared_ptr<Datum>> r5rs::datum()
{
  static auto parser = make_parser<Token, DatumPtr>();
  return parser;
}

ParserPtr<Token, std::shared_ptr<Definition>> r5rs::definition()
{
  static auto parser = make_parser<Token, DefinitionPtr>();
  return parser;
}

ParserPtr<Token, DatumPtr> r5rs::expression::quotation()
{
  static auto parser = make_parser<Token, DatumPtr>();
  return parser;
}

ParserPtr<Token, Literal> r5rs::expression::literal()
{
  static auto parser = make_parser<Token, Literal>();
  return parser;
}

ParserPtr<Token, Call> r5rs::expression::call()
{
  static auto parser = make_parser<Token, Call>();
  return parser;
}

ParserPtr<Token, Formals> r5rs::expression::formals()
{
  static auto parser = make_parser<Token, Formals>();
  return parser;
}

ParserPtr<Token, Formals> r5rs::expression::defFormals()
{
  static auto parser = make_parser<Token, Formals>();
  return parser;
}

ParserPtr<Token, Body> r5rs::expression::body()
{
  static auto parser = make_parser<Token, Body>();
  return parser;
}

ParserPtr<Token, Lambda> r5rs::expression::lambda()
{
  static auto parser = make_parser<Token, Lambda>();
  return parser;
}

ParserPtr<Token, Conditional> r5rs::expression::conditional()
{
  static auto parser = make_parser<Token, Conditional>();
  return parser;
}

ParserPtr<Token, Assignment> r5rs::expression::assignment()
{
  static auto parser = make_parser<Token, Assignment>();
  return parser;
}

ParserPtr<Token, DefinitionPtr> r5rs::expression::definitions()
{
  static auto parser = make_parser<Token, DefinitionPtr>();
  return parser;
}

ParserPtr<Token, CODPtr> r5rs::expression::cod()
{
  static auto parser = make_parser<Token, CODPtr>();
  return parser;
}

ParserPtr<Token, CODs> r5rs::expression::cods()
{
  static auto parser = make_parser<Token, CODs>();
  return parser;
}

ParserPtr<Token, std::shared_ptr<Exp>> r5rs::exp()
{
  static auto parser = make_parser<Token, ExpPtr>();
  return parser;
}

// ParserPtr<Token, Program> r5rs::program()
// {
//   static auto parser = make_parser<Token, Program>();
//   return parser;
// }

ParserPtr<Token, nullptr_t> r5rs::expression::match(Keyword keyword)
{
  return match<std::string>(TokenType::identifier)->filter(
    make_function(
      [=](std::string id) -> bool {
        return to_string(keyword) == id;
      }
    )
  )->as(nullptr);
}

namespace
{
  void variableInit()
  {
    *variable() =
      *match<std::string>(TokenType::identifier)->filter(
        make_function(
          [=](std::string id) -> bool {
            return keywords().find(id) == keywords().end();
          }
        )
      );
  }

  void simpleDatumInit()
  {
    *simpleDatum() = *(
      match<bool>(TokenType::boolean)->as<SimpleDatum>() ||
      match<int64_t>(TokenType::number)->as<SimpleDatum>() ||
      match<char>(TokenType::character)->as<SimpleDatum>() ||
      match<std::string>(TokenType::string)->as<SimpleDatum>() ||
      match<std::string>(TokenType::identifier)->as<Symbol>()->as<SimpleDatum>()
      )->shared()->as<DatumPtr>();
  }

  void listDatumInit()
  {
    *listDatum() =
      *select<1>(
        match<>(TokenType::left_paren),
        datum()->many(),
        match<>(TokenType::right_paren)
      )->as<ListDatum>()->shared()->as<DatumPtr>();
  }

  void vectorDatumInit()
  {
    *vectorDatum() =
      *select<1>(
        match<>(TokenType::vector_paren),
        datum()->many(),
        match<>(TokenType::right_paren)
      )->as<VectorDatum>()->shared()->as<DatumPtr>();
  }

  void datumInit()
  {
    *datum() = *(simpleDatum() || listDatum() || vectorDatum());
  }

  void expInit()
  {
    *exp() = *(
      variable()->as<Variable>()->shared()->as<ExpPtr>() ||
      literal()->shared()->as<ExpPtr>() ||
      call()->shared()->as<ExpPtr>() ||
      lambda()->shared()->as<ExpPtr>() ||
      conditional()->shared()->as<ExpPtr>() ||
      assignment()->shared()->as<ExpPtr>()
    );
  }

  // void programInit()
  // {
  //   *program() = *select<0>(cod()->many()->as<CODs>(), match<>(TokenType::eof));
  // }

  void quotationInit()
  {
    *quotation() = *(
      select<1>(
        match(TokenType::quote_symbol),
        datum()
      ) ||
      select<2>(
        match(TokenType::left_paren),
        match(Keyword::quote),
        datum(),
        match(TokenType::right_paren)
      )
    );
  }

  void literalInit()
  {
    *literal() = *(
      match<bool>(TokenType::boolean)->as<Literal>() ||
      match<int64_t>(TokenType::number)->as<Literal>() ||
      match<char>(TokenType::character)->as<Literal>() ||
      match<std::string>(TokenType::string)->as<Literal>() ||
      quotation()->as<Literal>()
    );
  }

  void callInit()
  {
    static auto ctor = make_function(
      [](nullptr_t, ExpPtr op, std::list<ExpPtr> ops, nullptr_t) {
        return Call{ op, ops };
      }
    );
    *call() = *combine(
      ctor,
      match<>(TokenType::left_paren),
      exp(),
      exp()->many(),
      match<>(TokenType::right_paren)
    );
  }

  void formalsInit()
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
    *formals() = *(
      select<1>(
        match(TokenType::left_paren),
        variable()->many(),
        match(TokenType::right_paren)
      )->map(fixed_ctor) ||
      variable()->map(binding_ctor) ||
      combine(
        both_ctor,
        match(TokenType::left_paren),
        variable()->some(),
        match(TokenType::dot),
        variable(),
        match(TokenType::right_paren)
      )
    );
  }

  void defFormalsInit()
  {
    static auto ctor = make_function(
      [](
        std::list<std::string> fixed,
        std::optional<std::string> binding
        ) {
          return Formals{ std::move(fixed), std::move(binding) };
        }
    );
    *defFormals() = *combine(
      ctor,
      variable()->many(),
      select<1>(
        match(TokenType::dot),
        variable()
      )->as<std::optional<std::string>>()->otherwise(std::nullopt)
    );
  }

  void bodyInit()
  {
    static auto ctor = make_function(
      [](std::list<DefinitionPtr> defs, std::list<ExpPtr> exps) {
        return Body{ std::move(defs), std::move(exps) };
      }
    );
    *body() = *combine(
      ctor,
      definition()->many(),
      exp()->some()
    );
  }

  void definitionInit()
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

    *definition() = *(
      select<2>(
        match(TokenType::left_paren),
        match(Keyword::define),
        (fun_parser || var_parser)->shared()->as<DefinitionPtr>(),
        match(TokenType::right_paren)
      ) || definitions()
    );
  }

  void lambdaInit()
  {
    static auto ctor = make_function(
      [](Formals formals, Body body) {
        return Lambda{
          std::make_shared<Formals>(std::move(formals)),
          std::make_shared<Body>(std::move(body))
        };
      }
    );
    *lambda() = *select<2>(
      match(TokenType::left_paren),
      match(Keyword::lambda),
      combine(
        ctor,
        formals(),
        body()
      ),
      match(TokenType::right_paren)
    );
  }

  void conditionalInit()
  {
    static auto ctor = make_function(
      [](ExpPtr test, ExpPtr consequent, ExpPtr alternate) {
        return Conditional{ test, consequent, alternate };
      }
    );
    *conditional() = *select<2>(
      match(TokenType::left_paren),
      match(Keyword::If),
      combine(
        ctor,
        exp(),
        exp(),
        exp()->otherwise(nullptr)
      ),
      match(TokenType::right_paren)
    );
  }

  void assignmentInit()
  {
    static auto ctor = make_function(
      [](std::string var, ExpPtr exp) {
        return Assignment{ std::move(var), exp };
      }
    );
    *assignment() = *select<2>(
      match(TokenType::left_paren),
      match(Keyword::set_),
      combine(
        ctor,
        variable(),
        exp()
      ),
      match(TokenType::right_paren)
    );
  }

  void definitionsInit()
  {
    *definitions() = *select<2>(
      match(TokenType::left_paren),
      match(Keyword::begin),
      definition()->many(),
      match(TokenType::right_paren)
    )->as<Definitions>()->shared()->as<DefinitionPtr>();
  }

  void codInit()
  {
    *cod() = *(
      exp()->as<CODPtr>() ||
      definition()->as<CODPtr>() ||
      cods()->shared()->as<CODPtr>()
    );
  }

  void codsInit()
  {
    *cods() = *select<2>(
      match(TokenType::left_paren),
      match(Keyword::begin),
      cod()->many(),
      match(TokenType::right_paren)
    )->as<CODs>();
  }

} // namespace

void r5rs::expression::init()
{
  variableInit();
  simpleDatumInit();
  listDatumInit();
  vectorDatumInit();
  datumInit();
  expInit();
  // programInit();
  quotationInit();
  literalInit();
  callInit();
  formalsInit();
  defFormalsInit();
  bodyInit();
  definitionInit();
  lambdaInit();
  conditionalInit();
  assignmentInit();
  definitionsInit();
  codInit();
  codsInit();

  assert(*variable());
  assert(*simpleDatum());
  assert(*listDatum());
  assert(*vectorDatum());
  assert(*datum());
  assert(*exp());
  // assert(*program());
  assert(*quotation());
  assert(*literal());
  assert(*call());
  assert(*formals());
  assert(*defFormals());
  assert(*body());
  assert(*definition());
  assert(*lambda());
  assert(*conditional());
  assert(*assignment());
  assert(*definitions());
  assert(*cod());
  assert(*cods());
}

IStream<expression::CODPtr> r5rs::ast(IStream<Token> input)
{
  static std::once_flag flag;
  std::call_once(flag, []() { init(); });

  return IStream<CODPtr>(
    make_function(
      [input]() mutable -> Try<CODPtr> {
        auto && res = std::invoke(*cod(), input);
        if (!res) { return Error{ "error." }; }
        input = res->second;
        return res->first;
      }
    )
  );
}
