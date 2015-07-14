#ifndef CH8_INTERPRETER_HPP
#define CH8_INTERPRETER_HPP

#include <stdexcept>
#include "definitions.hpp"

namespace ch8 {
    // The postfix notation is what arguments the
    // specific instruction takes. Example: A is for Address,
    // R for Register, C for Constant etc...
    enum class Instruction {
        SYS_A, CLS, RET, JP_A, CALL_A, SE_RC,
        SNE_RC, SE_RR, LD_RC, ADD_RC, LD_RR,
        OR_RR, AND_RR, XOR_RR, ADD_RR, SUB_RR,
        SHR_RR, SUBN_RR, SHL_RR, SNE_RR, LD_IA,
        JP_V0A, RND_RC, DRW_RRC, SKP_R, SKNP_R,
        LD_RD, LD_RK, LD_DR, LD_SR, ADD_IR, LD_FR,
        LD_BR, LD_IAR, LD_RAI
    };

    // Possible argumemnts to retrieve from instructiion.
   enum class Argument {
       X, Y, ADDRESS,
       CONSTANT
   };

    class Interpreter {
    public:
        static Instruction parse(byte, byte); // Parses upper and lower byte of instruction.
        static word parse_argument(Argument, byte, byte); // Retrieves argument given to instruction.
    };
}

#endif
