Design
======

This document is here to get a rough idea on how things are going to be implemented. Memory, processor, pipeline, display, keyboard, sound and timer are the modules I think are needed. These might not be separeted files by themselves, but these modules are more here to divide functionallity I think needs to be digested in chunks. Memory is problem, processor another (not related) etc... Emulator/interpreter will be written in C/C++ using very little stdlib functionality, most dependencies will lie on SDL2.

Memory
------

Should contain a piece of allocated memory, probably best to have the user of the class decide how much. Still haven't decided if it is best to dynamically allocate this via new or malloc or just use stack allocation for this. Storing memory with the 'char' datatype seems like a good idea since we are going to index 8-bits anyway. A piece of memory should only be able to read/write, thus, only these two methods should be implemented.

    Memory = (byte_1, byte_2, ..., byte_n), where 'n' is the desired size in bytes of the piece memory.
    Read: (Memory, Byte) -> Byte, should read from memory at a certain location and return the data within.
    Read(M, n) = M_n
    Write: (Memory, Byte) -> Byte, should write to memory at a certain location.
    Write(M, n, d) = d, M_n = Write(M, n, d)
    Valid(M, n) = {if n > PROGRAM_BOUND or n <= INTERPRETER_BOUND then false else true}, bound check if program is writing/reading correctly. Interpreter data is written on construction, thereafter: no more.

Processor
---------

Processor Pipeline
------------------

Display
-------

Keyboard
--------

Sound and Timer
---------------
