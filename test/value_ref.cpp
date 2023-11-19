#include <iostream>
#include <variant>

#include "value_ref.h"

using namespace r5rs;

int main()
{
  std::variant<value_ref<int>, const value_ref<const double>> v = value_ref<const double>(6.5);
  std::cout << std::get<const value_ref<const double>>(v) << std::endl;

  value_ref<const int> a = 5;
  std::cout << a << std::endl;

  return 0;
}
