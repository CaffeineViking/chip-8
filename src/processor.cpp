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
        case Instruction::JP_A: inst_jpa(addr); break;
        case Instruction::CALL_A: inst_calla(addr); break;
        case Instruction::SE_RC: inst_serc(x, constant); break;
        case Instruction::SNE_RC: inst_snerc(x, constant); break;
        case Instruction::SE_RR: inst_serr(x, y); break;
        case Instruction::LD_RC: inst_ldrc(x, constant); break;
        case Instruction::ADD_RC: inst_addrc(x, constant); break;
        case Instruction::LD_RR: inst_ldrr(x, y); break;
        case Instruction::OR_RR: inst_orrr(x, y); break;
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
        if (!jump_inst(instruction)) ++PC; // Prepare for next instruction.
    }

    word Processor::register_state(Register reg) const {
        switch (reg) {
        case Register::V0: case Register::V1: case Register::V2:
        case Register::V3: case Register::V4: case Register::V5:
        case Register::V6: case Register::V7: case Register::V8:
        case Register::V9: case Register::VA: case Register::VB:
        case Register::VC: case Register::VD: case Register::VE:
        case Register::VF: return V[static_cast<byte>(reg)];
        case Register::ST: return ST;
        case Register::DT: return DT;
        case Register::PC: return PC;
        case Register::I: return I;
        case Register::SP: return SP;
        default: return 0x0000;
        }
    }

    bool Processor::jump_inst(Instruction inst) {
        switch (inst) {
            case Instruction::JP_A: case Instruction::JP_V0A:
            case Instruction::CALL_A: case Instruction::RET: return true;
            default: return false;
        }
    }

    void Processor::inst_cls() {
        for (std::size_t i {0}; i < WIDTH * HEIGHT; ++i) {
            screen_buffer[i] = 0x00; // Clear pixels.
        }
    }

    void Processor::inst_ret() {
        if (SP == 0) PC = stack[0x00];
        else PC = stack[SP--];
    }

    void Processor::inst_jpa(addr address) { PC = address; }
    void Processor::inst_calla(addr address) {
        stack[++SP] = PC;
        PC = address;
    }

    void Processor::inst_serc(byte reg, byte constant) { if (V[reg] == constant) PC += 2; }
    void Processor::inst_snerc(byte reg, byte constant) { if (V[reg] != constant) PC += 2; }
    void Processor::inst_serr(byte regx, byte regy) { if (V[regx] == V[regy]) PC += 2; }

    void Processor::inst_ldrc(byte reg, byte constant) { V[reg] = constant; }
    void Processor::inst_addrc(byte reg, byte constant) { V[reg] += constant; }
    void Processor::inst_ldrr(byte regx, byte regy) { V[regx] = V[regy]; }
    void Processor::inst_orrr(byte regx, byte regy) { V[regx] |= V[regy]; }
}
