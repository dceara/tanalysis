

# Recursion #
```
// test.c
int main(int argc, char** argv)
{
    int x = foo (10, 10);
    int p1 = taint();
    int p2 = taint();
    int y = foo (foo_iterative(p1), p2);
    int tainted = taint();
    int z = foo (tainted, 100);
    int t = foo_iterative(z);
    int q = foo (foo_iterative(tainted), tainted);
    int p = foo (100, tainted);
    
    return 0;
}

int foo_iterative(int n)
{
    int prev;
    int prev_prev;
    int i;
    int aux = taint();
    
    prev = 1;
    prev_prev = 1;
    
    if (n < 1)
        return 0;
    for (i = 2; i <= n; ++ i) {
        aux = prev + prev_prev;
        prev = prev_prev;
        prev_prev = aux;
    }
    return prev_prev;
}

int foo(int n, int x) 
{
    int r = taint();
    if (n > 1)
        r = n * bar(n);
    else 
        r = 1;
    return r;
}

int bar(int n)
{
    int r = taint();
    if (n > 1)
        r = n * foo(n, 100);
    else 
        r = 1;
    return r;
} 
```

---

```
frama-c.byte -taint-analysis -print-results test.c
```

---

```
Environment for function main:
========================================
	Symname: argc = Generic: Gamma(argc), 
	Symname: argv = Generic: Gamma(argv), 
	Symname: x = Untainted
	Symname: p1 = Tainted
	Symname: p2 = Tainted
	Symname: y = Tainted
	Symname: tmp_2 = Tainted
	Symname: tainted = Tainted
	Symname: z = Tainted
	Symname: t = Tainted
	Symname: q = Tainted
	Symname: tmp_7 = Tainted
	Symname: p = Untainted
	Symname: __retres = Untainted
	Symname: main = Untainted
========================================

Environment for function foo:
========================================
	Symname: n = Generic: Gamma(n), 
	Symname: x = Generic: Gamma(x), 
	Symname: r = Generic: Gamma(n), 
	Symname: tmp_0 = Generic: Gamma(n), 
	Symname: foo = Generic: Gamma(n), 
========================================

Environment for function foo_iterative:
========================================
	Symname: n = Generic: Gamma(n), 
	Symname: prev = Generic: Gamma(n), 
	Symname: prev_prev = Generic: Gamma(n), 
	Symname: i = Generic: Gamma(n), 
	Symname: aux = Tainted
	Symname: __retres = Generic: Gamma(n), 
	Symname: foo_iterative = Generic: Gamma(n), 
========================================

Environment for function bar:
========================================
	Symname: n = Generic: Gamma(n), 
	Symname: r = Generic: Gamma(n), 
	Symname: tmp_0 = Generic: Gamma(n), 
	Symname: bar = Generic: Gamma(n), 
========================================
```

---

` frama-c.byte -taint-analysis -print-source test.c `

---

```
int foo(int n , int x ) ;
extern int ( /* missing proto */  taint)() ;
int foo_iterative(int n ) ;
int main(int argc , char **argv ) 
{ int x ;
  int p1 ;
  int p2 ;
  int y ;
  int tmp_2 ;
  int tainted ;
  int z ;
  int t ;
  int q ;
  int tmp_7 ;
  int p ;
  int __retres ;
  
  {/*sid:1*/
  x = foo(10, 10);
  /*sid:1*/
  /*T(argc) = G(argc)*/
  /*T(argv) = G(argv)*/
  /*T(x) = U*/
  /*T(p1) = U*/
  /*T(p2) = U*/
  /*T(y) = U*/
  /*T(tmp_2) = U*/
  /*T(tainted) = U*/
  /*T(z) = U*/
  /*T(t) = U*/
  /*T(q) = U*/
  /*T(tmp_7) = U*/
  /*T(p) = U*/
  /*T(__retres) = U*/
  /*T(main) = G(argc) + G(argv)*/
  
  /*sid:2*/
  p1 = taint();
  /*sid:2*/
  /*T(p1) = T*/
  
  /*sid:3*/
  p2 = taint();
  /*sid:3*/
  /*T(p2) = T*/
  
  /*sid:4*/
  tmp_2 = foo_iterative(p1);
  /*sid:4*/
  /*T(tmp_2) = T*/
  
  /*sid:5*/
  y = foo(tmp_2, p2);
  /*sid:5*/
  /*T(y) = T*/
  
  /*sid:6*/
  tainted = taint();
  /*sid:6*/
  /*T(tainted) = T*/
  
  /*sid:7*/
  z = foo(tainted, 100);
  /*sid:7*/
  /*T(z) = T*/
  
  /*sid:8*/
  t = foo_iterative(z);
  /*sid:8*/
  /*T(t) = T*/
  
  /*sid:9*/
  tmp_7 = foo_iterative(tainted);
  /*sid:9*/
  /*T(tmp_7) = T*/
  
  /*sid:10*/
  q = foo(tmp_7, tainted);
  /*sid:10*/
  /*T(q) = T*/
  
  p = foo(100, tainted);
  __retres = 0;
  /*sid:46*/
  return (__retres);
  /*sid:46*/
  /*T(main) = U*/
  }

}
int foo_iterative(int n ) 
{ int prev ;
  int prev_prev ;
  int i ;
  int aux ;
  int __retres ;
  
  {/*sid:13*/
  aux = taint();
  /*sid:13*/
  /*T(n) = G(n)*/
  /*T(prev) = U*/
  /*T(prev_prev) = U*/
  /*T(i) = U*/
  /*T(aux) = T*/
  /*T(__retres) = U*/
  /*T(foo_iterative) = G(n)*/
  
  prev = 1;
  prev_prev = 1;
  if (n < 1) {/*sid:18*/
    __retres = 0;
    /*sid:18*/
    /*T(__retres) = G(n)*/
    
    goto return_label;}
  
  /*sid:20*/
  i = 2;
  /*sid:20*/
  /*T(i) = G(n)*/
  
  /*sid:21*/
  while (i <= n) {/*sid:25*/
    aux = prev + prev_prev;
    /*sid:25*/
    /*T(aux) = G(n)*/
    
    prev = prev_prev;
    prev_prev = aux;
    /*sid:28*/
    i ++;
    /*sid:28*/
    /*T(aux) = T*/
    }
  
  /*sid:21*/
  /*T(prev) = G(n)*/
  /*T(prev_prev) = G(n)*/
  
  /*sid:29*/
  __retres = prev_prev;
  /*sid:29*/
  /*T(__retres) = G(n)*/
  
  /*sid:48*/
  return_label: /* CIL Label */ 
  return (__retres);
  /*sid:48*/
  /*T(prev) = G(n)*/
  /*T(prev_prev) = G(n)*/
  /*T(i) = G(n)*/
  }

}
int bar(int n ) ;
int foo(int n , int x ) 
{ int r ;
  int tmp_0 ;
  
  {/*sid:30*/
  r = taint();
  /*sid:30*/
  /*T(n) = G(n)*/
  /*T(x) = G(x)*/
  /*T(r) = T*/
  /*T(tmp_0) = U*/
  /*T(foo) = G(n) + G(x)*/
  
  if (n > 1)
  {{/*sid:34*/
    tmp_0 = bar(n);
    /*sid:34*/
    /*T(tmp_0) = G(n)*/
    
    /*sid:35*/
    r = n * tmp_0;
    /*sid:35*/
    /*T(r) = G(n)*/
    }}
  else {/*sid:36*/
  r = 1;
  /*sid:36*/
  /*T(r) = G(n)*/
  /*T(tmp_0) = G(n)*/
  }
  
  /*sid:37*/
  return (r);
  /*sid:37*/
  /*T(foo) = G(n)*/
  }

}
int bar(int n ) 
{ int r ;
  int tmp_0 ;
  
  {/*sid:38*/
  r = taint();
  /*sid:38*/
  /*T(n) = G(n)*/
  /*T(r) = T*/
  /*T(tmp_0) = U*/
  /*T(bar) = G(n)*/
  
  if (n > 1)
  {{/*sid:42*/
    tmp_0 = foo(n, 100);
    /*sid:42*/
    /*T(tmp_0) = G(n)*/
    
    /*sid:43*/
    r = n * tmp_0;
    /*sid:43*/
    /*T(r) = G(n)*/
    }}
  else {/*sid:44*/
  r = 1;
  /*sid:44*/
  /*T(r) = G(n)*/
  /*T(tmp_0) = G(n)*/
  }
  
  return (r);}
}
```

---

# Pointers as parameters #
```
// test.c
int main(int argc, char** argv) 
{
    int tainted = taint();
    int result_test = 1;
    int result_tainted = 1;
    int result_generic = 1;
    int result_generic2 = argc; 
    factorial(&result_test, 100);
    factorial(&result_tainted, tainted);
    factorial(&result_generic, argc);
    factorial(&result_generic2, 1000);
    return 0;
} 


void factorial(int *result, int n)
{
    if (n == 0)
        return;
    *result = *result * n;
    factorial (result, n - 1);
}
```

---

` frama-c.byte -taint-analysis -print-final test.c `

---

```
Environment for function main:
========================================
	Symname: argc = Generic: Gamma(argc), 
	Symname: argv = Generic: Gamma(argv), 
	Symname: tainted = Tainted
	Symname: result_test = Untainted
	Symname: result_tainted = Tainted
	Symname: result_generic = Generic: Gamma(argc), 
	Symname: result_generic2 = Generic: Gamma(argc), 
	Symname: __retres = Untainted
	Symname: main = Untainted
========================================

Environment for function factorial:
========================================
	Symname: result = Generic: Gamma(n), Gamma(result), 
	Symname: n = Generic: Gamma(n), 
	Symname: factorial = Untainted
========================================
```

---

` frama-c.byte -taint-analysis -print-source test.c `

---

```
extern int ( /* missing proto */  taint)() ;
void factorial(int *result , int n ) ;
int main(int argc , char **argv ) 
{ int tainted ;
  int result_test ;
  int result_tainted ;
  int result_generic ;
  int result_generic2 ;
  int __retres ;
  
  {/*sid:1*/
  tainted = taint();
  /*sid:1*/
  /*T(argc) = G(argc)*/
  /*T(argv) = G(argv)*/
  /*T(tainted) = T*/
  /*T(result_test) = U*/
  /*T(result_tainted) = U*/
  /*T(result_generic) = U*/
  /*T(result_generic2) = U*/
  /*T(__retres) = U*/
  /*T(main) = G(argc) + G(argv)*/
  
  result_test = 1;
  result_tainted = 1;
  result_generic = 1;
  /*sid:6*/
  result_generic2 = argc;
  /*sid:6*/
  /*T(result_generic2) = G(argc)*/
  
  factorial(& result_test, 100);
  /*sid:8*/
  factorial(& result_tainted, tainted);
  /*sid:8*/
  /*T(result_tainted) = T*/
  
  /*sid:9*/
  factorial(& result_generic, argc);
  /*sid:9*/
  /*T(result_generic) = G(argc)*/
  
  factorial(& result_generic2, 1000);
  __retres = 0;
  /*sid:19*/
  return (__retres);
  /*sid:19*/
  /*T(main) = U*/
  }

}
void factorial(int *result , int n ) 
{ 
  
  {/*sid:13*/
  if (n == 0) {goto return_label;}
  
  /*sid:13*/
  /*T(result) = G(result)*/
  /*T(n) = G(n)*/
  /*T(factorial) = G(result) + G(n)*/
  
  /*sid:16*/
  *result *= n;
  /*sid:16*/
  /*T(result) = G(n) + G(result)*/
  
  factorial(result, n - 1);
  /*sid:21*/
  return_label: /* CIL Label */ 
  return;
  /*sid:21*/
  /*T(result) = G(n) + G(result)*/
  /*T(factorial) = U*/
  }
}
```

---

# Loops #
```
// test.c
int main(int argc, int argc2, char** argv) //argc = G, argv = G
{
    int a, b, c, d, tainted, i;
    int a1, b1, c1, d1, i1;
    a = b = d = 0;
    c = tainted = taint();
    for (i = 0; i < argc; ++ i) {
        a++;
        while (b < 500) {
            do {
                c = argc + 1;
            } while (c < argc);
        }
        b+=tainted;
    }
    a1 = b1 = c1 = d1 = 0;
    for (i1 = 0; i1 < argc; ++ i1) {
        a1++;
        while (b1 < 500) {
            do {
                c1 = argc + 1;
            } while (c1 < argc); 
        }
        b1+=2;
    }
    return 0;
}
```

---

` frama-c.byte -taint-analysis -print-final test.c `

---

```
Environment for function main:
========================================
	Symname: argc = Generic: Gamma(argc), 
	Symname: argc2 = Generic: Gamma(argc2), 
	Symname: argv = Generic: Gamma(argv), 
	Symname: a = Generic: Gamma(argc), 
	Symname: b = Tainted
	Symname: c = Tainted
	Symname: d = Untainted
	Symname: tainted = Tainted
	Symname: i = Generic: Gamma(argc), 
	Symname: a1 = Generic: Gamma(argc), 
	Symname: b1 = Generic: Gamma(argc), 
	Symname: c1 = Generic: Gamma(argc), 
	Symname: d1 = Untainted
	Symname: i1 = Generic: Gamma(argc), 
	Symname: __retres = Untainted
	Symname: main = Untainted
========================================
```

---

` frama-c.byte -taint-analysis -print-source test.c `

---

```
extern int ( /* missing proto */  taint)() ;
int main(int argc , int argc2 , char **argv ) 
{ int a ;
  int b ;
  int c ;
  int d ;
  int tainted ;
  int i ;
  int a1 ;
  int b1 ;
  int c1 ;
  int d1 ;
  int i1 ;
  int __retres ;
  
  {/*sid:1*/
  {d = 0;
   b = d;
   a = b;}
  /*sid:1*/
  /*T(argc) = G(argc)*/
  /*T(argc2) = G(argc2)*/
  /*T(argv) = G(argv)*/
  /*T(a) = U*/
  /*T(b) = U*/
  /*T(c) = U*/
  /*T(d) = U*/
  /*T(tainted) = U*/
  /*T(i) = U*/
  /*T(a1) = U*/
  /*T(b1) = U*/
  /*T(c1) = U*/
  /*T(d1) = U*/
  /*T(i1) = U*/
  /*T(__retres) = U*/
  /*T(main) = G(argc2) + G(argc) + G(argv)*/
  
  {/*sid:6*/
   tainted = taint();
   /*sid:6*/
   /*T(tainted) = T*/
   
   /*sid:7*/
   c = tainted;
   /*sid:7*/
   /*T(c) = T*/
   }
  i = 0;
  /*sid:9*/
  while (i < argc) {a ++;
    while (b < 500) {while (1) {c = argc + 1;
                       if (! (c < argc)) {break;}
                       }
      }
    
    b += tainted;
    i ++;}
  
  /*sid:9*/
  /*T(a) = G(argc)*/
  /*T(b) = T*/
  /*T(i) = G(argc)*/
  
  {d1 = 0;
   c1 = d1;
   b1 = c1;
   a1 = b1;}
  i1 = 0;
  /*sid:31*/
  while (i1 < argc) {a1 ++;
    while (b1 < 500) {while (1) {c1 = argc + 1;
                        if (! (c1 < argc)) {break;}
                        }
      }
    
    b1 += 2;
    i1 ++;}
  
  /*sid:31*/
  /*T(a1) = G(argc)*/
  /*T(b1) = G(argc)*/
  /*T(c1) = G(argc)*/
  /*T(i1) = G(argc)*/
  
  __retres = 0;
  /*sid:48*/
  return (__retres);
  /*sid:48*/
  /*T(main) = U*/
  }
}
```

---

# Small Picture Editor #
```
// utils.h
#ifndef __UTILS_H
#define __UTILS_H

#include <stdio.h>

#define CHECK(condition, error_label, error_message) \
	do { \
		if (!(condition)) { \
			perror(error_message); \
			goto error_label; \
		} \
	} while(0)

#endif
```

---

```
// common.c
#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "utils.h"

int read_config(char* input_file, int* n, int* config, int max_size)
{
	FILE* fi;
	int i, j, k, _n;
	
	fi = fopen(input_file, "r");
	fscanf(fi, "%d", n);
	_n = *n;
	for(i = 0; i < _n; ++ i) {
		for (j = 0; j < _n; ++ j) {
			for (k = 0; k < _n; ++ k) {
				fscanf(fi, "%d", &config[i * _n * _n + j * _n + k]);
			}
		}
	}
		
	fclose(fi);
	return 0;
read_err:
    fclose(fi);	
fopen_err:
	return -1;
}

static inline int max(int x, int y) 
{
	return x < y ? y : x;
}

static inline int min(int x, int y)
{
	return x < y ? x : y;
}

static inline int max_distance(int x, int y, int z)
{
	return (x + y + z) / 5 + 1;
}

int is_alive(int x, int y, int z, int* config, int n)
{
	int i, j, k, d;
	int alive_cnt = 0;
	int cnt = 0;
	int i_min, i_max, j_min, j_max, k_min, k_max;
	d = max_distance(x, y, z);
	i_min = max(x - d, 0);
	i_max = min(x + d + 1, n);
	j_min = max(y - d, 0);
	j_max = min(y + d + 1, n);
	k_min = max(z - d, 0);
	k_max = min(z + d + 1, n);
	for (i = i_min; i < i_max; ++ i) {
		for (j = j_min; j < j_max; ++ j) {
			for (k = k_min; k < k_max; ++ k) {
				if (x == i && y == j && z == k)
					continue;
				if (config[i * n * n + j * n + k]) 
					++ alive_cnt;
				++ cnt;
			}	
		}
	}
	if (alive_cnt * 100 < cnt * LOWER_DEATH_LIMIT)
		return 0;
	
	if (alive_cnt * 100 > cnt * UPPER_DEATH_LIMIT)
		return 0;
	
	if (alive_cnt * 100 > cnt * LOWER_CHANGE_LIMIT && alive_cnt * 100 < cnt * UPPER_CHANGE_LIMIT) {
		if (config[x * n * n + y * n + z]) 
			return 0;
		return 1;
	}
	return 1;
}

int print_config(char* output_file, int n, int g, int* config, int* alive)
{
	FILE* fo;
	int i, j, k;

	CHECK((fo = fopen(output_file, "w")) != NULL, fopen_err, "Unable to open output file");

	for (i = 0; i < n; ++ i) {
		for (j = 0; j < n; ++ j) {
			for (k = 0; k < n; ++ k) {
				CHECK((fprintf(fo, "%d ", config[i * n * n + j * n + k]) != 0), write_err, "Unable to write to output file");
			}
			fprintf(fo, "\n");
		}
		fprintf(fo, "\n");
	}

	for (i = 0; i < g; ++ i) {
		CHECK((fprintf(fo, "%d\n", alive[i]) != 0), write_err, "Unable to write to output file"); 
	}
	fclose(fo);
	return 0;
write_err:
	fclose(fo);
fopen_err:
	return -1;
}

int dbg_print_config(int n, int* config)
{
	int i, j, k;
	for (i = 0; i < n; ++ i) {
		for (j = 0; j < n; ++ j) {
			for (k = 0; k < n; ++ k) {
				printf("%d ", config[i * n * n + j * n + k]);
			}
			printf("\n");
		}
		printf("\n\n");
	}
	printf("\n");
	return 0;
}
```

---

```
// main.c
#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "utils.h"


#define MAX_N 128
#define MAX_CELLS (MAX_N * MAX_N * MAX_N)
#define MAX_G 256

int cube[MAX_CELLS], alive[MAX_G], tmp_cube[MAX_CELLS];
int n, g;

int main(int argc, char** argv) 
{
	int i, j, k, cnt;
	int* old_config;
	int* curr_config;
	int* aux;

	g = strtol(argv[1], (char **)NULL, 10);

	CHECK((read_config(argv[2], &n, cube, MAX_N) == 0), err, "Unable to read initial config");

	old_config = cube;
	curr_config = tmp_cube;
	for (cnt = 0; cnt < g; ++ cnt) {
		for (i = 0; i < n; ++ i) {
			for (j = 0; j < n; ++ j) {
				for (k = 0; k < n; ++ k) {
					if (is_alive(i, j, k, old_config, n)) {
						curr_config[i * n * n + j * n + k] = 1;
						++ alive[cnt];
					} else {
						curr_config[i * n * n + j * n + k] = 0;
					}
				}
			}
		}
		aux = old_config;
		old_config = curr_config;
		curr_config = aux;
	}

	exit(print_config(argv[3], n , g, old_config, alive));

err:
	exit(1);
}
```

---

` frama-c.byte -taint-analysis -print-final *.c `

---

```
Environment for function main:
========================================
	Symname: cube = Generic: Gamma(cube), 
	Symname: alive = Generic: Gamma(alive), 
	Symname: tmp_cube = Generic: Gamma(tmp_cube), 
	Symname: n = Generic: Gamma(n), 
	Symname: g = Generic: Gamma(g), 
	Symname: argc = Generic: Gamma(argc), 
	Symname: argv = Generic: Gamma(argv), 
	Symname: i = Untainted
	Symname: j = Untainted
	Symname: k = Untainted
	Symname: cnt = Untainted
	Symname: old_config = Untainted
	Symname: curr_config = Untainted
	Symname: aux = Untainted
	Symname: tmp = Untainted
	Symname: tmp_0 = Untainted
	Symname: tmp_1 = Untainted
	Symname: __retres = Untainted
	Symname: main = Generic: Gamma(n), Gamma(alive), Gamma(argv), Gamma(argc), Gamma(cube), Gamma(tmp_cube), Gamma(g), 
========================================

Environment for function max:
========================================
	Symname: cube = Generic: Gamma(cube), 
	Symname: alive = Generic: Gamma(alive), 
	Symname: tmp_cube = Generic: Gamma(tmp_cube), 
	Symname: n = Generic: Gamma(n), 
	Symname: g = Generic: Gamma(g), 
	Symname: max = Generic: Gamma(x), Gamma(y), 
	Symname: x = Generic: Gamma(x), 
	Symname: y = Generic: Gamma(y), 
	Symname: tmp = Generic: Gamma(x), Gamma(y), 
========================================

Environment for function min:
========================================
	Symname: cube = Generic: Gamma(cube), 
	Symname: alive = Generic: Gamma(alive), 
	Symname: tmp_cube = Generic: Gamma(tmp_cube), 
	Symname: n = Generic: Gamma(n), 
	Symname: g = Generic: Gamma(g), 
	Symname: min = Generic: Gamma(x), Gamma(y), 
	Symname: x = Generic: Gamma(x), 
	Symname: y = Generic: Gamma(y), 
	Symname: tmp = Generic: Gamma(x), Gamma(y), 
========================================

Environment for function max_distance:
========================================
	Symname: cube = Generic: Gamma(cube), 
	Symname: alive = Generic: Gamma(alive), 
	Symname: tmp_cube = Generic: Gamma(tmp_cube), 
	Symname: n = Generic: Gamma(n), 
	Symname: g = Generic: Gamma(g), 
	Symname: __retres = Generic: Gamma(y), Gamma(x), Gamma(z), 
	Symname: max_distance = Generic: Gamma(y), Gamma(x), Gamma(z), 
	Symname: x = Generic: Gamma(x), 
	Symname: y = Generic: Gamma(y), 
	Symname: z = Generic: Gamma(z), 
========================================

Environment for function read_config:
========================================
	Symname: read_config = Untainted
	Symname: cube = Generic: Gamma(cube), 
	Symname: alive = Generic: Gamma(alive), 
	Symname: tmp_cube = Generic: Gamma(tmp_cube), 
	Symname: n = Generic: Gamma(n), 
	Symname: g = Generic: Gamma(g), 
	Symname: __retres = Untainted
	Symname: input_file = Generic: Gamma(input_file), 
	Symname: n_0 = Tainted
	Symname: config = Tainted
	Symname: max_size = Generic: Gamma(max_size), 
	Symname: fi = Tainted
	Symname: i = Tainted
	Symname: j = Tainted
	Symname: k = Tainted
	Symname: _n = Tainted
========================================

Environment for function print_config:
========================================
	Symname: print_config = Generic: Gamma(n_0), 
	Symname: cube = Generic: Gamma(config), Gamma(n_0), Gamma(g_0), Gamma(alive_0), Gamma(cube), 
	Symname: alive = Generic: Gamma(config), Gamma(n_0), Gamma(g_0), Gamma(alive_0), Gamma(alive), 
	Symname: tmp_cube = Generic: Gamma(config), Gamma(n_0), Gamma(g_0), Gamma(alive_0), Gamma(tmp_cube), 
	Symname: n = Generic: Gamma(config), Gamma(n_0), Gamma(g_0), Gamma(alive_0), Gamma(n), 
	Symname: g = Generic: Gamma(config), Gamma(n_0), Gamma(g_0), Gamma(alive_0), Gamma(g), 
	Symname: __retres = Generic: Gamma(n_0), 
	Symname: output_file = Generic: Gamma(output_file), 
	Symname: n_0 = Generic: Gamma(n_0), 
	Symname: g_0 = Generic: Gamma(g_0), 
	Symname: config = Generic: Gamma(config), 
	Symname: alive_0 = Generic: Gamma(alive_0), 
	Symname: fo = Untainted
	Symname: i = Generic: Gamma(n_0), Gamma(g_0), 
	Symname: j = Generic: Gamma(n_0), 
	Symname: k = Generic: Gamma(n_0), 
	Symname: tmp = Generic: Gamma(n_0), 
	Symname: tmp_0 = Generic: Gamma(g_0), 
========================================

Environment for function dbg_print_config:
========================================
	Symname: dbg_print_config = Untainted
	Symname: cube = Generic: Gamma(n_0), Gamma(config), Gamma(cube), 
	Symname: alive = Generic: Gamma(n_0), Gamma(config), Gamma(alive), 
	Symname: tmp_cube = Generic: Gamma(n_0), Gamma(config), Gamma(tmp_cube), 
	Symname: n = Generic: Gamma(n_0), Gamma(config), Gamma(n), 
	Symname: g = Generic: Gamma(n_0), Gamma(config), Gamma(g), 
	Symname: __retres = Untainted
	Symname: n_0 = Generic: Gamma(n_0), 
	Symname: config = Generic: Gamma(config), 
	Symname: i = Generic: Gamma(n_0), 
	Symname: j = Generic: Gamma(n_0), 
	Symname: k = Generic: Gamma(n_0), 
========================================

Environment for function is_alive:
========================================
	Symname: is_alive = Generic: Gamma(config), Gamma(z), Gamma(y), Gamma(x), Gamma(n_0), 
	Symname: cube = Generic: Gamma(cube), 
	Symname: alive = Generic: Gamma(alive), 
	Symname: tmp_cube = Generic: Gamma(tmp_cube), 
	Symname: n = Generic: Gamma(n), 
	Symname: g = Generic: Gamma(g), 
	Symname: __retres = Generic: Gamma(config), Gamma(z), Gamma(y), Gamma(x), Gamma(n_0), 
	Symname: x = Generic: Gamma(x), 
	Symname: y = Generic: Gamma(y), 
	Symname: z = Generic: Gamma(z), 
	Symname: config = Generic: Gamma(config), 
	Symname: n_0 = Generic: Gamma(n_0), 
	Symname: i = Generic: Gamma(n_0), Gamma(x), Gamma(y), Gamma(z), 
	Symname: j = Generic: Gamma(z), Gamma(y), Gamma(x), Gamma(n_0), 
	Symname: k = Generic: Gamma(z), Gamma(y), Gamma(x), Gamma(n_0), 
	Symname: d = Generic: Gamma(x), Gamma(y), Gamma(z), 
	Symname: alive_cnt = Generic: Gamma(config), Gamma(z), Gamma(y), Gamma(x), Gamma(n_0), 
	Symname: cnt = Generic: Gamma(z), Gamma(y), Gamma(x), Gamma(n_0), 
	Symname: i_min = Generic: Gamma(x), Gamma(y), Gamma(z), 
	Symname: i_max = Generic: Gamma(z), Gamma(y), Gamma(x), Gamma(n_0), 
	Symname: j_min = Generic: Gamma(x), Gamma(y), Gamma(z), 
	Symname: j_max = Generic: Gamma(z), Gamma(y), Gamma(x), Gamma(n_0), 
	Symname: k_min = Generic: Gamma(x), Gamma(y), Gamma(z), 
	Symname: k_max = Generic: Gamma(z), Gamma(y), Gamma(x), Gamma(n_0), 
========================================
```

---
