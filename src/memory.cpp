#include "memory.hpp"
#include <cstring>

namespace ch8 {
    Memory::Memory(const byte* program, std::size_t program_size) {
        std::memcpy(contents + static_cast<addr>(Limit::INTERPRETER), program, program_size);
        // Interpreter data needs to be written below...
    }

    bool Memory::valid(addr address) const{
        addr program_begin {static_cast<addr>(Limit::INTERPRETER)};
        addr program_end {static_cast<addr>(Limit::PROGRAM) - 1};
        if (within(address, program_begin, program_end)) return true;
        else return false;
    }

    byte Memory::read(addr address) const {
        if (valid(address)) return contents[address];
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
