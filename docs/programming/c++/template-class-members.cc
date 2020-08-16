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
