

# Overview #
The goal of the project is to analyze C programs and identify potentially harmful instructions (or ideally patterns) from the security point of view. The more general objective is to detect software vulnerabilities. To achieve this goal, the taint analysis will be used to facilitate the construction of test-cases for detecting activable vulnerabilities inside a modular approach in the <a href='http://vulcain.imag.fr/index.html'>MSTIC Vulcain project</a>.

# Project Description #
This is a plug-in for the <a href='http://frama-c.cea.fr'>Frama-C</a> platform. The plug-in performs interprocedural taint analysis on C source files. An (over)aproximation of the set of instructions that are influenced by user input is detected.

## Expected Tool Structure ##
  * the result of the analysis is an annotated program. The output will attach to each instruction the taint value for all the variables used by the instruction.
  * detection of potentially dangerous instructions (different schemas with tainted arguments):
```
      int a[SIZE];
      int index = read_int(); //index = tainted
      a[index] = some_value;
```
  * optionally slice the program for some given dangerous instruction to simplify the analysis such that only the instructions on which the dangerous ones are dependent are analyzed
  * defining metrics for evaluating different paths that may lead to the execution of dangerous instructions (i.e.: the number of parameters that must be tainted or the number on user dependent inputs required to activate the dangerous instruction)

# Installing #
## Pre-requisites ##
The analysis is built as a Frama-C plug-in so the main pre-requisite is the Frama-C platform.
Frama-C and the installation instructions can be found <a href='http://frama-c.cea.fr/download.html'>here</a>.
## Downloading the sources ##
The sources for the plug-in are available <a href='http://code.google.com/p/tanalysis/source/browse/#svn/trunk/tanalysis'>here</a>.
The plug-in requires that library functions should be annotated (because we don't have access to their source code). A tool for updating configuration files can also be downloaded from <a href='http://code.google.com/p/tanalysis/source/browse/#svn/trunk/TaintConfigEditor/TaintConfigEditor'>here</a>.
## Building the plug-in ##
In the sources directory:
  1. run the command:
```
       make build
```
  1. as root, run the command:
```
       make install
```
## Uninstalling the plug-in ##
In the sources directory, as root, run the command:
  * 
```
       make uninstall
```
# Running the analysis #
To run frama-c on some input use the following command:
`frama-c.byte [options] [source_files]`
## Options ##
  * **-taint-analysis**: the main switch for the plug-in. Enables the analysis.
  * **-main-function**: the name of the main function. If a value is provided, all the arguments for the supplied main function will be considered as user supplied (tainted).
  * **-print-intermediate**: for each analysis of a function, prints the taint values for its variables after it has been analyzed.
  * **-print-final**: for each function in the program, prints the taint values for its variables after the whole program has been analyzed.
  * **-print-source**: after the analysis is performed, the source code is rewritten adding taint annotation as C comments
  * **-do-results**: compute a percentage of tainted instructions with respect to the number of instructions executed in all possible contexts of the functions. An instruction is considered tainted if it assigns (directly or indirectly) a variable with a tainted value.
  * **-do-prepare-slice** : prints a source file prepared for slicing for vulnerable statements. **Still experimental.**
  * **-do-min-read-metrics** : Computes the execution path that will have the minimum read function calls.
  * **-do-max-read-metrics** : Computes the execution path that will have the maximum read function calls.
  * **-do-min-taint-metrics** : Computes the execution path that will have the minimum number of possibly tainted variables.
  * **-do-max-taint-metrics** : Computes the execution path that will have the maximum number of possibly tainted variables.
  * **-config-file**: specifies the path to the file containing the summaries for library functions. If this option isn't used the plug-in searches for a default configuration file (default.cfg) in the current directory.
  * **-taint-debug**: turn ON debug logging.
  * **-taint-info**: turn ON info logging.

# Examples #
## Printing final results ##
For an input like this one:
```
// test.c
int main(int argc, char** argv) 
{
    int x = bar(0, 1, 2);
    int y = bar(0, 1, argc);
    return 0;
}

int foo(int x3) 
{
    return 10;
}

int bar(int x1, int y1, int z1) 
{
    return x1 + foobar(z1, x1, y1);
}

int foobar(int x2, int y2, int z2)
{
    return x2 + foo(y2);
}
```
running the command: `frama-c.byte -taint-analysis -print-final test.c`
will produce the result:
```
Environment for function main:
========================================
	Symname: argc = Generic: Gamma(argc), 
	Symname: argv = Generic: Gamma(argv), 
	Symname: x = Untainted
	Symname: y = Generic: Gamma(argc), 
	Symname: __retres = Untainted
	Symname: main = Untainted
========================================

Environment for function bar:
========================================
	Symname: x1 = Generic: Gamma(x1), 
	Symname: y1 = Generic: Gamma(y1), 
	Symname: z1 = Generic: Gamma(z1), 
	Symname: tmp = Generic: Gamma(z1), 
	Symname: __retres = Generic: Gamma(x1), Gamma(z1), 
	Symname: bar = Generic: Gamma(x1), Gamma(z1), 
========================================

Environment for function foo:
========================================
	Symname: x3 = Generic: Gamma(x3), 
	Symname: __retres = Untainted
	Symname: foo = Untainted
========================================

Environment for function foobar:
========================================
	Symname: x2 = Generic: Gamma(x2), 
	Symname: y2 = Generic: Gamma(y2), 
	Symname: z2 = Generic: Gamma(z2), 
	Symname: tmp = Untainted
	Symname: __retres = Generic: Gamma(x2), 
	Symname: foobar = Generic: Gamma(x2), 
========================================
```
## Printing the annotated source code ##
For the same input, when running the command: ` frama-c.byte -taint-analysis -print-source test.c`, we get this annotated source file:
```
/* Generated by CIL v. 1.3.6 */
/* print_CIL_Input is false */

int bar(int x1 , int y1 , int z1 ) ;
int main(int argc , char **argv ) 
{ int x ;
  int y ;
  int __retres ; 
  {/*sid:1*/
  x = bar(0, 1, 2);
  /*sid:1*/
  /*T(argc) = G(argc)*/
  /*T(argv) = G(argv)*/
  /*T(x) = U*/
  /*T(y) = U*/
  /*T(__retres) = U*/
  /*T(main) = G(argc) + G(argv)*/
  
  /*sid:2*/
  y = bar(0, 1, argc);
  /*sid:2*/
  /*T(y) = G(argc)*/
  
  __retres = 0;
  /*sid:9*/
  return (__retres);
  /*sid:9*/
  /*T(main) = U*/
  }
}
int foo(int x3 ) 
{ int __retres ; 
  {/*sid:4*/
  __retres = 10;
  /*sid:4*/
  /*T(x3) = G(x3)*/
  /*T(__retres) = U*/
  /*T(foo) = G(x3)*/
  
  /*sid:10*/
  return (__retres);
  /*sid:10*/
  /*T(foo) = U*/
  }
}
int foobar(int x2 , int y2 , int z2 ) ;
int bar(int x1 , int y1 , int z1 ) 
{ int tmp ;
  int __retres ; 
  {/*sid:5*/
  tmp = foobar(z1, x1, y1);
  /*sid:5*/
  /*T(x1) = G(x1)*/
  /*T(y1) = G(y1)*/
  /*T(z1) = G(z1)*/
  /*T(tmp) = G(z1)*/
  /*T(__retres) = U*/
  /*T(bar) = G(y1) + G(x1) + G(z1)*/
  
  /*sid:6*/
  __retres = x1 + tmp;
  /*sid:6*/
  /*T(__retres) = G(x1) + G(z1)*/
  
  /*sid:11*/
  return (__retres);
  /*sid:11*/
  /*T(bar) = G(x1) + G(z1)*/
  }
}
int foobar(int x2 , int y2 , int z2 ) 
{ int tmp ;
  int __retres ; 
  {/*sid:7*/
  tmp = foo(y2);
  /*sid:7*/
  /*T(x2) = G(x2)*/
  /*T(y2) = G(y2)*/
  /*T(z2) = G(z2)*/
  /*T(tmp) = U*/
  /*T(__retres) = U*/
  /*T(foobar) = G(y2) + G(x2) + G(z2)*/
  
  /*sid:8*/
  __retres = x2 + tmp;
  /*sid:8*/
  /*T(__retres) = G(x2)*/
  
  /*sid:12*/
  return (__retres);
  /*sid:12*/
  /*T(foobar) = G(x2)*/
  }
}

```
## Computing taint metrics ##
For an input like this one:
```
// test.c
void main()
{
    int a;
    int c;
    
    if (c) {
        a = 2;
    } else {
        a = taint();
    }              
    
    while (a < 10) { 
        c = taint ();
    }
    
    return;
}
```
running the command: `frama-c.byte -taint-analysis -do-max-taint-metrics test.c`
will produce the result:
```
(MaxTaintMetric (NonCyclic (DepVar 0, PosTaint 1), Cyclic (DepVar 0, PosTaint 2)))
/* Generated by CIL v. 1.3.6 */
/* print_CIL_Input is false */

extern int ( /* missing proto */  taint)() ;
void main(void) 
{ int a ;
  int c ;
  
  {/*BEST_PATH*/
  if (c) {a = 2;}
  else {/*BEST_PATH*/
  a = taint();}
  
  /*BEST_PATH*/
  while (a < 10) {/*BEST_PATH*/
    c = taint();}
  
  /*BEST_PATH*/
  return;}
}
```
More examples can be found on the [examples](Examples.md) page.

# Taint Experimental results #
After running the analysis on different inputs we obtained these results:

| Program | # lines of code | # executed instructions in all calling contexts | # tainted instructions | tainted percentage |
|:--------|:----------------|:------------------------------------------------|:-----------------------|:-------------------|
| Queue   | 227             | 80                                              | 55                     | 68.7%              |
| ABR     | 408             | 1937                                            | 1828                   | 94.3%              |
| Huffman | 499             | 336                                             | 220                    | 65.4%              |
| Link Editor | 6732            | 6686                                            | 4362                   | 65.2%              |
| <a href='http://packages.debian.org/sid/mailx'>mailx</a> | 14609           | 38147                                           | 22214                  | 58.2%              |


# Taint Metrics Implementation Overview #
The metrics computation has been built as a generic graph computation in the following way. A global graph is built based on the control flow graph. All the blocks in the control flow graph are considered nodes in the graph, except function calls. Each basic block that contains a function call is split into two nodes (a begin\_call node and an end\_call node). For each begin\_call node of a given function an edge is created from the begin\_call node to the entry point of the function. For each end\_call node of a given function an edge is created from the exit point of the function to the end\_call node.

The generic computation module will require the other modules that use it to supply metrics values and metrics operators for each edge in the graph with respect to the edge margins.

Because the metrics module must compute the "best" execution path (that is the path with the "biggest" metric) loops (as normal program constructs or as mutual recursive functions) represent a problem. So, at first the loops are detected using <a href='http://en.wikipedia.org/wiki/Tarjan%27s_strongly_connected_components_algorithm'>Tarjan's strongly connected components algorithm</a>. For each loop the back edges are removed so that the new graph will not contain any cycles. Afterward, the loop edges that remain in the graph will never be selected into the best path because of the removed back edges.

Now, with our new acyclic directed graph we can reduce the problem to a graph shortest path problem by negating the values on the remaining edges. After running <a href='http://en.wikipedia.org/wiki/Dijkstra%27s_algorithm'>Dijkstra's algorithm</a> we obtain the best path with respect to our metrics in the acyclic directed graph.

The last step in our approach is to add the previously removed back edges that are on the best path, and for each back edge add the value of the loop it represents as a supplementary metric value.

# Limitations & Not implemented features #
  * no support for multi-threading.
  * the memory locations are considered independent (you cannot navigate from a variable to another variable using pointers)
  * for now the plug-in doesn't perform any kind of alias analysis which may result in false negatives
  * ...