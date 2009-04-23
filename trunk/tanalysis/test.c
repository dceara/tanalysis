/* typedef struct {
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
*/

typedef struct {
	int a;
	int b;
} s_type;

int main (int argc, char** argv)
{
	int a, b, c;
	s_type s;
	s.a = 2;
	
	if (s.a = 3)
		b = s.a;
	else
		b = argc + 15 * s.a;
	return b;
}   
