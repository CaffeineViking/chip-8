Design
======

This document is here to get a rough idea on how things are going to be implemented. Memory, processor, pipeline, display, keyboard, sound and timer are the modules I think are needed. These might not be separeted files by themselves, but these modules are more here to divide functionallity I think needs to be digested in chunks. Memory is problem, processor another (not related) etc... Emulator/interpreter will be written in C/C++ using very little stdlib functionality, most dependencies will lie on SDL2.

Memory
------

Should contain a piece of allocated memory, probably best to have the user of the class decide how much. Still haven't decided if it is best to dynamically allocate this via new or malloc or just use stack allocation for this. Storing memory with the 'char' datatype seems like a good idea since we are going to index 8-bits anyway. A piece of memory should only be able to read/write, thus, only these two methods should be implemented. Upon construction of this object, insert data that is to be contained in the interpreter section, the interpreter's memory area should not be accessible by the user program at any time unless it is through a specific instruction (like font loading).

    Memory = (byte_1, byte_2, ..., byte_n), where 'n' is the desired size in bytes of the piece of memory.
    Valid(Memory, Address) -> Boolean, returns 'true' if the provided address is within program bounds.
    Read(Memory, Address) -> Byte, reads the byte located at the specified address, checks for address validity first.
    Write(Memory, Address, Byte) -> Void, writes to the selected memory location a byte, also check for address validity.

Processor
---------

The processor module should have all information needed to function as a working computing device, that is, it should have all of the registers required and it should also create a link to the memory module so it can interface with the user program and interpreter data. How it will do this is still undecided, either by providing the memory as a reference or providing it whenever a  memory instruction is processed. Every 'tick' the processor should fetch the next instruction and process it, meaning, doing what the instruction actually says. This procedure deserves a part of its own, so it is left to the next section. Additionally I am thinking of adding a small display buffer to the processor, which is not too unrealistic since it should be contained in fast memory. This is mostly needed so easier interfacing with the display module works, not needing to pass a reference to it every time.

    Stack = (word_0, word_1, word_F), the stack should have 16 fast memory places, contained within the processor. Each is a 16-bit value usually used to store intermediate jump addresses between subroutine calls.
                                      The top of the stack is remembered by the SP (Stack Pointer) which is a 8-bit value. It is unclear if the stack should be stored in the actual processor structure.
    Processor = (PC, SP, DT, ST, I, V0, V1, ..., VF), where 'Vx' is a 8-bit general purpose register, PC and I are 16-bit registers, ST and DT are special purpose registers and finally SP is a 8-bit stack pointer.
                                                    Additionally, the VF register should not be read by the user program (unclear if it should be enforced). The I register is used for storing addresses. I think
                                                    8-bit registers should be stored as 'char's and 16-bit registers should be stored as 2 'char's in a array (for simplicity).
    Tick(Processor, Memory) -> Void, steps the processor by reading the next instruction at location PC (every instruction is 2 bytes long). After reading it matches the read instruction and executes it. See the next section.
                                     In a nutshell this is what a processor does, changes state by getting and executing the next instruction. Not many more actions are needed, it is quite self contained.
    DisplayBuffer(Processor) -> Bitmap, gets the stored display buffer in the fast memory area of the processor, this is usually going to be called by the display so it can actually draw the frame. This is probably not how the actual
                                        Chip-8 works but it is easy and should work. It is also good to separate these two modules (processor and display) since display will mostly be a SDL2 texture, making processor independent.
    DelayDone(Processor) -> Boolean, returns 'true' if the delay timer is done, that is, if the timer is zero. Will be used to message outside world of its state.
    SoundDone(Processor) -> Boolean, returns 'true' if the sound timer is done, that is, if the timer is zero. Used to message the sound implementation to sound the buzzer if false.
    Interrupt(Processor, Flag) -> Void, interrupts the processor to get some information from the outside world, it doesn't work exactly like the real interrupt routines in a real processor. It simply is called when things like
                                        a keyboard is pressed or released or when the 60 Hz timer has made a loop (decrement delay and sound registers then provided they are non-zero).

Processor Pipeline
------------------

This section (and module) are concerned with the fetching of the instructions from memory, execution of these, in fact, everything related to the processor that is not immediatly obvious in the above section. When running the Tick(Processor, Memory) function, first, the desired instruction at PC is fetch from memory. If PC is 0x200, it will fetch 0x200 - 0x201, since every instruction is 2 bytes long. When loaded, the processor will parse the instruction to determine what to do next. For example, consider: ADD Vx, Vy (hex-representation: $8xy3), the processor parses the hex value 8xy3 and sees that this is a add instruction, then adds the Vx and the Vy register, storing the result into the Vx register (this is done via the ALU, but here it doesn't really matter since this is an emulator). After the instruction is done, the PC is incremented (thus, now pointing at address 0x202, which is even). In a nutshell:

1. Fetch instruction OP at PC. A single instruction is 2 bytes long.
2. Parse OP, determine what instruction is going to be run. Maybe have an enumerator for this?
3. Fetch operands related to OP, these are determined by the actual instruction, so it varies.
4. Execute the instruction, storing possible results in the given operands of OP.
5. Increment the PC, making sure that the address is even.

n. An interrupt from the outside can happen.
n + 1. Fetch operands related to the interrupt being called (things like keyboard, sound, timer related things).
n + 2. Do stuff depending on the type of interrupt, usually modifying the state of the processor.
n + 3. Nothing more is needed, this might seem unrealistic, but this is an emulator after all ;)

Display
-------

Keyboard
--------

Sound and Timer
---------------
