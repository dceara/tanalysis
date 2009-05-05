int main(int argc, char** argv) //argc = G, argv = G
{
    int a2 = 2, b, b2 = 2;
    while (a2 < 10) {
        while (b2 < argc) {
            b2 = 1;
        }
        a2 = b;                     // !!!! a2 becomes tainted -> first cond becomes
                                    // tainted -> b2 becomes tainted
    }
}
 
