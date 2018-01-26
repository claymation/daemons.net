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
