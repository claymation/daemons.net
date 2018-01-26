#include <string>

void f()
{
    int i = 42; // initialized each time f() is called

    while (i--) {
        std::string s{"foo"}; // initialized each iteration
    }
}
