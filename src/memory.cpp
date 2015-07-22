#include "memory.hpp"
#include <cstring>

namespace ch8 {
    Memory::Memory(const byte* program, std::size_t program_size) {
        // Copy given external program to local main memory, placing it in the correct location.
        std::memcpy(contents + static_cast<addr>(Limit::INTERPRETER), program, program_size);

        // Copy the interpreters font to main memory, not modifying the original.
        std::memcpy(contents + 0x000, Interpreter::retrieve_font(), Interpreter::FONT_SIZE);
    }

    bool Memory::valid(addr address) const{
        addr program_begin {static_cast<addr>(Limit::INTERPRETER)};
        addr program_end {static_cast<addr>(Limit::PROGRAM) - 1};
        if (within(address, program_begin, program_end)) return true;
        else return false;
    }

    byte Memory::read(addr address) const {
        addr font_begin {0x000};
        addr font_end {static_cast<addr>(Limit::FONT) - 1};
        if (valid(address) || within(address, font_begin, font_end)) return contents[address];
        else throw std::out_of_range {"Couldn't read data, invalid memory address."};
    }

    void Memory::write(addr address, byte data) {
        if (valid(address)) contents[address] = data;
        else throw std::out_of_range {"Couldn't write data, invalid memory address."};
    }

    bool Memory::within(addr target, addr begin, addr end) {
        if (target <= end && target >= begin) return true;
        else return false;
    }
}
