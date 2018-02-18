Chip-8 Interpreter
==============================

<p align="center">
    <img src="/share/screenshot.png" alt="Space Invaders in the Interpreter"/>
</p>

A fairly simple Chip-8 interpreter written in modern C++, it features all of the standard instructions and comes bundled with a nice built-in memory and program debugger. I've tested space invaders, tetris and a couple of other games, and it seems to run fine. I haven't implemented sound yet though, that is still on my to-do list. Need to work on timer "synchronization" too.

Compiling and Testing
---------------------

1. Start up your favorite terminal emulator.
2. Go to the root of this project.
3. Issue ```make``` and hope for the best.
4. You will find that ```bin``` has stuff.
5. Either start the program or test suite.

or: `makepkg -i` if you're on Arch Linux.

Usage and Documents
-------------------

- ```bin/chip-8.out <path-for-rom>```
- ```bin/chip-8.out share/INVADERS```
- **J**: start or step the built-in debugger.
- **K**: will resume normal execution.
- **1, 2, 3, 4**: maps 1, 2, 3, C on chip-8.
- **Q, W, E, R**: maps 4, 5, 6, D on chip-8.
- **A, S, D, F**: maps 7, 8, 9, E on chip-8.
- **Z, X, C, V**: maps A, 0, B, F on chip-8.
- **ESC**: terminates the interpreter.
- You'll find some design docs in ```docs```.

Dependencies
------------

- GCC 4.7
- SDL 2.0
- SDL_image 2.0

Reporting Bugs
--------------

There are definitely no known bugs in this software at this time.
