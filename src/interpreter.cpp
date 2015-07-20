#include "interpreter.hpp"

namespace ch8 {
    Instruction Interpreter::parse(byte inst_upper, byte inst_lower) {
        byte upper_identifier = inst_upper >> 4; // First nibble never has mangled arguments.
        byte lower_identifier = inst_lower & 0x0F; // Last nibble identifies variants of inst.

        if (inst_upper == 0x00 && inst_lower == 0x00) return Instruction::EXIT;
        else if (inst_upper == 0x00 && inst_lower == 0xE0) return Instruction::CLS;
        else if (inst_upper == 0x00 && inst_lower == 0xEE) return Instruction::RET;
        else if (upper_identifier == 0x00) return Instruction::SYS_A;
        else if (upper_identifier == 0x01) return Instruction::JP_A;
        else if (upper_identifier == 0x02) return Instruction::CALL_A;
        else if (upper_identifier == 0x03) return Instruction::SE_RC;
        else if (upper_identifier == 0x04) return Instruction::SNE_RC;
        else if (upper_identifier == 0x05 && lower_identifier == 0x00) return Instruction::SE_RR;
        else if (upper_identifier == 0x06) return Instruction::LD_RC;
        else if (upper_identifier == 0x07) return Instruction::ADD_RC;
        else if (upper_identifier == 0x08 && lower_identifier == 0x00) return Instruction::LD_RR;
        else if (upper_identifier == 0x08 && lower_identifier == 0x01) return Instruction::OR_RR;
        else if (upper_identifier == 0x08 && lower_identifier == 0x02) return Instruction::AND_RR;
        else if (upper_identifier == 0x08 && lower_identifier == 0x03) return Instruction::XOR_RR;
        else if (upper_identifier == 0x08 && lower_identifier == 0x04) return Instruction::ADD_RR;
        else if (upper_identifier == 0x08 && lower_identifier == 0x05) return Instruction::SUB_RR;
        else if (upper_identifier == 0x08 && lower_identifier == 0x06) return Instruction::SHR_RR;
        else if (upper_identifier == 0x08 && lower_identifier == 0x07) return Instruction::SUBN_RR;
        else if (upper_identifier == 0x08 && lower_identifier == 0x0E) return Instruction::SHL_RR;
        else if (upper_identifier == 0x09 && lower_identifier == 0x00) return Instruction::SNE_RR;
        else if (upper_identifier == 0x0A) return Instruction::LD_IA;
        else if (upper_identifier == 0x0B) return Instruction::JP_V0A;
        else if (upper_identifier == 0x0C) return Instruction::RND_RC;
        else if (upper_identifier == 0x0D) return Instruction::DRW_RRC;
        else if (upper_identifier == 0x0E && inst_lower == 0x9E) return Instruction::SKP_R;
        else if (upper_identifier == 0x0E && inst_lower == 0xA1) return Instruction::SKNP_R;
        else if (upper_identifier == 0x0F && inst_lower == 0x07) return Instruction::LD_RD;
        else if (upper_identifier == 0x0F && inst_lower == 0x0A) return Instruction::LD_RK;
        else if (upper_identifier == 0x0F && inst_lower == 0x15) return Instruction::LD_DR;
        else if (upper_identifier == 0x0F && inst_lower == 0x18) return Instruction::LD_SR;
        else if (upper_identifier == 0x0F && inst_lower == 0x1E) return Instruction::ADD_IR;
        else if (upper_identifier == 0x0F && inst_lower == 0x29) return Instruction::LD_FR;
        else if (upper_identifier == 0x0F && inst_lower == 0x33) return Instruction::LD_BR;
        else if (upper_identifier == 0x0F && inst_lower == 0x55) return Instruction::LD_IAR;
        else if (upper_identifier == 0x0F && inst_lower == 0x65) return Instruction::LD_RAI;
        else throw std::runtime_error {"Couldn't parse instruction."};
    }

    word Interpreter::parse_argument(Argument arg, byte inst_upper, byte inst_lower) {
        switch (arg) {
        case Argument::X: return inst_upper & 0x0F; break;
        case Argument::Y: return inst_lower >> 4; break;
        case Argument::ADDRESS: return (static_cast<word>(inst_upper & 0x0F) << 8) + inst_lower; break;
        case Argument::CONSTANT: return inst_lower; break;
        }

        return 0xC0DE;
    }
}
