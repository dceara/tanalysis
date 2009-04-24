int main(int argc, char** argv) //argc = G, argv = G
{
    int a;         // T(a) = T, T(b) = T
    a = 2;            // T(a) = U
    while (a < argc)    // T(cond) = U
        a = a + 1;    // T(a) = U  
    return 0;         // T(main) = U
}
 
