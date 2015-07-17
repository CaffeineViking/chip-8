#include <iostream>
#include <iomanip>

#include "memory.hpp"
#include "processor.hpp"
#include "definitions.hpp"

int main() {
    // Simple for loop (equivalent) where
    // V1 is incremented until reaching V0.
    const ch8::byte program[] =
    {0x60, 0x0A, 0x61, 0x00, // LD V0, 10; LD V1, 0
     0x71, 0x01, 0x50, 0x10, // ADD V1, 1; SE V0, V1
     0x12, 0x04}; // JP 0x204

    ch8::Memory memory {program, sizeof(program)};
    ch8::Processor processor;

    char input;
    do {
        processor.dump();
        ch8::addr program_counter {processor.register_state(ch8::Processor::Register::PC)};
        std::cout << "Next instruction: 0x" << std::setw(2) << std::setfill('0') << std::hex << static_cast<short>(memory.read(program_counter))
                  << std::setw(2) << std::setfill('0') << std::hex << static_cast<short>(memory.read(program_counter + 1)) << std::endl;

        processor.step(memory);
        input = std::getchar();
    } while(input != 'q' && processor.running());

    return 0;
}
