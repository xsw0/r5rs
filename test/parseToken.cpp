#include <iostream>


#include "GC.h"
#include "Lex.h"
#include "Parse.h"
#include "String.h"
#include "Token.h"
#include "Type.h"
#include "color.h"

using namespace r5rs;

int main(int argc, char* argv[]) {
  auto toks = tokens(cinIStream());

  for (size_t i = 0; toks[i]; ++i) {
    auto&& tok = *toks[i];
    std::cout << C_BOLD << r5rs::to_string(tok.type) << C_BLACK << "["
      << tok.row << ":" << tok.col << "]:" << C_GREEN
      << std::visit(String(), tok.value) << C_BLACK << std::endl;
  }

  return 0;
}
