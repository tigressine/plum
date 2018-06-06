plomachine
----------
This project is a PL/0 virtual machine and compiler. It's being built for class but it's such an interesting project that I felt it was worthy of a repository. The only component available is the virtual machine at the moment, but this will change as the semester progresses.

setup
-----
To download and setup for use, clone the repository and build from the source files using the following commands. Note that you will need ``git`` and ``gcc`` installed to download and build the program. It's possible that this project could be built with a different C compiler but I haven't tried it.
::
  git clone https://www.github.com/tgsachse/plomachine.git && cd plomachine
  ./Build.sh

This produces an executable named ``plomachine`` that can then be used to run PL/0 btyecode. See below!

usage
-----
Using plomachine is easy! Simply pass your bytecode to the machine as the first argument and the machine will execute that code. This is the command:
::
  ./plomachine yourbytecode.plc

Note that ``yourbytecode.plc`` can be named anything you want. I also made up the file extension (**PL**\\0 **C**ompiled) and for all I know PL/0 bytecode has a different extension. If I ever find out what it is I'll make the appropriate change here.

example
-------
The format for PL/0 bytecode is 4 integers in a row, followed by a newline. It looks like this:
::
  2 2 12 180

The first number is that line's operation code (opcode). The second number is typically the primary register for the instruction and the third number is either the lexicographical level for some operations or the secondary register for others. The last number can be either a constant, a register, or a few other things. These 4 numbers are labeled ``OP R L M`` respectively.

Here's the code for a compiled example program, a factorial function that finds the factorial value of 3:
::
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

Assuming this code is saved in a file named ``TestFactorial.plc`` it can be run with this command:
::
  ./plomachine TestFactorial.plc
 
If you want to view debugging info you have a few options.

``--trace_cpu``:
  Prints the contents of the CPU for each instruction cycle.
``--trace_records``:
  Prints the contents of the activation record stack for each instruction cycle.
``--trace_registers``:
  Prints the contents of the registers for each instruction cycle.
``--trace_all``:
  Prints everything (messy).

This is the produced stack trace when the command ``./plomachine TestFactorial.plc --trace_cpu`` is run:
::
  Program stack trace:
  OP  R  L  M     PC    |
  ----------------------
  INC 0  0  6     1     | 
  LIT 0  0  3     2     | 
  STO 0  0  4     3     | 
  LIT 0  0  1     4     | 
  STO 0  0  5     5     | 
  CAL 0  0  7     7     | 
  INC 0  0  4     8     | 
  LOD 0  1  4     9     | 
  LOD 1  1  5     10    | 
  MUL 1  0  1     11    | 
  STO 1  1  5     12    | 
  LIT 1  0  1     13    | 
  SUB 0  0  1     14    | 
  STO 0  1  4     15    | 
  NEQ 0  0  1     16    | 
  JPC 0  0  18    17    | 
  CAL 0  1  7     7     | 
  INC 0  0  4     8     | 
  LOD 0  1  4     9     | 
  LOD 1  1  5     10    | 
  MUL 1  0  1     11    | 
  STO 1  1  5     12    | 
  LIT 1  0  1     13    | 
  SUB 0  0  1     14    | 
  STO 0  1  4     15    | 
  NEQ 0  0  1     16    | 
  JPC 0  0  18    18    | 
  RTN 0  0  0     18    | 
  RTN 0  0  0     6     | 
  JMP 0  0  19    19    | 
  LOD 0  0  5     20    | 
  6                         <-- System call to print calculated value of 3!
  SIO 0  0  1     21    | 
  SIO 0  0  3     0     |

If you're curious give the other flags a try and see what happens!

bugs
----
Please file an `issue report`_ if you find any bugs. I want this software to be extremely memory-safe and highly stable.

.. _`issue report`: https://github.com/tgsachse/plomachine/issues
