#include "processor.hpp"
#include <stdexcept>

namespace ch8 {
    bool Processor::execute(Instruction inst, byte inst_upper, byte inst_lower) {
        return false;
    }

    void Processor::tick(Memory& memory) {
        // Since every instruction is 2 bytes long, we need to fetch
        // the upper and lower part of the instrution. Also, this is assuming
        // PC is currently aligned to an even address, if not, shit happens.
        // if (PC % 2 != 0) throw std::out_of_range {"Unaligned address."};

        // Fetch 2 byte instruction at PC.
        byte instruction_upper {memory.read(PC)};
        byte instruction_lower {memory.read(++PC)};

        // Parse instruction and execute it, getting constants from body.
        Instruction instruction {Interpreter::parse(instruction_upper, instruction_lower)};
        bool success {execute(instruction, instruction_upper, instruction_lower)};
        if (!success) throw std::runtime_error {"Couldn't execute instruction"};
        ++PC; // Prepare for next instruction.
    }

    word Processor::register_state(Register reg) const {
        switch (reg) {
        case Register::V0: case Register::V1: case Register::V2:
        case Register::V3: case Register::V4: case Register::V5:
        case Register::V6: case Register::V7: case Register::V8:
        case Register::V9: case Register::VA: case Register::VB:
        case Register::VC: case Register::VD: case Register::VE:
        case Register::VF: return V[static_cast<byte>(reg)]; break;
        case Register::ST: return ST; break;
        case Register::DT: return DT; break;
        case Register::PC: return PC; break;
        case Register::I: return I; break;
        case Register::SP: return SP; break;
        }
    }
}
