#ifndef CH8_PROCESSOR_HPP
#define CH8_PROCESSOR_HPP

#include <stdexcept>
#include "definitions.hpp"
#include "memory.hpp"
#include "interpreter.hpp"

namespace ch8 {
    class Processor {
    public:
        enum class Register {
            V0 = 0, V1, V2, V3, V4,
            V5, V6, V7, V8, V9,
            VA, VB, VC, VD, VE, VF,
            ST, DT, PC, I, SP
        };

        bool running() const { return still_running; } // Is the program still running?
        const byte* display_buffer() const { return screen_buffer; } // Needs to be drawn for real later.
        bool sound_issued() const { return ST != 0; } // Upper abstraction needs to sound beep.
        bool delay_issued() const { return DT != 0; } // Both timers need to be counted down.
        word register_state(Register) const; // Returns the state of a register (dump), for debugging.

        bool execute(Instruction, byte, byte); // Executes the instruction with arguments given.
        void tick(Memory&); // Steps the processor state forward.

    private:
        bool still_running {true};
        byte V[16]; // General purpose registers (8-bits), V0 - VF.
        byte ST {0}, DT {0}; // Special purpose registers, sound and delay timers.

        static constexpr addr PROGRAM_INIT {0x200};
        addr PC {PROGRAM_INIT}, I; // Program Counter and address storage register I.
        byte SP {0x00}; // Stack pointer, only need 16 places (officially) so 8-bits suffice.

        static constexpr std::size_t STACK_SIZE {16};
        word stack[STACK_SIZE]; // The 16-bit sized stack places, usually contains return addresses.
        byte screen_buffer[64 * 32];  // The 64x32 sized screen needs to store its state. Instructions
                                      // affecting the screen modify this, higher implementation will use this.
    };
}

#endif
