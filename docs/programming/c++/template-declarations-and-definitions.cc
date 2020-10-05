/*
 * Declarations.
 */
template<typename T>
void func();

template<typename T>
class Class;

template<typename T>
extern T var;

/*
 * Definitions.
 */
template<typename T>
void func()
{
    // ...
}

template<typename T>
class Class
{
    // ...
};

template<typename T>
T var;

template<typename T>
using Alias = Class<T>;
