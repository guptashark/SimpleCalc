# SimpleCalc

## About

SimpleCalc is a simple [prefix expression](https://en.wikipedia.org/wiki/Polish_notation) calculator. The format of the operations is: 

**oper arg1 arg2**

where **oper** is binary function that takes arg1 and arg2 as its arguments and outputs a result. **arg1** and **arg2** are both (currently) integers. The only other input that the calculator will understand is **help** which will list all the math operations that the calculator can do, and **quit** which exits the program. The calculator also exits on **Ctrl-D**. 

## Building
The code is compiled with g++ set to the c++17 standard. Navigate to the project directory and type in "make" and you should have the runnable executable. The code doesn't yet install the calculator into the system, as in modify the path variable or anything like that as of yet. 

## Usage Examples
```shell
$ ./a.out
~> * 3 4
12
~> / 6 2
3
~> - 12 20
-8
~> + 9 3 
12
~> quit
$
```
