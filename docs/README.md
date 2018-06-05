# plomachine

This project is a PL/0 virtual machine and compiler. It's being built for class but it's such an interesting project that I felt it was worthy of a repository. The only component available is the virtual machine at the moment, but this will change as the semester progresses.

# setup
To download and setup for use, clone the repository and build from the source files using the following commands. Note that you will need `git` and `gcc` installed to download and build the program. It's possible that this project could be built with a different C compiler but I haven't tried it.
```
git clone htttps://www.github.com/tgsachse/plomachine.git && cd plomachine
./Build.sh
```
This produces an executable named `plomachine` that can then be used to run PL/0 btyecode. See below!

# usage
Using plomachine is easy! Simply pass your bytecode to the machine as the first argument and the machine will execute that code. This is the command:
```
./plomachine yourbytecode.plc
```
Note that `yourbytecode.plc` can be named anything you want. I also made up the file extension (**PL**\0 **C**ompiled) and for all I know PL/0 bytecode has a different extension. If I ever find out what it is I'll make the appropriate change here.

# example
The format for PL/0 bytecode is 4 integers in a row, followed by a newline. It looks like this:
```
2 2 12 180
```
The first number is that line's operation code (opcode). The second number is typically the primary register for the instruction and the third number is either the lexicographical level for some operations or the secondary register for others. The last number can be either a constant, a register, or a few other things. These 4 numbers are labeled `OP R L M` respectively.

Here's the code for a compiled example program, a factorial function:
```
6 0 0 6
1 0 0 3
4 0 0 4
1 0 0 1
4 0 0 5
5 0 0 7
7 0 0 19
6 0 0 4
3 0 1 4
3 1 1 5
13 1 0 1
4 1 1 5
1 1 0 1
12 0 0 1
4 0 1 4
18 0 0 1
8 0 0 18
5 0 1 7
2 0 0 0
3 0 0 5
9 0 0 1
9 0 0 3
```
Assuming this code is saved in a file named `TestFactorial.plc` it can be run with this command:
```
./plomachine TestFactorial.plc
```
It will produce the following output by default:
```
<added soon>
```
