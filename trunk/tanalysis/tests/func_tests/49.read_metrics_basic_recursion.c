int main(int argc) 
{
    foo(222);
    return 0;
}

void foo(int n)
{
    if (n) {
        foo(n-1);
    }
    if (n > 10) {
    }
    return;
}
