#include <iostream>
#include <iomanip>

#include "memory.hpp"
#include "processor.hpp"
#include "definitions.hpp"

void print_instruction(const ch8::Memory& memory, ch8::addr program_counter) {
    std::cout << "Next instruction: 0x" << std::setw(2) << std::setfill('0') << std::hex << static_cast<ch8::addr>(memory.read(program_counter))
        << std::setw(2) << std::setfill('0') << std::hex << static_cast<ch8::addr>(memory.read(program_counter + 1)) << std::endl;
}

void print_display(const ch8::byte* buffer) {
    for (std::size_t y {0}; y < 32; ++y) {
        for (std::size_t x {0}; x < 64; ++x) {
            std::cout << static_cast<unsigned>(buffer[x + y * 64]);
        } std::cout << std::endl;
    }
}

int main() {
    // Simple for loop (equivalent) where
    // V1 is incremented until reaching V0.
    const ch8::byte program[] =
    {0x60, 0x0A, 0x61, 0x00, // LD V0, 10; LD V1, 0
     0x71, 0x01, 0x00, 0xE0, 0xA2, 0x12, 0xD1, 0x01, // ADD V1, 1; CLS; LD I, 0x20E; DRW V1, V0, 2
     0x50, 0x10, 0x12, 0x04, // SE V0, V1; JP 0x204
     0x00, 0x00, 0x18}; // 0b00010000

    ch8::Memory memory {program, sizeof(program)};
    ch8::Processor processor {memory};

    char input;
    do {
        processor.dump();
        ch8::addr program_counter {processor.register_state(ch8::Processor::Register::PC)};
        print_display(processor.display_buffer());
        print_instruction(memory, program_counter);

        processor.step();
        input = std::getchar();
    } while(input != 'q' && processor.running());

    return 0;
}
