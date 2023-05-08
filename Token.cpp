#include "Token.h"

#include <unordered_map>

using namespace r5rs;

std::string r5rs::TokenType2String(TokenType type)
{
  static const std::unordered_map<TokenType, std::string> names {
#define R5RS_TOKEN_TYPE_ACCESS(id) {TokenType::id, #id},
    R5RS_TOKEN_TYPE
#undef R5RS_TOKEN_TYPE_ACCESS
  };

  return names.at(type);
}
