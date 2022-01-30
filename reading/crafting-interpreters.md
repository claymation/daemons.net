# Crafting Interpreters

[1st Edition](http://craftinginterpreters.com) (2021)  
Robert Nystrom

## Part I: Welcome

### 1. Introduction

We're going to write two interpreters for the didactic scripting language, Lox:

* `jlox`, a tree-walk interpreter in Java
* `clox`, a bytecode virtual machine in C

Sounds like fun! Let's get started.

### 2. A Map of the Territory

Languages today are built much the way they were years ago.

Implementations follow paths as if climbing a mountain, starting from the
characters of the program text, ascending to higher levels of abstraction,
reaching a peak where the meaning of the program is understood, and finally
descending toward low-level representations suitable for the machine:

* *scanning*, aka lexing or lexical analysis, transforms a stream of
  characters to a stream of tokens
* *parsing* builds a parse tree / syntax tree representation of the token
  stream
* *static analysis* is more language-specific than the first two steps. Here,
  identifiers are bound according to scoping rules. Statically typed languages
  can do type checking here.

  Information learned during static analysis can be stored as attributes in the
  syntax tree, in a symbol table, or in an intermediate representation.

  These first three phases constitute the "front end".
* *intermediate representations* are neither language nor machine specific.
  IR styles include control flow graph, static single-assignment,
  continuation-passing style, and three-address code. GIMPLE and RTL are used in
  GCC; LLVM in clang.
* *optimization*, in which the user's program is swapped out for a faster (or
  smaller) program with the same semantics (meaning).

  Lua and CPython trade optimizing compilers for more efficient runtimes.

  These next two phases constitute the "middle end" (how can you have a middle
  _end_?)
* *code generation*, the "back end" of the compiler, emits machine code or
  machine-agnostic bytecode.
* *virtual machines* interpret/execute bytecode.
* the *runtime* provides run-time support for language features like garbage
  collection and querying an object's type.

*Single-pass compilers* don't build a full syntax tree; they generate code as
soon as they have seen enough text to know what to generate. This was important
for early compilers that ran on machines that didn't have enough memory to store
even the entire program text. The Pascal and C languages were designed around
this constraint, which is why, for instance, C requires forward declarations.

Syntax-directed translation is one technique for building single-pass compilers.

*Tree-walk interpreters* construct a syntax tree and then evaluate each node in
turn. Matz's Ruby Interpreter worked like that.

*Transpilers* translate the source text to another language. Originally,
transcompilers translated assembly languages; now, it's more common to transpile
high-level languages to JavaScript.

With *just-in-time compilation*, source text or machine independent bytecode is
translated to machine code on-demand, as the program is executed.

*Compilers* translate programs from one form (source code) to another (machine
code, byte code, another high-level language), but don't execute it.

*Interpreters* execute programs. Some interpreters compile the source text to
bytecode and then execute that.

In other news, apparently peanuts are fruit.


### 3. The Lox Language

Lox is a little language with a C-like syntax. It resembles high-level scripting
languages like JavaScript, Scheme, and Lua.

Lox is *dynamically typed*, because that's easier to implement than a static
type system:

> A static type system is a ton of work to learn and implement.

Lox uses *garbage collection* (as opposed to *reference counting*) to
automatically manage memory.

Lox's built-in data types are:

* **Booleans** (`true`, `false`)
* **numbers** are double-precision floating-point (`1234`, `1.234`)
* **strings** are enclosed in double quotes (`"I am a string"`)
* **nil**, the un-value

We have the usual _infix binary_ operators (`+`, `-`, `*`, `/`), and a unary `-`
to negate a value. `+` concatenates strings.

We have the usual comparison operators (`>`, `>=`, `<`, `<=`), which work on
numbers, and equality operators (`==`, `!=`), which work on any type.

> I’m generally against implicit conversions.

We have a hybrid of C and Python logical operators: `!`, `and`, and `or` do what
you'd expect. Because they short circuit, `and` and `or` are like control flow
structures.

> Where an expression’s main job is to produce a value, a statement’s job is to
> produce an effect.

We have a `print` statement for didactic convenience, and C-style `{}` blocks.

Variables are declared with `var` statements:

```
var foo = "bar";
```

For control flow, we have `if`, `while`, and `for`.

Functions are defined with `fun`, return values with `return, and are called
with parens:

```
fun add(a, b) {
    return a + b;
}

add(2, 3)
```

A function that doesn't `return` anything implicitly returns `nil`.

Functions are first class — you can pass functions to functions, store them in
variables, etc. You can also define functions within functions. And we have
closures.

Lox has class-based objects.

(I finally understand JavaScript's `prototype`, thanks to [What about JavaScript][]
in his other book, _Game Programming Patterns_.)

[what about javascript]: http://gameprogrammingpatterns.com/prototype.html#what-about-javascript

Classes are first-class. Call the class like a function to create an instance;
define `init()` to initialize new instances.

```
class Foo
{
  init()
  {
    this.bar = 42;
  }
}

foo = Foo();

print foo.bar;
```

We have single inheritance, via the `<` operator, and `super` to call base class
methods.

Lox isn't everything-is-an-object OOP like Ruby. Built-in types are not
instances of classes and don't have methods.

The Lox standard library is, well, `clock()`.

## Part II: A Tree-Walk Interpreter

### 4. Scanning


