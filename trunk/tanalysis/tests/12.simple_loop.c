int main(int argc, char** argv) //argc = G, argv = G
{
    int a, b;         // T(a) = T, T(b) = T
    a = 2;            // T(a) = U
    while (a < argc)    // T(cond) = G(argc)
        a = a + b;    // T(a) = G(argc) + U + T = T  
    return 0;         // T(main) = U
}
 
