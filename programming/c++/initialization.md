# Initialization

### Automatic objects

C++ initializes automatic (local) variables of class type at the point of declaration, and deletes such variables when they go out of scope:

<div class="row">
<div class="column">

```c++ init-auto.cpp
#include <iostream>

struct T {
    T() { std::cout << __func__ << std::endl; }
    ~T() { std::cout << __func__ << std::endl; }
};

int main(int argc, char *argv[])
{
    for (int i = 0; i < 3; ++i)
    {
        T t;
    }
}
```

</div>

<div class="column">

```txt !g++ init-auto.cpp && ./a.out
```

</div>
</div>

Automatic variables of built-in type are not initialized.


### Heap objects

Similarly, objects of class type on the heap are initialized by `operator new` and deleted by `operator delete`:

<div class="row">
<div class="column">

```c++ init-heap.cpp
#include <iostream>

struct T {
    T() { std::cout << __func__ << std::endl; }
    ~T() { std::cout << __func__ << std::endl; }
};

int main(int argc, char *argv[])
{
    for (int i = 0; i < 3; ++i) {
        T *t = new T;
        delete t;
    }
}
```

</div>
<div class="column">

```txt !g++ init-heap.cpp && ./a.out
```

</div>
</div>

As before, obejcts of built-in type on the heap are not initialized.


### Static objects

Objects of class type with static storage duration (that is, global variables, static local variables, and static class members) are initialized once before first use and deleted at program termination:

<div class="row">
<div class="column">

```c++ init-static.cpp
#include <iostream>

struct T {
    T() { std::cout << __func__ << std::endl; }
    ~T() { std::cout << __func__ << std::endl; }
};

void func(void)
{
    static T t;
}

int main(int argc, char *argv[])
{
    for (int i = 0; i < 3; ++i)
    {
        func();
    }
}
```

</div>
<div class="column">

```txt !g++ init-static.cpp && ./a.out
```

</div>
</div>

Objects of built-in type with static storage duration are initialized as in C: they're zero-initialized by the program loader or the language run-time.
