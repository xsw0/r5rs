#ifndef R5RS_TEST_OUTPUT_H
#define R5RS_TEST_OUTPUT_H

template<typename T>
std::ostream & operator<<(std::ostream & os, std::vector<T> const & value)
{
  if (value.empty()) { os << "{}"; return os; }
  os << "{ " << value.front();
  for (size_t i = 1; i < value.size(); ++i)
  {
    os << ", " << value[i];
  }
  os << " }";
  return os;
}

#endif
