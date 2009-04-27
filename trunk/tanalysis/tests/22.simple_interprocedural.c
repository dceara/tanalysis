int main(int argc, char** argv) //argc = G, argv = G
{
    int a = 2, b;
    int c = foo (a, b);
    int d = foo (a, argc);
    int e = foo (a, foo(a, d));
    return 0;
}

int foo(int x, int y)
{
    return x + y;
}
 
 