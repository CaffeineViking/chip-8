#include "processor.hpp"
#include <stdexcept>

namespace ch8 {
    void Processor::tick(Memory& memory) {
        // Since every instruction is 2 bytes long, we need to fetch
        // the upper and lower part of the instrution. Also, this is assuming
        // PC is currently aligned to an even address, if not, shit happens.
        // if (PC % 2 != 0) throw std::out_of_range {"Unaligned address."};

        // Fetch 2 byte instruction at PC.
        byte instruction_upper {memory.read(PC)};
        byte instruction_lower {memory.read(++PC)};

        // Parse instruction and execute it, getting constants from body.
        Instruction instruction {parse(instruction_upper, instruction_lower)};
        bool success {execute(instruction, instruction_upper, instruction_lower)};
        if (!success) throw std::runtime_error {"Instruction failed."};
        ++PC; // Prepare for next instruction.
    }

    Processor::Instruction Processor::parse(byte inst_upper, byte inst_lower) const {
        return Instruction::NOP;
    }

    bool Processor::execute(Instruction inst, byte inst_upper, byte inst_lower) {
        return false;
    }
}
