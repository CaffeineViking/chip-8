Specification
=============

This is a short technical specification for the Chip-8, mostly taken from the amazing [Cowgod's Chip-8 Technical Reference][1]. It describes the requirements for the interpreter to work on already written software, thus, enabling one to run existing ROMs written for real computers written to work on the Chip-8 specification. Memory, registers, keyboard, display, fonts, timers and sound need to be emulated. All of the standard Chip-8 instructions will be implemented.

Memory
------

The standard Chip-8 language is built for machines that can address memory locations from 0x000 to 0xFFF, a whopping 4096 bytes of memory. For an instruction to uniquely address a location, 12 bit sized registers are needed. The Chip-8 specification later solves this by providing a 16 bit sized register I. Locations 0x000 - 0x1FF (512 bytes) are specific to the interpreter itself, based on the specification. Programs for the classic Chip-8 begin at address 0x200, and are allocated the rest of the memory: 0x200 - 0xFFF (4096 - 512 = 3584 bytes). Since we are writing an accurate interpreter, these boundaries will be preserved, programs will also start at 0x200 BUT can be changed to start at a different entry-point by providing an argument to the interpreter.

Registers
---------

There are a total of 16 general purpose 8-bit registers on the Chip-8, these are usually referred to as 'Vx' (where the 'x' ranges from 0-F). There is also a 16-bit register called 'I', this register is usually used to store memory addresses (leaving some bits unused, since memory only needs 12 bits). Additionally there is the static 'VF' register, it contains all of the status flags required by the interpreter (more on that soon). There are two special purpose 8-bit registers reserved for timers and sound, when these registers are non-zero, they auto-decrement at a rate of 60 Hz. There are two other registers that are not accesible by the program, the PC (Program Counter) and SP (Stack Pointer). The PC is obviously 16 bits, since it deals with memory. The SP can be 8 bit sized, since, officially there are only 16 16-bit sized places on the stack (the SP points to the top), it can also be 16-bits. Since there are 16 places for the stack, a subroutine nesting level of 16 can be reached.

Keyboard
--------

The official Chip-8 keyboard has 16 buttons, and logically, thus has hexadecimal labels. This is the only input type the user has access to.

Display
-------

The original Chip-8 has a 64x32-pixel monochrome display, where (0, 0) is located on the top-left part of the display and (63, 31) is located at the bottom-right. Chip-8 draws graphics on the screen by the means of sprites, thus, one never draws only a single pixel (unless the sprite is only a pixel in-itself). The size of a sprite on the original Chip-8 is 15 bytes, allowing a 8x15 sprite to be created. Additionally the fonts are stored in this format on the reserved memory space of the interpreter, however only using 8x5 sized sprites. The official requirement is that the hexadecimal characters be present there (need to find out official standard location for these...).

Timers and Sound
----------------

The two special purpose registers previously named are the DT (Delay Timer) and ST (Sound Timer). The delay timer is active whenever the DT register is non-zero, when active it will keep decrementing itself at a rate of 60 Hz until it is zero again. When zero, it will deactivate itself, not decrementing anymore. The same strategy is applied to the sound timer, however, whenever the ST register has a value above zero the system buzzer will sound. The sound timer will be deactivated when reaching zero, thus, stopping the buzzer. There is only one tone on the Chip-8, decided by the implementor.

Instructions
------------

There are a total of 36 instructions in the official Chip-8. Every instruction is 2 bytes long, and are stored most-significant byte first. In memory the most significant byte should be contained at an even address, if a program includes sprite data it should be padded so any instructions following it will be properly situated in RAM (this is up to the user i guess...). See technical specification linked here to see format of these instructions.

Summary
-------

* (1) At least 4096 bytes of memory. [0x000, 0x200[ given to interpreter (512 bytes) and [0x200, 0xFFF] for program (3584 bytes).
* (2) Programs should start at address 0x200, give option to override this. Basically, at program start set PC to desired entry-point (usually 0x200).
* (3) A total of 16 general purpose 8-bit registers, V0 - VF (hexadeximal notation). VF "should" not be used by any program since it is used as a flag register by some instructions.
* (4) There is also the 16-bit I register, used to store memory addresses (only the lowest 12-bits are used since memory only needs that).
* (5) Two special purpose registers for delay and sound timers, both are 8-bit and are automatically decremented every 60 Hz when non-zero.
* (6) Good old 16-bit PC and 8-bit (maybe) SP. The stack has 16 places, 16-bits each. Maybe store this in the cpu itself instead of main memory?
* (7) Hexadecimal keyboard, keys labled 0-F. Just mapping these to respective keys on a modern keyboard should work.
* (8) Need a 64x32 display buffer for the monochrome display, the origin is located on the top-left part of the screen (0, 0).
* (9) The Chip-8 draws by using sprites, it can draw 15 byte sized sprites per draw call (ex. 8x15 or 8x5 sized sprites).
* (10) Fonts representing the keyboard input need to be stored as sprites somewhere in the interpreters memory space.
* (11) Special purpose registers for delay and sound, DT and ST are needed. These will decrement when non-zero at a rate of 60 Hz and stop.
* (12) The buzzer will sound when the ST register is non-zero, will stop when zero.
* (13) All of the 36 instructions present on the official Chip-8 need to be implemented.

[1]: devernay.free.fr/hacks/chip8/C8TECH10.HTM "Cowgod's Chip-8 Technical Refernece"
