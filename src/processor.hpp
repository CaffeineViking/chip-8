#ifndef CH8_PROCESSOR_HPP
#define CH8_PROCESSOR_HPP

#include <random>
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

        // Probably won't change,  but it's nice anyway.
        static constexpr std::size_t SCREEN_WIDTH  {64};
        static constexpr std::size_t SCREEN_HEIGHT {32};

        Processor(Memory&); // Always needs main memory.
        bool running() const { return still_running; } // Is the program still running?
        void execute(Instruction, byte, byte); // Executes the instruction with arguments given.
        bool display_updated() const { return screen_buffer_updated; }
        void updated_display() { screen_buffer_updated = false; }
        void step(); // Steps the processor state forward.

        // Outputs to emulated IO.
        const byte* display_buffer() const { return screen_buffer; } // Needs to be drawn for real later.
        bool sound_issued() const { return ST != 0; } // Upper abstraction needs to sound beep.
        bool delay_issued() const { return DT != 0; } // Both timers need to be counted down.

        // Inputs from emulated IO.
        void key_pressed(byte k) { key_states[k] = true; } // Key has been pressed.
        void key_released(byte k) { key_states[k] = false; } // Key has been released.
        void tick_sound() { --ST; } // Issued every 60 Hz whenever sound state is non-zero.
        void tick_delay() { --DT; } // Issued every 60 Hz whenever delay state is non-zero.

        word register_state(Register) const; // Returns the state of a register (dump), for debugging.
        void dump() const; // Dumps entire state to stdout.

    private:
        bool still_running {true};
        Memory& memory; // Reference to main memory.
        static bool jump_inst(Instruction); // Checks if this is a jump instruction.
        byte V[16] = {0}; // General purpose registers (8-bits), V0 - VF.
        byte ST {0}, DT {0}; // Special purpose registers, sound and delay timers.

        static constexpr addr PROGRAM_INIT {0x200};
        addr PC {PROGRAM_INIT}, I {0x0000}; // Program Counter and address storage register I.
        byte SP {0x00}; // Stack pointer, only need 16 places (officially) so 8-bits suffice.

        static constexpr std::size_t STACK_SIZE {16 + 1}; // + 1 since first is never used.
        word stack[STACK_SIZE] = {0}; // The 16-bit sized stack places, usually contains return addresses.

        std::mt19937 random_generator; // Will be given a random seed from device.
        std::uniform_int_distribution<byte> random_udistribution {0, 255}; // Should output uniformally.

        static constexpr byte KEYS {16}; // The Chip-8 has officially 16 keys.
        bool key_states[KEYS] = {0}; // A key is either pressed or not.

        bool screen_buffer_updated { true }; // If we need to draw the screen on the emulator or not.
        // The 64x32 sized screen needs to store its state. Instructions // affecting the screen modify
        // this, higher implementation will use // this. A zero represents no color, a one is color :).
        byte screen_buffer[SCREEN_WIDTH * SCREEN_HEIGHT] = { 0 }; // Buffer for the emulator for later.

        // Shitload of instructions below.
        void inst_cls(); // Clear screen.
        void inst_ret(); // Returns to the address pointed by SP.
        void inst_jpa(addr); // Jumps to the target address.
        void inst_calla(addr); // Jumps to the target address and sets the return address.

        void inst_serc(byte, byte); // Skips next instruction if register is equal to constant.
        void inst_snerc(byte, byte); // Skips next instruction if register is NOT equal to constant.
        void inst_serr(byte, byte); // Skips next instruction if both registers are equal.
        void inst_ldrc(byte, byte); // Loads constant to register.
        void inst_addrc(byte, byte); // Adds constant to register.

        void inst_ldrr(byte, byte); // Loads value of a certain register to another.
        void inst_orrr(byte, byte); // Performs a bitwise OR operation on both registers.
        void inst_andrr(byte, byte); // Performs a bitwise AND operation on both registers.
        void inst_xorrr(byte, byte); // Performs a bitwise XOR operation on both registers.
        void inst_addrr(byte, byte); // Adds register content to another register.
        void inst_subrr(byte, byte); // Subtracts register from another register.
        void inst_shrrr(byte, byte); // Shifts first register right by one digit.
        void inst_subnrr(byte, byte); // Subtracts register from another register, inverse.
        void inst_shlrr(byte, byte); // Shifts first register left by one digit.

        void inst_snerr(byte, byte); // Skips next instruction if NOT equal.
        void inst_ldia(addr); // Loads a certain constant address to I register.
        void inst_jpv0a(addr); // Jumps to a certain address offset by register V0.
        void inst_rndrc(byte, byte); // Assign random value to register, limited to constant.
        void inst_drwrrc(byte, byte, byte); // Draw sprite from memory, at register location.

        void inst_skpr(byte); // Skips next instruction if key is pressed.
        void inst_sknpr(byte); // Do NOT skip next instruction if key is pressed.
        void inst_ldrd(byte); // Loads value of register to delay timer.
        void inst_ldrk(byte); // Load the identifier of the pressed key.
        void inst_lddr(byte); // Loads the value of register into delay timer.
        void inst_ldsr(byte); // Loads the value of register into sound timer.
        void inst_addir(byte); // Adds value of register to I address register.

        void inst_ldfr(byte); // Loads address of font stored in register to I.
        void inst_ldbr(byte); // Store BCD representation of number in memory starting at I.
        void inst_ldiar(byte); // Store registers V0 - VX to memory starting at address I.
        void inst_ldrai(byte); // Load registers V0 - Vx with values stored in I.
    };
}

#endif
