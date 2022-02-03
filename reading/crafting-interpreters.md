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

A *lexeme* is a minimal sequence of characters that represents something.

Here are some lexemes: `var` &nbsp; `foo` &nbsp; `=` &nbsp; `42` &nbsp; `;`.

The language's *lexical grammar* defines how sequences of characters map to
lexemes. Lox is a *regular language* — it could be scanned with regular
expressions.

A *token* is a lexeme adorned with some additional information learned during
scanning:

* token type, e.g., *var*, *identifier*, *equals*
* literal value, for identifiers, strings, and numbers
* location, line and column at which the lexeme appears in the source text

The [`Scanner`][] class amounts to a loop over a `switch` statement. To handle
two-character tokens like `!=` and comments (`//`), the scanner employs
one-character lookahead. To handle decimal number literals (e.g., `3.14159`),
the scanner looks ahead two characters.

[`Scanner`]: crafting-interpreters/java/com/craftinginterpreters/lox/Scanner.java

By the end of the chapter, we have a scanner that translates the input character
stream into a list of tokens, as seen here in the REPL:

```
clay@satellite crafting-interpreters % ./jlox
> var foo = 42; // some comment
VAR var null
IDENTIFIER foo null
EQUAL = null
NUMBER 42 42.0
SEMICOLON ; null
EOF  null

> fun foo(bar) { print bar.blee; }
FUN fun null
IDENTIFIER foo null
LEFT_PAREN ( null
IDENTIFIER bar null
RIGHT_PAREN ) null
LEFT_BRACE { null
PRINT print null
IDENTIFIER bar null
DOT . null
IDENTIFIER blee null
SEMICOLON ; null
RIGHT_BRACE } null
EOF  null
```

### 5. Representing Code

Expressions can be represented as trees, with values at the leaves and operators
at interior nodes. The expression is evaluated via a post-order traversal of the
tree.

And now, a bit about formal grammars:

Our scanner used a *regular grammar* to translate sequences of characters to
tokens. Our parser will use a *context-free grammar* to translate sequences of
tokens to expressions. Context-free grammars are more expressive: they can
represent arbitrarily nested expressions.

Grammars have rules (called productions) that can be used to produce (derive)
strings (called derivations). Productions reference two flavors of *symbol*:

* *terminal*, a letter from the grammar's alphabet (i.e., a token)
* *nonterminal*, a reference to another production

Each production has a *head* and a *body*: an input matching the head may be
replaced by the body. So, for the production:

```
protein → cooked "eggs" ;
```

any occurence of the head (`protein`) in the input may be replaced by the body
(`cooked "eggs"`). Using the author's notation, `protein` and `cooked` are
nonterminals (unquoted) and `eggs` is a terminal (quoted).

In context-free grammars, the head of each production must be a single
nonterminal.

We're going to start by writing _some_ of the syntactic grammar, just enough to
allow us to parse a handful of expressions. We'll add more grammar rules in
later chapters.

And now, back to trees:

Using our grammar, which defines valid sequences of tokens, we're going to
write a parser that converts the stream of tokens emitted by the scanner into a
*abstract syntax tree* representing the code.

Since we're using the object-oriented programming paradigm, we'll have a base
`Expr` class for the nodes of our tree, with subclasses for each type of
expression. Because Java requires a lot of boilerplate code, we'll write a
program to generate all the classes from a simple textual description.

The *expression problem* is an object-oriented design problem that notes that
while it's easy to add new classes to a hierarchy, it's hard to add new methods
(classes of behavior) to all existing classes in the hierarchy.

The *visitor pattern* (from GoF) helps here. We define a `Visitor` interface,
which defines methods for each of the classes in the hierarchy that we want to
"visit". We also define an abstract `accept(Visitor)` method in the hierarchy
base class and implement it in each of the derived classes. The implementation
of `accept(Visitor)` is straightforward — it just calls the appropriate method
on the passed-in Visitor. Then, to add a new class of behaviors, we just
implement the Visitor interface in a concrete class, and pass an instance of
that to an instance of any class in our hierarchy.

To get a feel for the pattern, we implement an [`AstPrinter`][] class and use
that to print some simple expressions. (And, for fun, [`RpnPrinter`][]).

[`AstPrinter`]: crafting-interpreters/java/com/craftinginterpreters/lox/AstPrinter.java
[`RpnPrinter`]: crafting-interpreters/java/com/craftinginterpreters/lox/RpnPrinter.java
