#include "processor.hpp"
#include <stdexcept>

namespace ch8 {
    void Processor::execute(Instruction inst, byte inst_upper, byte inst_lower) {
        byte x = Interpreter::parse_argument(Argument::X, inst_upper, inst_lower);
        byte y = Interpreter::parse_argument(Argument::Y, inst_upper, inst_lower);
        word addr = Interpreter::parse_argument(Argument::ADDRESS, inst_upper, inst_lower);
        byte constant = Interpreter::parse_argument(Argument::CONSTANT, inst_upper, inst_lower);

        switch (inst) {
        case Instruction::SYS_A: break;
        case Instruction::CLS: inst_cls(); break;
        case Instruction::RET: inst_ret(); break;
        default: throw std::runtime_error {"Couldn't execute instruction."};
        }
    }

    void Processor::step(Memory& memory) {
        // Since every instruction is 2 bytes long, we need to fetch
        // the upper and lower part of the instrution. Also, this is assuming
        // PC is currently aligned to an even address, if not, shit happens.
        // if (PC % 2 != 0) throw std::out_of_range {"Unaligned address."};

        // Fetch 2 byte instruction at PC.
        byte instruction_upper {memory.read(PC)};
        byte instruction_lower {memory.read(++PC)};

        // Parse instruction and execute it, getting constants from body.
        Instruction instruction {Interpreter::parse(instruction_upper, instruction_lower)};
        execute(instruction, instruction_upper, instruction_lower);
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
        default: return 0x0000;
        }
    }

    void Processor::inst_cls() {
        for (std::size_t i {0}; i < WIDTH * HEIGHT; ++i) {
            screen_buffer[i] = 0x00; // Clear pixel.
        }
    }

    void Processor::inst_ret() {
        if (SP == 0) PC = stack[0x00];
        else PC = stack[SP--];
    }
}
