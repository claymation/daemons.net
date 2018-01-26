#include <string>

// constant initialization
int i = 42;

// zero initialization followed by list initialization
std::string s{"foo"};

class T {
    constexpr T(int x) : x{x} {}

    int x;
};

// candidate for constant initialization
T w = 42;
