
# Overview #
In the context of vulnerability detection, we present an approach for characterizing the execution paths which may lead to exploitable vulnerabilities. In order to do this, we propose a sound taint analysis calculus which also encompasses a taint dependency sequence (TDS) calculus, making explicit the sequences of control and data dependencies which must be met in order for a variable to become tainted.

We extend the analysis previously performed [here](STACv1.md) by dealing with all the possible kinds of dependencies: data and control flow (both positive and negative).


# Installing #

## Prerequisites ##
The tool has been built as a plug-in for the <a href='http://www.frama-c.com'>Frama-C</a> platform, therefore the main prerequisite is Frama-C itself. Instructions for downloading and installing Frama-C can be found  <a href='http://www.frama-c.com/download.html'>here</a>.
## Installing STAC ##
  1. The sources can be downloaded from <a href=''>here</a>. **TODO**
  1. In the source folder run:
```
make && make install
```
## Uninstalling STAC ##
  1. In the source folder run:
```
make uninstall
```

# Running the analysis #
In order to run Frama-C on a set of C input files use the following command:
```
frama-c [options] [source_files]
```
The full set of options for the STAC plug-in can be obtained when running:
```
frama-c -STAC-help
```
The options include:
  * **-taint-analysis** enables the taintness and TDS computation for the given files.
  * **-disable-alias** disables the <a href='http://cms.dc.uba.ar/materias/aap/2008/c2/descargas/pointsTo.pdf'>may-alias</a> computation performed in the preprocessing phase.
  * **-tds `<`sid`>`** pretty prints the TDS associated to a statement in the program. The statement is specified by using the statement id (sid) assigned by Frama-C.
  * **-print-prg** pretty prints the program while adding the statement ids as comments in the source. This is a way of finding out the ids of the statements we are interested in before using the **-tds** option.
  * **-no-filename** by default, STAC prints the elements of the TDS as pairs (source\_file, line). However, if the analyzed program consists only of one source file, we can suppress printing its name through this option.
  * **-tds-metrics `<`metric`>`** applies the given metric when outputting the TDS. For now, the metric names and implementations are hardcoded in STAC. The possible values are:
    * **no-loop** filters out the TDS which correspond to more than one loop iterations
    * **pure-data** filters out the TDS which contain control dependencies
    * **control** filters out the "pure-data" TDS
    * **pure-control** keeps the TDS which end in a control dependency chain and this is the only control dependency chain in the TDS
  * **-translate-vulnerable** this option is conditioned by the use of the **-taint-analysis** flag. It finds all the statements in the program which correspond to possibly unsafe memory operations (for the moment, a simple pattern of the form _a buffer access to a tainted buffer or with a tainted index_ is used). For each possibly vulnerable statement, a new annotated C file is generated in order to be further analyzed as we will show later.

# Simple Example #
Let's take a look at the following simple example. Let's call it `test.c`:
```
 1 int input()
 2 {
 3   int taint = 0;
 4   int result = taint;
 5   return result;
 6 }
 7 
 8 int main()
 9 {
10   int x, y, z, buf[42];
11   x = 200;
12   y = input();
13   if (x > y)
14     z = y + 1;
15   else
16     z = input();
17   buf[z] = 128;
18   return 0;
19 }
```
We use here the custom function `input()` in order to denote a user-supplied input. The function uses the local variable `taint` in order to denote a tainted variable. For the moment, we have chosen this hardcoded approach just to create a prototype implementation. In a nicer implementation, the inputs should be specified with the help of a config file.

The `main()` function reads a user supplied value for `y` and based on it and `x` sets the value of `z` (either to `20` or to another user supplied value). Therefore, if we compute a taint analysis, we would reach the conclusion that `z` is tainted at the end of the main function. This is not really interesting as we don't really know why `z` is tainted. Is it because of the value of `y`? Is it because of the user supplied value for `z`? Is it because the user may choose which branch of the condition to take?

# Taint Dependency Sequences - TDS #
A TDS associated to a variable will represent a sequence of labels which, if executed in this order, may allow the user to control the value of the variable.

In the previous example, a possible TDS is `<`4,12,14`>`. This corresponds to executing the `input()` function call at line 12 which in turn taints its returned value at line 4 and then assign the returned value to `y`. This value is then used for computing the value of `z` at line 14.

Let's take a look at the results obtained when using STAC. We begin by deciding on a program point on which we want to focus. We have chosen line 17. We need to find the statement id associated to line 17 so we run:
```
frama-c -print-prg test.c
```
and we get a program annotated with the sids. We are interested in the following part:
```
  /* sid:22 */
  buf[z] = 128;
```
which corresponds to the buffer access at line 17. Therefore the sid computed by Frama-C is 21. We now perform the TDS calculus and print the TDS computed for statement 21 (at line 17):
```
frama-c -taint-analysis -no-filename -tds 21 test.c
```
We get the following set of TDS:
```
y:{
  [(DF:(4))(DF:(12))]
}
z:{
  [(DF:(4))(DF:(16))],
  [(DF:(4))(DF:(12))(DF:(14))],
  [(DF:(4))(DF:(12))(CF:(13))]
}
buf:{
  [(DF:(4))(DF:(16))(DF:(17))],
  [(DF:(4))(DF:(12))(DF:(14))(DF:(17))],
  [(DF:(4))(DF:(12))(CF:(13))(DF:(17))]
}
__retres:{}
x:{}
```
If we take a closer look at the TDS associated to `z` we will notice that the TDS we used as an example is there (`<`4,12,14`>`). However, this isn't the only way of tainting `z`. We notice that `z` could be also tainted because of being directly assigned some user supplied value (`<`4,16`>`) or because the assignment which sets `z` can be chosen with the help of `y` which is tainted (`<`4,12,14`>`). As an additional remark, the `DF` and `CF` attributes in the TDS correspond to data dependency and control dependencies respectively.

Well, this was just a small example. More complex examples can be found on the [TDS examples](tdsexamples.md) page.

# Characterizing Vulnerability Activation Inputs #
**TODO**

<img src='http://tanalysis.googlecode.com/svn/wiki/images/jessie_general_approach.jpeg' width='75%' />