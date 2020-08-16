template<typename T>
class Foo
{
public:

    void used()
    {
    }

    void unused()
    {
    }
};

int main()
{
    Foo<int> foo;

    foo.used();
}
