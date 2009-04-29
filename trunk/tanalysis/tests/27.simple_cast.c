typedef struct {
    int a;
    char c1;
    char c2;
    char c3;
    char c4;
} t1;

typedef struct {
    int a;
    char c[8];
} t3;

typedef struct {
    int a;
    int b;
    int c;
} t4;

typedef struct {
    int a;
    int b;
} t2;

int main(int argc, char** argv) //argc = G, argv = G
{
    t1* s1;
    t3* s3;
    t2 s2;
    t4 s4;
    s2.a = 10;
    s2.b = 10;
    s4.a = s4.b = s4.c = 10;
    s1 = (t1*)(&s2);
    s3 = (t3*)(&s2);            // s3 should become tainted here because of the
                                // array
    return 0;
}
 