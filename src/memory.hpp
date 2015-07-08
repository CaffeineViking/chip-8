#ifndef C8_MEMORY_HPP
#define C8_MEMORY_HPP

#include <stdexcept>
#include "definitions.hpp"

namespace c8 {
    class Memory {
    public:
        // Interpreter data needs to be written in the Memory constructor.
        bool valid(addr) const noexcept; // Checks if user program is operating on a valid address.
        byte read(addr) const; // Reads byte at certain address. Throws exception if invalid.
        void write(addr, byte); // Writes byte to a certain address. Throws exception if invalid.

    private:
        static constexpr std::size_t SIZE {0x1000}; // 4096 bytes of memory.
        enum class Limit : addr {
            INTERPRETER = 0x200, // Memory locations below 0x200 are reserved for the interpreter.
            PROGRAM = SIZE // The rest of the memory is for the program, but only up to addresss 0x0FFF.
        };

        static bool within(addr, addr, addr) noexcept; // Checks if an address is between a piece of memory.
        byte contents[SIZE]; // Actual contents, need to do bounds checking since we are dealing with
                             // a emulator here, would be bad to access memory that is out of bounds.
    };
}

#endif
