#include <string>

std::string s;  // default-initialized before main() or first use

void f()
{
    static std::string s;   // default-initialized when f() is first called
}
