#ifndef _R5RS_EXPRESSIONS_H_
#define _R5RS_EXPRESSIONS_H_

#include <string>
#include <vector>
#include <list>
#include <optional>

#include "Type.h"
#include "Token.h"
#include "Parse.h"

namespace r5rs
{
  namespace expression
  {
    struct COD;
    struct Datum;
    struct Definition;
    struct COD;
    struct Datum;
    struct Definition;
    struct Exp;

    class CODs;
    class SimpleDatum;
    class ListDatum;
    class VectorDatum;
    class Variable;
    class Literal;
    class Call;
    class Formals;
    class Body;
    class Lambda;
    class Conditional;
    class Assignment;
    class Define;
    class Definitions;

    // command or definition
    struct COD
    {
      using cod_t = std::variant<
        CODs *,
        Exp *,
        Definition *
      >;
      virtual ~COD() = default;
      virtual cod_t cod_type() = 0;
    };

    using CODPtr = std::shared_ptr<COD>;

    struct Datum
    {
      using datum_t = std::variant<
        SimpleDatum *,
        ListDatum *,
        VectorDatum *
      >;

      virtual ~Datum() = default;
      virtual datum_t datum_type() = 0;
    };

    using DatumPtr = std::shared_ptr<Datum>;

    struct Definition: COD
    {
      using def_t = std::variant<
        Define *,
        Definitions *
      >;

      ~Definition() override = default;
      cod_t cod_type() final { return this; }
      virtual def_t def_type() = 0;
    };

    using DefinitionPtr = std::shared_ptr<Definition>;

    struct Exp: COD
    {
      using exp_t = std::variant<
        Variable *,
        Literal *,
        Call *,
        Lambda *,
        Conditional *,
        Assignment *
      >;

      ~Exp() override = default;
      cod_t cod_type() final { return this; }
      virtual exp_t exp_type() = 0;
    };

    using ExpPtr = std::shared_ptr<Exp>;

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

    class CODs: public COD
    {
    public:
      std::list<std::shared_ptr<COD>> cods;
      explicit CODs(std::list<CODPtr> cods): cods(std::move(cods)) {}
      cod_t cod_type() override { return this; }
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
      datum_t datum_type() override { return this; }
    };

    class ListDatum: public Datum
    {
    public:
      std::list<DatumPtr> list;
      explicit ListDatum(std::list<DatumPtr> list): list(std::move(list)) {}
      datum_t datum_type() override { return this; }
    };

    class VectorDatum: public Datum
    {
    public:
      std::list<DatumPtr> list;
      explicit VectorDatum(std::list<DatumPtr> list): list(std::move(list)) {}
      datum_t datum_type() override { return this; }
    };

    class Variable: public Exp
    {
    public:
      std::string id;
      explicit Variable(std::string id): id(std::move(id)) {}
      exp_t exp_type() override { return this; }
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

      exp_t exp_type() override { return this; }
      value_t value;
      explicit Literal(value_t value): value(std::move(value)) {}
    };

    class Call: public Exp
    {
    public:
      ExpPtr op;
      std::list<ExpPtr> operands;

      exp_t exp_type() override { return this; }
      explicit Call(ExpPtr op, std::list<ExpPtr> ops): op(op), operands(std::move(ops)) {}
    };

    class Lambda: public Exp
    {
    public:
      exp_t exp_type() override { return this; }

      explicit Lambda(
      std::shared_ptr<Formals> formals,
      std::shared_ptr<Body> body
      ): formals(formals), body(body)
      {
      }

      std::shared_ptr<Formals> formals;
      std::shared_ptr<Body> body;
    };

    class Conditional: public Exp
    {
    public:
      exp_t exp_type() override { return this; }

      explicit Conditional(
        ExpPtr test,
        ExpPtr consequent,
        ExpPtr alternate
      ): test(test), consequent(consequent), alternate(alternate)
      {
      }

      ExpPtr test;
      ExpPtr consequent;
      ExpPtr alternate;
    };

    class Assignment: public Exp
    {
    public:
      exp_t exp_type() override { return this; }

      explicit Assignment(
        std::string variable,
        ExpPtr exp
      ): variable(std::move(variable)), exp(exp)
      {
      }

      std::string variable;
      ExpPtr exp;
    };

    class Define: public Definition
    {
    public:
      def_t def_type() override { return this; }

      explicit Define(std::string var, ExpPtr exp): variable(std::move(var)), exp(exp) {};

      std::string variable;
      ExpPtr exp;
    };

    class Definitions: public Definition
    {
    public:
      def_t def_type() override { return this; }

      explicit Definitions(std::list<DefinitionPtr> defs): defs(std::move(defs)) {};

      std::list<DefinitionPtr> defs;
    };

    template<typename T = nullptr_t>
    ParserPtr<Token, T> match(TokenType type)
    {
      return make_parser(
        make_function(
          [=](IStream<Token> input) -> ParserResult<Token, T> {
            if (input.eof()) { return Error{ "eof", input.current() }; }
            if (type != input[0]->type) { return Error{ "match fail.", input.current() }; }
            if (!std::holds_alternative<T>(input[0]->value))
            {
              return Error{ "type error.", input.current() };
            }
            return std::make_pair(std::get<T>(input[0]->value), input + 1);
          }
        )
      );
    }

    ParserPtr<Token, nullptr_t> match(Keyword keyword);
    ParserPtr<Token, std::string> variable();

    ParserPtr<Token, DatumPtr> simpleDatum();
    ParserPtr<Token, DatumPtr> listDatum();
    ParserPtr<Token, DatumPtr> vectorDatum();
    ParserPtr<Token, DatumPtr> quotation();

    ParserPtr<Token, Literal> literal();
    ParserPtr<Token, Call> call();
    ParserPtr<Token, Lambda> lambda();
    ParserPtr<Token, Conditional> conditional();
    ParserPtr<Token, Assignment> assignment();

    ParserPtr<Token, Formals> formals();
    ParserPtr<Token, Formals> defFormals();
    ParserPtr<Token, Body> body();

    ParserPtr<Token, DefinitionPtr> definitions();
    ParserPtr<Token, CODPtr> cod();
    ParserPtr<Token, CODs> cods();

    void init();
  } // namespace expression

  using Program = expression::CODs;
  using Datum = expression::Datum;
  using Exp = expression::Exp;
  using Definition = expression::Definition;

  ParserPtr<Token, std::shared_ptr<Datum>> datum();
  ParserPtr<Token, std::shared_ptr<Exp>> exp();
  ParserPtr<Token, Program> program();
  ParserPtr<Token, std::shared_ptr<Definition>> definition();

  IStream<expression::CODPtr> ast(IStream<Token> input);
} // namespace r5rs

#endif
