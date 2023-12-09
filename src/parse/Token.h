#ifndef R5RS_TOKEN_H
#define R5RS_TOKEN_H

#include <string>
#include <unordered_map>
#include <variant>

#define R5RS_TOKEN_TYPE                                                        \
  R5RS_TOKEN_TYPE_ACCESS(err)                                                  \
                                                                               \
  R5RS_TOKEN_TYPE_ACCESS(identifier)                                           \
  R5RS_TOKEN_TYPE_ACCESS(left_paren)                                           \
  R5RS_TOKEN_TYPE_ACCESS(right_paren)                                          \
  R5RS_TOKEN_TYPE_ACCESS(vector_paren)            /* #( */                     \
  R5RS_TOKEN_TYPE_ACCESS(quote_symbol)            /* '  */                     \
  R5RS_TOKEN_TYPE_ACCESS(quasiquote_symbol)       /* `  */                     \
  R5RS_TOKEN_TYPE_ACCESS(unquote_symbol)          /* ,  */                     \
  R5RS_TOKEN_TYPE_ACCESS(unquote_splicing_symbol) /* ,@ */                     \
  R5RS_TOKEN_TYPE_ACCESS(dot)                     /* .  */                     \
                                                                               \
  R5RS_TOKEN_TYPE_ACCESS(boolean)                                              \
  R5RS_TOKEN_TYPE_ACCESS(character)                                            \
  R5RS_TOKEN_TYPE_ACCESS(string)                                               \
  R5RS_TOKEN_TYPE_ACCESS(number)                                               \
                                                                               \
  R5RS_TOKEN_TYPE_ACCESS(eof)

#define R5RS_KEYWORD                                                           \
  /* syntactic keyword */                                                      \
  R5RS_KEYWORD_ACCESS(Else, "else")                                            \
  R5RS_KEYWORD_ACCESS(evaluates_to, "=>")                                      \
  R5RS_KEYWORD_ACCESS(define, "define")                                        \
                                                                               \
  R5RS_KEYWORD_ACCESS(unquote, "unquote")                                      \
  R5RS_KEYWORD_ACCESS(unquote_splicing, "unquote-plicing")                     \
                                                                               \
  /* expression keyword */                                                     \
  R5RS_KEYWORD_ACCESS(quote, "quote")                                          \
  R5RS_KEYWORD_ACCESS(lambda, "lambda")                                        \
  R5RS_KEYWORD_ACCESS(If, "if")                                                \
                                                                               \
  R5RS_KEYWORD_ACCESS(set_, "set!")                                            \
  R5RS_KEYWORD_ACCESS(begin, "begin")                                          \
  R5RS_KEYWORD_ACCESS(cond, "cond")                                            \
  R5RS_KEYWORD_ACCESS(And, "and")                                              \
  R5RS_KEYWORD_ACCESS(Or, "or")                                                \
  R5RS_KEYWORD_ACCESS(Case, "case")                                            \
                                                                               \
  R5RS_KEYWORD_ACCESS(let, "let")                                              \
  R5RS_KEYWORD_ACCESS(let_, "let*")                                            \
  R5RS_KEYWORD_ACCESS(letrec, "letrec")                                        \
  R5RS_KEYWORD_ACCESS(Do, "do")                                                \
  R5RS_KEYWORD_ACCESS(delay, "delay")                                          \
                                                                               \
  R5RS_KEYWORD_ACCESS(quasiquote, "quasiquote")

namespace r5rs {
  enum class TokenType {
#define R5RS_TOKEN_TYPE_ACCESS(id) id,
    R5RS_TOKEN_TYPE
#undef R5RS_TOKEN_TYPE_ACCESS
  };

  enum class Keyword {
#define R5RS_KEYWORD_ACCESS(var, val) var,
    R5RS_KEYWORD
#undef R5RS_KEYWORD_ACCESS
  };

  std::string to_string(TokenType type);
  std::string to_string(Keyword keyword);

  const std::unordered_map<std::string, TokenType>& typeTokens();
  const std::unordered_map<std::string, Keyword>& keywords();

  class Token final {
  public:
    using value_t = std::variant<nullptr_t, bool, char, int64_t, std::string>;
    TokenType type = TokenType::err;
    value_t value = nullptr;
    size_t row = 0;
    size_t col = 0;
  };
} // namespace r5rs

#endif
