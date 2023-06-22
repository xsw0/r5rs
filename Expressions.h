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
  namespace expression
  {
    // command or definition
    struct COD
    {
      virtual ~COD() = default;
    };

    using CODPtr = std::shared_ptr<COD>;

    struct Datum
    {
      struct Symbol
      {
        std::string name;
      };

      virtual ~Datum() = default;
    };

    using DatumPtr = std::shared_ptr<Datum>;

    struct Definition: COD
    {
      ~Definition() override = default;
    };

    using DefinitionPtr = std::shared_ptr<Definition>;

    struct Exp: COD
    {
      ~Exp() override = default;
    };

    using ExpPtr = std::shared_ptr<Exp>;

    class CODs: public COD
    {
    public:
      std::list<std::shared_ptr<COD>> cods;
      explicit CODs(std::list<CODPtr> cods): cods(std::move(cods)) {}
    };

    class SimpleDatum: public Datum
    {
    public:
      using value_t = std::variant<
        char,
        bool,
        int64_t,
        std::string,
        Symbol
      >;
      explicit SimpleDatum(value_t value): value(std::move(value)) {}

      value_t value;
    };

    class ListDatum: public Datum
    {
    public:
      std::list<DatumPtr> list;
      explicit ListDatum(std::list<DatumPtr> list): list(std::move(list)) {}
    };

    class VectorDatum: public Datum
    {
    public:
      std::list<DatumPtr> list;
      explicit VectorDatum(std::list<DatumPtr> list): list(std::move(list)) {}
    };

    class Variable: public Exp
    {
    public:
      std::string id;
      explicit Variable(std::string id): id(std::move(id)) {}
    };

    class Literal: public Exp
    {
    public:
      using value_t = std::variant<
        DatumPtr,
        bool,
        char,
        int64_t,
        std::string
      >;

      value_t value;
      explicit Literal(value_t value): value(std::move(value)) {}
    };

    class Call: public Exp
    {
    public:
      ExpPtr op;
      std::list<ExpPtr> operands;

      explicit Call(ExpPtr op, std::list<ExpPtr> ops): op(op), operands(std::move(ops)) {}
    };

    class Formals
    {
    public:
      std::list<std::string> fixed;
      std::optional<std::string> binding;
      explicit Formals(
        std::list<std::string> fixed = {},
        std::optional<std::string> binding = std::nullopt
      ): fixed{ std::move(fixed) }, binding{ std::move(binding) }
      {
      }
    };

    class Body
    {
    public:
      std::list<DefinitionPtr> defs;
      std::list<ExpPtr> exps;
      explicit Body(
        std::list<DefinitionPtr> defs,
        std::list<ExpPtr> exps
      ): defs(std::move(defs)), exps(std::move(exps))
      {
      }
    };

    class Lambda: public Exp
    {
    public:
      std::shared_ptr<Formals> formals;
      std::shared_ptr<Body> body;
      explicit Lambda(
      std::shared_ptr<Formals> formals,
      std::shared_ptr<Body> body
      ): formals(formals), body(body)
      {
      }
    };

    class Conditional: public Exp
    {
    public:
      ExpPtr test;
      ExpPtr consequent;
      ExpPtr alternate;
      explicit Conditional(
        ExpPtr test,
        ExpPtr consequent,
        ExpPtr alternate
      ): test(test), consequent(consequent), alternate(alternate)
      {
      }
    };

    class Assignment: public Exp
    {
    public:
      std::string variable;
      ExpPtr exp;
      explicit Assignment(
        std::string variable,
        ExpPtr exp
      ): variable(std::move(variable)), exp(exp)
      {
      }
    };

    class Define: public Definition
    {
    public:
      std::string variable;
      ExpPtr exp;
      explicit Define(std::string var, ExpPtr exp): variable(std::move(var)), exp(exp) {};
    };

    class Definitions: public Definition
    {
    public:
      std::list<DefinitionPtr> defs;
      explicit Definitions(std::list<DefinitionPtr> defs): defs(std::move(defs)) {};
    };

    template<typename T = nullptr_t>
    Parser<Token, T> match(TokenType type)
    {
      return make_function(
        [=](IStream<Token> input) -> ParserResult<Token, T> {
          if (input.eof()) { return Error{ "eof", input.current() }; }
          if (type != input[0]->type) { return Error{ "match fail.", input.current() }; }
          if (!std::holds_alternative<T>(input[0]->value))
          {
            return Error{ "type error.", input.current() };
          }
          return std::make_pair(std::get<T>(input[0]->value), input + 1);
        }
      );
    }

    Parser<Token, nullptr_t> match(Keyword keyword);
    Parser<Token, std::string> variable();

    Parser<Token, DatumPtr> simpleDatum();
    Parser<Token, DatumPtr> listDatum();
    Parser<Token, DatumPtr> vectorDatum();
    Parser<Token, DatumPtr> quotation();

    Parser<Token, Literal> literal();
    Parser<Token, Call> call();
    Parser<Token, Lambda> lambda();
    Parser<Token, Conditional> conditional();
    Parser<Token, Assignment> assignment();

    Parser<Token, Formals> formals();
    Parser<Token, Formals> defFormals();
    Parser<Token, Body> body();

    Parser<Token, DefinitionPtr> definitions();
    Parser<Token, CODPtr> cod();
    Parser<Token, CODs> cods();
  } // namespace expression

  using Program = expression::CODs;
  using Datum = expression::Datum;
  using Exp = expression::Exp;
  using Definition = expression::Definition;

  Parser<Token, std::shared_ptr<Datum>> datum();
  Parser<Token, std::shared_ptr<Exp>> exp();
  Parser<Token, Program> program();
  Parser<Token, std::shared_ptr<Definition>> definition();
} // namespace r5rs

#endif
