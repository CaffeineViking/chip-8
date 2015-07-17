#include <iostream>
#include <iomanip>

#include "memory.hpp"
#include "processor.hpp"
#include "definitions.hpp"

int main() {
    const ch8::byte program[] =
    {0x60, 0x42, 0x61, 0x08, // LD V0, 0x42; LD V1, 0x08
     0x80, 0x11}; // OR V0, V1

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
