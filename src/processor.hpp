#ifndef C8_PROCESSOR_HPP
#define C8_PROCESSOR_HPP

#include "definitions.hpp"

namespace c8 {
    class Processor {
    public:
    private:
        static constexpr std::size_t STACK_SIZE {16};
        enum class Register {
            V0 = 0, V1, V2, V3, V4,
            V5, V6, V7, V8, V9,
            VA, VB, VC, VD, VE, VF,
            ST, DT, PC, I, SP
        };

        byte V[16]; // General purpose registers (8-bits), V0 - VF.
        byte ST {0}, DT {0}; // Special purpose registers, sound and delay timers.
        addr PC {0x200}, I; // Program Counter and address storage register I.
        byte SP {0x00}; // Stack pointer, only need 16 places (officially) so 8-bits suffice.
        word stack[STACK_SIZE]; // The 16-bit sized stack places, usually contains return addresses.
    };
}

#endif
