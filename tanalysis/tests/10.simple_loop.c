int main(int argc, char** argv) //argc = G, argv = G
{
    int a, b;         // T(a) = T, T(b) = T
    a = 2;            // T(a) = U
    while (a < 10)    // T(cond) = U
        a = a + b;    // T(a) = U  
    return 0;         // T(main) = U
}
 
