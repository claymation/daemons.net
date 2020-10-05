# C++ Templates

Templates are recipes teaching the compiler how to make functions, classes,
variables, and type aliases from ingredients (types and values) provided by
the user at compile time.

## Declaring and defining templates

Template functions, classes, variables, and type aliases are declared and
defined just like their non-template counterparts, except preceded with
`template<Params...>`, where `Params...` is a list of one or more [_template
parameters_](#Template-parameters).

```c++ templates.cc
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
using alias = T;
```

Templates can be defined in namespace or class scope, but not within a function.

## Template class members

Template classes can contain static and non-static (i.e., instance) data and
function members, just as with ordinary (non-template) classes. They can also
contain template members (function, class, variable, and type alias).

To refer to template class members from outside the template class definition,
the member name is preceded by `template<Params...>`, just as in the template
class declaration, _and_ the template class name is followed by an additional
`<...>` containing the template parameter names:

```c++ template-class-members.cc
template<typename T>
class Bar
{
public:
    // Ordinary function member
    Bar();

    // Ordinary non-static data member
    int i;

    // Ordinary static data member
    static int si;

    // Template function member
    template<typename U>
    void func();

    // Nested template class
    template<typename U>
    class Baz
    {
    public:
        // Ordinary function member
        Baz();

        // Ordinary static data member
        static int sj;
    };
};

template<typename T>
Bar<T>::Bar()
{
    // ...
}

template<typename T>
int Bar<T>::si;

template<typename T>
template<typename U>
Bar<T>::Baz<U>::Baz()
{
    // ...
}

template<typename T>
template<typename U>
int Bar<T>::Baz<U>::sj;
```

Nested template member references require multiple `template<Params...>`
prefixes, starting with the outermost template and working in.

## Template parameters

## Template instantiation

Templates are _instantiated_ when concrete types (or non-type values) are
substituted for the template's parameters. For function templates, the
substitution is often implicit. For class templates, the subsitution is
explicitly given by the user.

```c++ template-implicit-instantiation.cc
template<typename T>
void func(T x)
{
    // ...
}

template<typename T>
class Foo
{
public:
    Foo() {}
};

int main()
{
    // Instantiation with T = int
    func(0);
    
    // Instantiation with T = double
    func(0.0);
    
    // Instantiation with T = bool
    func(true);
    
    // Instantiation with T = int
    Foo<int> fi;
    
    // Instantiation with T = double
    Foo<double> fd;
    
    // Instantiation with T = bool
    Foo<bool> fb;
}
```

The compiler generates code for each template instantiation, as can be seen
with `nm --demangle`:

```txt !c++ -c -o - template-implicit-instantiation.cc | nm --demangle -
```

The fact that two versions of the constructor (seemingly with the same name)
are generated for each template class is a peculiarity of the
[Itanium C++ ABI][itanium-cxx-abi], which defines separate _complete object_
and _base object_ constructors.
