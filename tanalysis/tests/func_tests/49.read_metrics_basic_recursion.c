int main(int argc, char** argv) //argc = G, argv = G
{
    int a, b;
    int a1, b1;
    a = b = 0;              
    a1 = b1 = taint();      
    
    while (a < 10) { 
        while (b < 20) {
            b++;
            taint();        
        }
        a++;
    } 
    
    /* 
    a1 = 0;  
    while (a1 < 10) {
        while (b1 < argc) { 
            b1++;          
        }
        a1++;              
    } */
    
    return 0;
}
 
