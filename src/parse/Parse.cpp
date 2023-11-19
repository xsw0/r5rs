#include "Parse.h"

#include <unordered_map>

using namespace r5rs;

IStream<Char>& r5rs::cinIStream() {
  static bool hasEmitEof = false;
  static size_t line = 1;
  static size_t col = 0;
  static IStream<Char> is(make_function([&]() -> Try<Char> {
    if (std::cin.eof()) {
      return Error{ "not find char" };
    }
    char ch = std::cin.get();
    if (ch == '\xff') {
      return Char{ '\xff', line, col };
    }
    Char Ch = Char{ ch, line, col };
    ++col;
    if (ch == '\n') {
      ++line, col = 0;
    }
    return Ch;
    }));
  return is;
}
