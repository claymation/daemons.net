#include <iostream>

int main()
{
  double  d = 123456.789;
  int     i = d;
  short   s = d;
  char    c = d;
  bool    b = d;

  std::cout << std::fixed
            << "d: " << d << '\n'
            << "i: " << i << '\n'
            << "s: " << s << '\n'
            << "c: " << (int)c << '\n'
            << "b: " << b << '\n';
}
