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
