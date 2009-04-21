typedef struct {
  int x;
  int y;
} Node;

int *pa, *pb;

int buf[256];
int x;
float y;

void a()
{
	b();
}

void b() 
{
	c();
	d();
	f();
}

void c() 
{
	a();
	d();
}

void d() 
{
	e();
}

void e() 
{
	d();
}

void f() 
{
	e();
	g();
}

void g() 
{
	f();
	h();
}

void h() 
{
	g();
}

int test(int n)
{
	if (n > 0)
		test(n-1);
}

int main (int argc, char** argv)
{
	return test(argc);
	/* int a;
	
	a = 2;
	return 0;
	*/
	/*
	int a = 2, c =5;
	int* p = buf;

	*(p + 15) = 20;
	++p;
	if (c == 2)
		p[14] = a * 25 + 13;
	else 
		a = 512;
	--p;
	c = a + p[15];
	return c;
	*/ 
}   
