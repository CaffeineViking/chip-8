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

        void execute(Instruction, byte, byte); // Executes the instruction with arguments given.
        void step(Memory&); // Steps the processor state forward.

    private:
        void inst_cls(); // Clear screen.
        void inst_ret(); // Returns to the address pointed by SP.
        void inst_jpa(addr); // Jumps to the target address.
        void inst_calla(addr); // Jumps to the target address and sets the return address.
        void inst_serc(byte, byte); // Skips next instruction if register is equal to constant.
        void inst_snerc(byte, byte); // Skips next instruction if register is NOT equal to constant.
        void inst_serr(byte, byte); // Skips next instruction if both registers are equal.
        void inst_ldrc(byte, byte); // Loads constant to register.
        void inst_addrc(byte, byte); // Addts constant to register.
        void inst_ldrr(byte, byte); // Loads value of a certain register to another.
        void inst_orrr(byte, byte); // Performs a bitwise OR operation on both registers.

        bool still_running {true};
        static bool jump_inst(Instruction); // Checks if this is a jump instruction.
        byte V[16] = {0}; // General purpose registers (8-bits), V0 - VF.
        byte ST {0}, DT {0}; // Special purpose registers, sound and delay timers.

        static constexpr addr PROGRAM_INIT {0x200};
        addr PC {PROGRAM_INIT}, I {0x0000}; // Program Counter and address storage register I.
        byte SP {0x00}; // Stack pointer, only need 16 places (officially) so 8-bits suffice.

        static constexpr std::size_t STACK_SIZE {16 + 1}; // + 1 since first is never used.
        word stack[STACK_SIZE] = {0}; // The 16-bit sized stack places, usually contains return addresses.

        static constexpr std::size_t WIDTH {64};
        static constexpr std::size_t HEIGHT {32};
        byte screen_buffer[WIDTH * HEIGHT];  // The 64x32 sized screen needs to store its state. Instructions
                                      // affecting the screen modify this, higher implementation will use this.
                                      // A byte of 0x00 represents no color, 0x01 represents color.
    };
}

#endif
