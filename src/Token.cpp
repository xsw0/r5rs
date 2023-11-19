#include "Token.h"

#include <array>

using namespace r5rs;

std::string r5rs::to_string(TokenType type) {
  static const std::unordered_map<TokenType, std::string> names{
#define R5RS_TOKEN_TYPE_ACCESS(id) {TokenType::id, #id},
      R5RS_TOKEN_TYPE
#undef R5RS_TOKEN_TYPE_ACCESS
  };

  return names.at(type);
}

std::string r5rs::to_string(Keyword keyword) {
  static const std::unordered_map<Keyword, std::string> names{
#define R5RS_KEYWORD_ACCESS(id, name) {Keyword::id, name},
      R5RS_KEYWORD
#undef R5RS_KEYWORD_ACCESS
  };

  return names.at(keyword);
}

const std::unordered_map<std::string, TokenType> &r5rs::typeTokens() {
  static const std::unordered_map<std::string, TokenType> table{
#define R5RS_TOKEN_TYPE_ACCESS(id) {#id, TokenType::id},
      R5RS_TOKEN_TYPE
#undef R5RS_TOKEN_TYPE_ACCESS
  };

  return table;
}

const std::unordered_map<std::string, Keyword> &r5rs::keywords() {
  static const std::unordered_map<std::string, Keyword> table{
#define R5RS_KEYWORD_ACCESS(id, name) {name, Keyword::id},
      R5RS_KEYWORD
#undef R5RS_KEYWORD_ACCESS
  };

  return table;
}
