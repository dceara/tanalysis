int prod = 1;
int prod2 = 1;

int main()
{
    int tainted;
    int n = 100;
    foo(tainted);
    bar(100);
    return 0;
}

void foo(int n)
{
    prod *= n;
}

/*
void bar(int n)
{
    if (n != 1) {
        prod2 *= n;
    } 
}
*/