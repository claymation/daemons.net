# The C++ Programming Language

Bjarne Stroustrup
4th Edition
2013
[Homepage](http://www.stroustrup.com/4th.html)

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
along the way. However (§44.3):

> The definition of C++ has been revised to ensure that a construct that is
> both legal C and legal C++ has the same meaning in both languages

C++ was designed to be efficient (§1.2.4). Except for dynamic memory
allocation (`new` and `delete`), run-time type information (`typeid` and
`dynamic_cast`), and exceptions (`try` and `throw`), the language requires no
run-time support.

## Part II: Basic Facilities

### 6. Types and Declarations

Standards-compliant code isn't necessarily good code (§6.1). It's not
necessarily portable, either, as many features are implementation-defined.
Use static assertions to verify assumptions about the implementation. Avoid
unspecified and undefined behavior.

C++ runs in hosted or freestanding environments (§6.1.1). Implementations
targeting freestanding environments allow language features (like exceptions)
to be disabled and include fewer library facilities (e.g., no STL).

The C++ type system is based on the fundamental data storage units provided by
most computers (§6.2.1) -- namely, 32- and 64-bit integer registers, floating
point registers, and byte-addressable memory.

Of the fundamental types, use only `bool`, `char`, `int`, and `double`, unless
you have specialized space or performance requirements.

`char`, `signed char`, and `unsigned char` are distinct types (§6.2.3); you
can't mix pointers between the types.

Naked `char` will be equivalent to either `signed char` or `unsigned char`
(implementation-defined).

The C++ standard supports ones-complement hardware (like, incidentally, the
Apollo Guidance Computer). As such, the value -128 may not be supported in an
8-bit `signed char` on all platforms.

C++ defines the character literal underlying type as `char`, unlike C99 which
defines it as `int`.
