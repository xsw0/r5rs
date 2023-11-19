#include "Lex.h"

#include <unordered_map>

using namespace r5rs;
using namespace r5rs::lex;

function_ptr<Char, char> r5rs::lex::char2Char() {
  static auto func = make_function([](char ch) { return Char{ ch }; });
  return func;
}

function_ptr<std::string, IStream<char>> r5rs::lex::charStream2String() {
  static auto func = make_function([](IStream<char> stream) -> std::string {
    return stream.foldl(make_function([](std::string string, char ch) {
      return std::move(string) + ch;
      }),
      std::string());
    });
  return func;
}

function_ptr<std::string, std::list<char>> r5rs::lex::charList2String() {
  static auto func = make_function([](std::list<char> stream) -> std::string {
    return std::string(stream.cbegin(), stream.cend());
    });
  return func;
}

ParserPtr<Char, char> r5rs::lex::any() {
  static auto parser = make_parser(
    make_function([](IStream<Char> input) -> ParserResult<Char, char> {
      auto&& ch = input[0];
      if (ch && ch->ch) {
        return std::make_pair(ch->ch, input + 1);
      }
      else {
        return Error{ "eof" };
      }
      }));
  return parser;
}

ParserPtr<Char, char> r5rs::lex::match(char c) {
  return make_parser(
    make_function([=](IStream<Char> input) -> ParserResult<Char, char> {
      if (input.eof()) {
        return Error{ "eof", input.current() };
      }
      if (c != input[0]->ch) {
        return Error{ "match fail.", input.current() };
      }
      return std::make_pair(input[0]->ch, input + 1);
      }));
}

ParserPtr<Char, char> r5rs::lex::match(function_ptr<bool, char> pred) {
  return make_parser(
    make_function([=](IStream<Char> input) -> ParserResult<Char, char> {
      if (input.eof()) {
        return Error{ "eof" };
      }
      if (!std::invoke(*pred, input[0]->ch)) {
        return Error{ "match fail." };
      }
      return std::make_pair(input[0]->ch, input + 1);
      }));
}

ParserPtr<Char, char> r5rs::lex::match_any_of(std::string set) {
  return match(make_function([set = std::move(set)](char c) -> bool {
    return set.find(c) != std::string::npos;
    }));
}

ParserPtr<Char, std::string> r5rs::lex::match(std::string str) {
  return make_parser(make_function([str = std::move(str)](IStream<Char> input)
    -> ParserResult<Char, std::string> {
      for (size_t i = 0; i != str.size(); ++i) {
        auto&& x = input[i];
        if (!x || x->ch != str[i]) {
          return Error{ "match fail." };
        }
      }
      return std::make_pair(str, input + str.size());
    }));
}

ParserPtr<Char, nullptr_t> r5rs::lex::delimiter() {
  static auto parser =
    (match('\0') || match_any_of(" \t\r\n()\";"))->as(nullptr);
  return parser;
}

ParserPtr<Char, std::string> r5rs::lex::comment() {
  static auto parser = select<1>(match(';'),
    match(make_function([](char ch) -> bool {
      return ch != '\0' && ch != '\n';
      }))
    ->many()
        ->map(charList2String()),
        match('\n'));
  return parser;
}

ParserPtr<Char, nullptr_t> r5rs::lex::intertoken_space() {
  static auto parser = (match_any_of(" \t\r\n")->as(std::string()) || comment())
    ->many()
    ->as(nullptr);
  return parser;
}

ParserPtr<Char, std::string> r5rs::lex::identifier() {
  static auto parser = peculiar_identifier() ||
    combine(make_function([](char lhs, std::list<char> rhs) {
    std::string id;
    id.push_back(lhs);
    id += std::invoke(*charList2String(), rhs);
    return id;
      }),
      initial(), subsequent()->many());
  return parser;
}

ParserPtr<Char, char> r5rs::lex::initial() {
  static auto parser = letter() || special_initial();
  return parser;
}

ParserPtr<Char, char> r5rs::lex::letter() {
  static auto parser = match(make_function([](char c) -> bool {
    return c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z';
    }));
  return parser;
}

ParserPtr<Char, char> r5rs::lex::special_initial() {
  static auto parser = match_any_of(R"(!$%&*/:<=>?^_~)");
  return parser;
}

ParserPtr<Char, char> r5rs::lex::subsequent() {
  static auto parser = initial() || digit() || special_subsequent();
  return parser;
}

ParserPtr<Char, char> r5rs::lex::digit() {
  static auto parser = match(
    make_function([](char ch) -> bool { return ch >= '0' && ch <= '9'; }));
  return parser;
}

ParserPtr<Char, char> r5rs::lex::special_subsequent() {
  static auto parser = match_any_of(R"(+-.@)");
  return parser;
}

ParserPtr<Char, std::string> r5rs::lex::peculiar_identifier() {
  static auto char2str = make_function([](char ch) { return std::string{ ch }; });
  static auto parser = match_any_of(R"(+-)")->map(char2str) || match("...");
  return parser;
}

ParserPtr<Char, bool> r5rs::lex::boolean() {
  static auto parser = match("#t")->as(true) || match("#f")->as(false);
  return parser;
}

ParserPtr<Char, char> r5rs::lex::character() {
  static auto parser =
    select<1>(match("#\\"), (character_name() || any()))->peek(delimiter());
  return parser;
}

ParserPtr<Char, char> r5rs::lex::character_name() {
  static auto parser = match("space")->as(' ') || match("newline")->as('\n');
  return parser;
}

ParserPtr<Char, std::string> r5rs::lex::string() {
  static auto parser =
    select<1>(match('"'), string_element()->many(), match('"'))
    ->map(charList2String());
  return parser;
}

ParserPtr<Char, char> r5rs::lex::string_element() {
  static auto normal = match(make_function(
    [](char c) -> bool { return c != '\\' && c != '"' && c != '\0'; }));
  static auto parser =
    match("\\\\")->as('\\') || match("\\\"")->as('"') || normal;
  return parser;
}

ParserPtr<Char, int64_t> r5rs::lex::number() {
  static auto digit =
    match(make_function([](char c) -> bool { return std::isdigit(c); }));

  static auto s2n =
    make_function([](std::string str) { return std::stoll(str); });

  static auto parser = digit->some()->map(charList2String())->map(s2n);

  return parser;
}

ParserPtr<Char, TokenType> r5rs::lex::single_symbol() {
  static const std::unordered_map<char, TokenType> single{
      {'(', TokenType::left_paren},     {')', TokenType::right_paren},
      {'\'', TokenType::quote_symbol},  {'`', TokenType::quasiquote_symbol},
      {',', TokenType::unquote_symbol}, {'.', TokenType::dot},
  };

  static auto find = make_function([=](char ch) {
    auto it = single.find(ch);
    if (it == single.end()) {
      return TokenType::err;
    }
    return it->second;
    });

  static auto cond =
    make_function([](TokenType type) { return type != TokenType::err; });

  static auto parser = any()->map(find)->filter(cond);

  return parser;
}

ParserPtr<Char, TokenType> r5rs::lex::two_char_symbol() {
  static const std::unordered_map<std::string, TokenType> two{
      {"#(", TokenType::vector_paren},
      {",@", TokenType::unquote_splicing_symbol},
  };

  static auto fail = make_parser(
    make_function([](IStream<Char> input) -> ParserResult<Char, TokenType> {
      return Error{ "faild." };
      }));

  static auto lexer = [=]() {
    auto lexer = fail;
    for (auto&& [symbol, type] : two) {
      lexer = lexer || match(symbol)->as(type);
    }
    return lexer;
    }();

    return lexer;
}

ParserPtr<Char, TokenType> r5rs::lex::symbol() {
  static auto parser = two_char_symbol() || single_symbol();
  return parser;
}

ParserPtr<Char, Token> r5rs::lex::token() {
  static auto parser = make_parser(
    make_function([&](IStream<Char> input) -> ParserResult<Char, Token> {
      if (!input[0]) {
        return Error{ "not find a char" };
      }
      auto line = input[0]->line;
      auto col = input[0]->col;

      auto make_token = [&](TokenType type,
        typename Token::value_t val = nullptr) {
          return Token{ type, std::move(val), line, col };
        };

      auto&& b = boolean()->map(make_function(
        [&](bool b) { return make_token(TokenType::boolean, b); }));

      auto&& ch = character()->map(make_function(
        [&](char ch) { return make_token(TokenType::character, ch); }));

      auto&& str = string()->map(make_function([&](std::string str) {
        return make_token(TokenType::string, str);
        }));

      auto&& num = number()->map(make_function(
        [&](int64_t n) { return make_token(TokenType::number, n); }));

      auto&& id = identifier()->map(make_function([&](std::string id) {
        return make_token(TokenType::identifier, id);
        }));

      auto&& sym = symbol()->map(
        make_function([&](TokenType type) { return make_token(type); }));

      return std::invoke(*(id || b || num || ch || str || sym), input);
      }));
  return parser;
}

ParserPtr<Char, Token> r5rs::lex::eof() {
  static auto parser = make_parser(
    make_function([](IStream<Char> input) -> ParserResult<Char, Token> {
      if (input[0] && input[0]->ch == '\xff') {
        return std::make_pair(
          Token{ TokenType::eof, nullptr, input[0]->line, input[0]->col },
          input + 1);
      }
      return Error{ "not found eof." };
      }));
  return parser;
}

IStream<Token> r5rs::tokens(IStream<Char> input) {
  static auto parser = select<1>(intertoken_space(), token() || eof());
  return IStream<Token>(make_function([input]() mutable -> Try<Token> {
    auto&& res = std::invoke(*parser, input);
    if (!res) {
      return Error{ "error." };
    }
    input = res->second;
    return res->first;
    }));
}
