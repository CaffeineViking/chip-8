#include <iostream>
#include <iomanip>
#include <fstream>

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

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0]
                  << " <rom path>" << std::endl;
        return 1;
    }

    char* program;
    std::size_t program_size;
    std::ifstream program_stream {argv[1], std::ios::binary};
    if (program_stream) {
        program_stream.seekg(0, std::ios::end);
        program_size = program_stream.tellg();
        program_stream.seekg(0, std::ios::beg);
        program = new char[program_size];
        program_stream.read(program, program_size);
    } program_stream.close();

    std::cout << argv[1] << " loaded, occupying " << program_size << " bytes." << std::endl;
    ch8::Memory memory {reinterpret_cast<ch8::byte*>(program), program_size};
    ch8::Processor processor {memory};
    delete[] program; // Program already copied.

    char input;
    do {
        processor.dump();
        ch8::addr program_counter {processor.register_state(ch8::Processor::Register::PC)};
        print_instruction(memory, program_counter);

        if (input == 'd') print_display(processor.display_buffer());
        if (input == 't') { // Emulates 60 Hz timer interrupt.
            while (processor.delay_issued()) processor.tick_delay();
            while (processor.sound_issued()) processor.tick_sound();
        }

        processor.step();
        input = std::getchar();
    } while(input != 'q' && processor.running());

    return 0;
}
