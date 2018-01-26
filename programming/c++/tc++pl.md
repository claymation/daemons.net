# The C++ Programming Language

[4th Edition](http://www.stroustrup.com/4th.html) (2013)  
Bjarne Stroustrup  

## Part I: Introduction

### 1. Notes to the Reader

C++ is not (strictly) an object-oriented programming language (§1.2.1). It
aims to support several popular programming styles and techniques:

 * Procedural programming
 * Data abstraction
 * Object-oriented programming
 * Generic programming

C++11 adds _rvalue_ references which make functional programming techniques
(passing around temporary objects instead of mutating objects with identity)
more efficient (§1.2.1).

The static type system in C++ is key to good performance (§1.2.2).

C and C++ are distinct languages: C++ is not a proper superset of C (§1.2.3).
The languages have evolved independently, borrowing features from each other
along the way. However:

> The definition of C++ has been revised to ensure that a construct that is
> both legal C and legal C++ has the same meaning in both languages.

C++ was designed to be efficient (§1.2.4). Except for dynamic memory
allocation (`new` and `delete`), run-time type information (`typeid` and
`dynamic_cast`), and exceptions (`try` and `throw`), the language requires no
run-time support.

I'm skipping the rest of Part I (a brief tour of C++) and jumping ahead to the
details.

## Part II: Basic Facilities

### 6. Types and Declarations

Standards-compliant code isn't necessarily good code (§6.1). It's not
necessarily portable, either, as many features are implementation-defined (due
to target platform hardware differences), and most real-world code uses
facilities defined by the system (e.g., POSIX), not the language.

Use compile-time static assertions (`static_assert()`) to verify assumptions
about the implementation. Avoid unspecified and undefined behavior. Use static
analysis tools.

C++ runs in hosted or freestanding environments (§6.1.1). Implementations
targeting freestanding environments allow language features (like exceptions)
to be disabled and include fewer library facilities (e.g., no STL).

C++ is strongly-typed (§6.2):

> Every name (identifier) in a C++ program has a type associated with it. 

The type system is based on the fundamental data storage units provided by
most computers (§6.2.1):

> The assumption is that a computer provides bytes for holding characters,
> words for holding and computing integer values, some entity most suitable for
> floating-point computation, and addresses for referring to those entities.

Of the fundamental types, use only `bool`, `char`, `int`, and `double`, unless
you have specialized space or performance requirements.

`char` is equivalent to either `signed char` or `unsigned char`
(implementation-defined), and at least 8 bits wide (§6.2.3).

The C++ standard supports ones-complement hardware. As such, the value -128
may not be representable in an 8-bit `signed char` on all platforms.

`char`, `signed char`, and `unsigned char` are distinct types; you
can't mix pointers between the types (§6.2.3.1).

C++ defines character literals (`'a'`) as `char`, unlike C which defines them
as `int` (§6.2.3.2).

Integer types come in three flavors (plain, `signed`, `unsigned`) and four
sizes (`short`, plain, `long`, and `long long`) (§6.2.4). The plain integer
types (`int`, `long`, etc) are always signed; the `signed` types are synonyms
for the plain types.  The `unsigned` types are best used to represent bit
arrays; any other use (to increase numerical range or enforce positive
values) is fraught due to implict type conversion.

The type of an integer literal depends on its form (decimal, octal, or hex),
value, and suffix (§6.2.4.2). There's a long list of rules, but the take home
message is to be explicit (using suffixes) whenever you want something besides
an `int`:

```c++
#include <iostream>

int main() {
  std::cout << (0xBEEF << 16) << std::endl;
}
```

prints `-1091633152` on a machine with 32-bit `int`s, while:

```c++
#include <iostream>

int main() {
  std::cout << (0xBEEFU << 16) << std::endl;
}
```

prints `3203334144`.

Floating point literals are of type `double` unless a type suffix is supplied
(§6.2.5.1).

Programmers can define suffix operators for user-defined types (§6.2.6),
opening the possiblity for literals like `"foo bar"s` and `123_km` -- who
knew?

`void` is a fundamental type useful only in building more complex types; there
are no `void` objects (§6.2.7).

Stroustrup wants you to write portable code (§6.2.8):

> People who claim they don’t care about portability usually do so because
> they use only a single system and feel they can afford the attitude that “the
> language is what my compiler implements.” This is a narrow and shortsighted
> view.

On the sizes of the fundamental types (§6.2.8):

> Sizes of C++ objects are expressed in terms of multiples of the size of a
> char, so by definition the size of a char is 1.
> ...
> [I]t is guaranteed that a char has at least 8 bits, a short at least 16 bits,
> and a long at least 32 bits.

The fundamental types can be intermixed in expressions; the compiler converts
values as necessary, but cannot always preserve the value, as when converting a
wider type to a narrower one, or a floating type to an integer:

```c++ conversions.cc
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
```

produces:

```txt !g++ conversions.cc && ./a.out
```

Caveat programmor: value-destroying conversions are perfectly legal and don't
produce any compiler warnings. Per Stroustrup (§6.2.8):

> Conversions that are not value-preserving are best avoided.

Two non-fundamental but useful types defined in `<cstddef>` are:

* `size_t`, capable of representing the size (in bytes) of any object
* `ptrdiff_t`, capable of representing the difference between any two pointers
(i.e., the number of elements in an array)

An object may occupy more memory than its type suggests; the extra memory is
padding due to the alignment requirements of the hardware (§6.2.9). The
`alignof()` operator and `alignas()` type specifier can be used to get or set,
respectively, the alignment of an object.

Declarations specify the type of an entity; definitions reserve memory for it
(§6.3):

> There must always be exactly one definition for each name in a C++ program.
> However, there can be many declarations.

Declarations can be quite complex, including optional prefix specifiers
(`static`, `virtual`), suffix function specifiers (`const`, `noexcept`), and
initializer or function body, but all declarations include a base type and a
declarator. The declarator defines the name and may include declarator
operators (`*`, `&`, `[]`, `()`) to define pointers, references, arrays, and
functions. The postfix declarator operators (`[]` and `()`) bind tighter than
the prefix operators (`*` and `&`), so parentheses are required to declare
types like _pointer-to-function_: `void (*fn)(void)`.

While multiple names can be declared in a single declaration, declaration
operators bind to a single name (6.3.2). Thus:

```c++
char* p, q;
```

defines `p` as _pointer-to-`char`_, and `q` as a plain `char`.

> Such declarations with multiple names and nontrivial declarators make a
> program harder to read and should be avoided.

Some names are reserved for the implementation (§6.3.3):

* non-local names beginning with underscore (`_start`)
* any name beginning with an underscore and uppercase letter (`_Bool`)
* any name containing a double underscore (`__func__`)

Stroustrup offers some advice on naming (one of [two hard things] in
computer science):

> Names from a large scope ought to have relatively long and reasonably
> obvious names. However, code is clearer if names used only in a small scope
> have short, conventional names such as x, i, and p. ... Use all capitals for
> macros and never for non-macros.

Four syntactic styles of object initialization are possible (§6.3.5):

* `T a1 {v};`: list initialization
* `T a2 = {v};`: list or aggregate initialization
* `T a2 = v;`: assignment-style initialization
* `T a3(v);`: function-style initialization

List initializtion is new in C++11. According to Stroustrup, this is the
preferred style as it can be used in every context and avoids narrowing
conversions (e.g., `long` to `int` and `double` to `float`). Personally,
I find it rather ugly.

You probably _don't_ want list initialization when using `auto` for type
deduction, though; the deduced type would be `std::initializer_list<T>`.

The empty list initializer (`{}`) initializes an object to the type's default
value.

Initialization is not required (§6.3.5.1), but:

> The only really good case for an uninitialized variable is a large input buffer.

The value of an uninitialized variable depends on the type of the variable and
its storage duration:

Global, namespace, local `static`, and `static` member variables are
initialized to `{}` of the appropriate type. For the built-in types, this
happens by virtue of the fact that the `.bss` segment is zeroed by the program
loader. For user-defined types, the compiler inserts calls to the type's
default constructor before the object is first referenced.

Variables of built-in type with automatic or dynamic storage duration are not
default initialized. Automatic and dynamic variables of user-defined type are
default initialized by calling the default constructor.

In declarations empty parentheses define a function, so to explicitly default
initialize an object use `{}` rather than `()` (§6.3.5.2):

```c++
T x{}; // declares, defines, and initializes a T
T y(); // declares a function returning T
```

[two hard things]: https://martinfowler.com/bliki/TwoHardThings.html
