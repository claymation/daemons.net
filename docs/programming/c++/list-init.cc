#include <string>
#include <vector>

std::string s{"foo"};           // direct list initialization
std::string t = {"foo"};        // copy list initialization

std::vector<int> v{1, 2, 3};    // direct list initialization
std::vector<int> w = {1, 2, 3}; // copy list initialization

class T {
    T(const std::string& s)
        : s{s}                  // direct list initialization
    {}

    std::string s;
    std::string t{"foo"};       // direct list initialization
    std::string u = {"foo"};    // copy list initialization
};

T* tp = new T{"foo"};           // direct list initialization

T f(T t)
{
    return {"foo"};             // copy list initialization
}

void g()
{
    f({"foo"});                 // copy list initialization
}
