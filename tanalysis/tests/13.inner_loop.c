int main(int argc, char** argv) //argc = G, argv = G
{
    int a, b;
    int a1, b1;
    a = b = 0;
    
    while (a < 10) {
        while (b < 20) {
            b++;
        }
        a++;
    } 
    
    a1 = 0;
    while (a1 < 10) {
        while (b1 < argc) {
            b1++;
        }
        a1++;
    }
    
    return 0;               // T(main) = U
                            // T(a) = T(b) = T(a1) = U
                            // T(b1) = T
}
 
