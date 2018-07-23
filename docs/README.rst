plum
----------
Plum is an interpreter designed to run PL/0 code. It is modal, and is capable of parsing,
scanning, compiling, assembling, and executing programs. Compiled programs are executed
on a provided stack-based virtual machine. Check out `the manual`_ for a full description of
Plum's modes and flags.

Plum is derived from PLVM, an acronym for PL/0 Virtual Machine. The name is a bit of a
misnomer, as Plum is no longer only a virtual machine... but the name is cool, so it stays!

setup
-----
To download and setup for use, clone the repository and build from the source files using the following commands. Note that you will need ``git`` and ``gcc`` installed to download and build the program. It's possible that this project could be built with a different C compiler but I haven't tried it.
::
    git clone https://www.github.com/tgsachse/plum.git && cd plum
    ./build.sh

This command will produce an executable program named ``plum`` in the same folder as
the build script. This executable is the entire interpreter and can be used to run
your PL/0 files!

usage
-----
Plum's syntax looks like this:
::
    ./plum <mode> <filename> <--options>

The mode and filename are required. 0 or more options can be included but must trail
the filename and mode as shown above. Here are a few example commands to get you started:
::
    ./plum run program1.plo
    ./plum run program2.plo --print-symbol-table --trace-cpu
    ./plum compile program3.plo --print-assembly --print-lexeme-table
    ./plum execute program4.plc --trace-all
    ./plum scan program5.plo --outfile lexemes.txt

bugs
----
Please file an `issue report`_ if you find any bugs. My goal is  make this software extremely memory-safe and highly stable.

.. _`the manual`: https://github.com/tgsachse/plum/blob/master/docs/MANUAL.pdf
.. _`issue report`: https://github.com/tgsachse/plomachine/issues
