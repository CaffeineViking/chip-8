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
        LD_BR, LD_IAR, LD_RAI, EXIT
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
        static const byte* retrieve_font(); // Retrieves font for keyboard. A total of 8x5x16 bytes.

        static constexpr std::size_t FONTS {16};
        static constexpr std::size_t FONT_HEIGHT {5};
        static constexpr std::size_t FONT_SIZE {FONT_HEIGHT * FONTS};

    private:
        static constexpr byte font[FONT_SIZE] = {
            0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
            0x20, 0x60, 0x20, 0x20, 0x70, // 1
            0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
            0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
            0x90, 0x90, 0xF0, 0x10, 0x10, // 4
            0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
            0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
            0xF0, 0x10, 0x20, 0x40, 0x40, // 7
            0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
            0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
            0xF0, 0x90, 0xF0, 0x90, 0x90, // A
            0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
            0xF0, 0x80, 0x80, 0x80, 0xF0, // C
            0xE0, 0x90, 0x90, 0x90, 0xE0, // D
            0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
            0xF0, 0x80, 0xF0, 0x80, 0x80  // F
        };
    };
}

#endif
