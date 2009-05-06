int main()
{
    int a, b;
    
    b = 200;
    if (b) {
        if (a) {
            a = b = 10;
            a*= b;
        } else {
            a = 500;
        }
    }
    return a;
}