int global = 0;

int main()
{
    foo();
    untaint_global();
    return 0;
}

void foo()
{
    int tainted;
    global = tainted;
}

void untaint_global()
{
    global = 100;
}
