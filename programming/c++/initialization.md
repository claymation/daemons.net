# C++ Object Initialization

Initialization provides an object's initial value. The object's type,
scope, storage duration, and context determine whether and how it is
initialized.


## Static objects

Objects with static storage duration (i.e., variables declared at namespace
scope, `static` class members, and `static` local variables) and no explicit
initializer are guaranteed to be [zero-initialized][zero initialization]:

```c++ zero-init.cc
bool b;     // false
char c;     // '\0'
int i;      // 0
double d;   // 0.0
void *p;    // nullptr

void f()
{
    static int i;   // 0
}

class T {
    static int i;   // declaration; definition required below
};

int T::i;   // 0
```

Exactly how zero initialization occurs is implementation defined, but
conventionally static objects are placed in the `.bss` section of the
executable image, where they occupy no space, and are zero-filled at program
start-up (by the operating system, program loader, or language run-time).

Static objects of class type with a default constructor, that are not
explicitly initialized, are [default-initialized][default initialization] at
run-time, before first use. When, exactly, this happens depends on the
object's scope, and, in some cases, on the compiler. `static` locals are
initialized the first time control passes through their declaration; globals
and `static` members are ostensibly initialized at program start-up, before
`main()`, but compilers can defer initialization until first use:

```c++ static-default-init.cc
#include <string>

std::string s;  // default-initialized before main() or first use

void f()
{
    static std::string s;   // default-initialized when f() is first called
}
```

The order of initialization for static objects defined within a translation
unit is defined (they're initialized in the order they're declared), but the
order of initialization of objects defined in different translation units is
undefined. Further, the objects defined in a particular translation unit may
never be initialized if none of them is ever used by the program.

Initial values for static objects can be provided with an initializer:

```c++ constant-init.cc
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
```

Compilers may be able to use [constant initialization][] to construct the
object at compile-time and emit its representation in the initialized `.data`
section of the program image. That's the case for objects of built-in type and
class types with `constexpr` constructors and constant constructor arguments.

For objects of class type *without* `constexpr` constructors, the class
constructor is called at run-time, before first use, as described above for
default initialization. This dynamic initialization stage occurs after the
static zero initialization stage, which zero-fills the object's store at
program start-up, before its constructor is invoked.


## Automatic objects

Objects with automatic storage duration (i.e., local variables) are
initialized each time control reaches their declaration:

```c++ auto-init.cc
#include <string>

void f()
{
    int i = 42; // initialized each time f() is called

    while (i--) {
        std::string s{"foo"}; // initialized each iteration
    }
}
```

Without an explicit initializer, automatic objects are default-initialized.
Objects of class type with a default constructor are default-initialized by
calling the default constructor. Objects of class type without a default
constructor and objects of built-in type **are left uninitialized** by
default initialization.  Their value is not defined; compilers should emit
warnings if such variables are used uninitialized. According to Stroustrup
([TC++PL](tc++pl.html)):

> The only really good case for an uninitialized variable is a large input
> buffer. 


## Dynamic objects

Objects with dynamic storage duration (i.e., allocated with `new`) are
initialized by `operator new` after it allocates memory for the object:

```c++
int *pi = new int;      // uninitialized
int *pj = new int{42};  // direct-initialized

T *pt = new T;          // default-initialized
T *pu = new T{42};      // list-initialized
```

As with automatic objects, dynamic objects are default-initialized if
no intializer is provided. Also like automatic objects, objects of built-in
type **are left uninitialized** by default initialization; their values are
not defined.


## Non-static members

Class constructors are responsible for initializing their non-`static`
members using an initializer list or in-class initializer:

```c++
class T {
    // Default constructor, default-initializes all members
    T() = default;

    // Constructor with initializer list
    explicit T(int x) 
        : i{x}  // direct-initialized
        , j{}   // value-initialized
    {}

    int i = 42; // in-class initializer; overridden by initializer list 
    int j;      // uninitialized by default constructor
    int k;      // uninitialized
}
```

In-class initializers provide default values that can be overridden by
constructor initializer lists; if both are provided, the initializer list
takes precedence.

The generated default constructor default-initializes all non-`static`
members. As before, members of built-in type **are left uninitialized** by
default initialization, and therefore will have undefined values.


## Initializers

There are four syntactic styles of initialization:

 * `T object{ arg1, arg2, ... };`
 * `T object = { arg1, arg2, ... };`
 * `T object(arg1, arg2, ... );`
 * `T object = arg;`

What they mean depends on their context and the type T.

### List Initialization

```c++ list-init.cc
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
```

These are all forms of [list initialization][]. The common element is the
braced-init-list, `{...}`. List initialization is new in C++11, and, according
to Stroustrup, is the preferred method because it's general (available in all
contexts) and safe (immune to narrowing conversions).

The two forms of list initialization, direct and copy, differ in whether they
can call `explicit` constructors: direct list initialization can,
copy list initialization cannot.

Generally, a list initialization of the form `T t{arg1, arg2, ...}` will first
search for a constructor taking a `std::initializer_list<>` as the only
non-default argument, and failing to find a match will then search for a
constructor taking the number and type of arguments as specified in the
braced-init-list, allowing only non-narrowing conversions. Thus, after:

```c++ vector-init.cc
#include <vector>

std::vector<int> v{100, 0};
std::vector<int> w(100, 0);
```

`v` contains exactly two elements (`100` and `0`), while `w` contains 100
elements, all `0`, because `std::vector<T>` has both kinds of constructor:

```c++
template<typename T>
class vector {
    vector(std::initializer_list<T> init);
    explicit vector(size_type count, const T& value = T());
}
```

List initialization reduces to value, aggregate, direct, or
copy initialization in certain cases:

  * an empty braced-init-list denotes [value initialization][], which produces
    the default value for the type
  * a braced-init-list applied to an aggregate type denotes
    [aggregate initialization][], which initializes individual members of the
    aggregate
  * a braced-init-list applied to a specialization of the
    `std::initializer_list` template denotes direct or copy initialization of
    that `std::initializer_list` object

### Value Initialization

```c++ value-init.cc
#include <string>

bool b{};           // false
char c{};           // '\0'
int i{};            // 0
double d{};         // 0.0
std::string s{};    // ""
```

Value initialization is, in effect, zero initialization followed by
default initialization. Because default initialization leaves objects of
built-in type uninitialized, value initialization of built-in types is
equivalent to zero initialization:

```c++ built-in-value-init.cc
void f()
{
    int i{};    // 0
    int j;      // undefined
}
```

### Aggregate Initialization

```c++
struct T {
    int x;
    int y;
    int z;
};

T t{1, 2, 3};       // { 1, 2, 3 }
T u = {1, 2, 3};    // { 1, 2, 3 }
T v{1};             // { 1, 0, 0 }

char a[] = {'f', 'o', 'o', '\0'};
char b[] = "foo";   // equivalent
char c[10] = {};    // zero-filled
```

Aggregates are arrays and simple class types (typically `struct`s and
`union`s) with no bases, no private members, and no user-provided
constructors. The individual elements/members of the aggregate are
initialized, in order, from the elements in the braced-init-list. If not
specified, the array size is determined by the length of the initializer. If
the initializer contains fewer elements than the type requires, the remaining
elements are zero-initialized. As a special case, character arrays can be
aggregate-initialized from string literals.

Proving that C++ is not a superset of C, C++ does not allow designated
initializers (until C++20, at least), which C has had since C99:

```c
struct T {
    int x;
    int y;
    int z;
};

T t = {
    .x = 1, // oops! can't do this in C++
    .y = 2,
    .z = 3,
};
```

### Direct Initialization

```c++
#include <string>
#include <vector>

int i(42);
int j{42};  // special case for braced-init-list with built-in types

std::string s("foo");
std::string t(10, 'a');     // "aaaaaaaaaa"

std::vector<int> v(10);     // 10 0s
std::vector<int> v(10, 5);  // 10 42s
```

Direct intialization searches for a compatible constructor, considering
`explicit` and non-`explicit` constructors, and performing user-defined or
standard conversions, as necessary.

Direct initialization is susceptible to the [most vexing parse][]:

```c++
T f();  // function declaration, not (empty) direct initialization
```

### Copy Initialization

```c++
#include <string>

int i = 42;

std::string s = "foo";

std::string f(std::string s)    // s is copy-initialized from argument
{
    return s + "foo";
}

void g()
{
    std::string s = "foo";
    std::string t = f(s);   // t is copy-initialized from temporary
}
```

Copy initialization is less permissive than direct initialization: it only
searches non-`explicit` constructors, and requires that the initializer be
convertible to the object type.

[zero initialization]: http://en.cppreference.com/w/cpp/language/zero_initialization
[default initialization]: http://en.cppreference.com/w/cpp/language/default_initialization
[constant initialization]: http://en.cppreference.com/w/cpp/language/constant_initialization
[list initialization]: http://en.cppreference.com/w/cpp/language/list_initialization
[value initialization]: http://en.cppreference.com/w/cpp/language/value_initialization
[aggregate initialization]: http://en.cppreference.com/w/cpp/language/aggregate_initialization
[most vexing parse]: http://en.wikipedia.com/wiki/Most_vexing_parse
