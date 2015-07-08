#ifndef CH8_PROCESSOR_HPP
#define CH8_PROCESSOR_HPP

#include <stdexcept>
#include "definitions.hpp"
#include "memory.hpp"

namespace ch8 {
    class Processor {
    public:
        void tick(Memory&); // Steps the processor state forward.
        bool running() const { return still_running; } // Is the program still running?

    private:
        // Mostly used for readability.
        enum class Register {
            V0 = 0, V1, V2, V3, V4,
            V5, V6, V7, V8, V9,
            VA, VB, VC, VD, VE, VF,
            ST, DT, PC, I, SP
        };

        // The postfix notation is what arguments the
        // specific instruction takes. Example: A is for Address,
        // R for Register, C for Constant etc...
        enum class Instruction {
            SYS_A, CLS, RET, JP_A, CALL_A, SE_RC,
            SNE_RC, SE_RR, LD_RC, ADD_RC, LD_RR,
            OR_RR, AND_RR, XOR_RR, ADD_RR, SUB_RR,
            SHR_R, SHR_RR, SUBN_RR, SHL_R, SHL_RR,
            SNE_RR, LD_IA, JP_V0A, RND_RC, DRW_RRC,
            SKP_R, SKNP_R, LD_RD, LD_RK, LD_DR, LD_SR,
            ADD_IR, LD_FR, LD_BR, LD_IAR, LD_ARI, NOP
        };

        Instruction parse(byte, byte) const; // Determine what instruction we are dealing with.
        bool execute(Instruction, byte, byte); // Executes the instruction, get constants from upper/lower.

        bool still_running {true};
        byte V[16]; // General purpose registers (8-bits), V0 - VF.
        byte ST {0}, DT {0}; // Special purpose registers, sound and delay timers.
        addr PC {0x200}, I; // Program Counter and address storage register I.
        byte SP {0x00}; // Stack pointer, only need 16 places (officially) so 8-bits suffice.

        static constexpr std::size_t STACK_SIZE {16};
        word stack[STACK_SIZE]; // The 16-bit sized stack places, usually contains return addresses.
    };
}

#endif
