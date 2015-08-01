#include <iostream>
#include <fstream>

#include "memory.hpp"
#include "processor.hpp"
#include "definitions.hpp"

// Load ROM file to memory.
ch8::Memory load(const char* path) {
    char* program;
    std::size_t program_size;
    std::ifstream program_stream {path, std::ios::binary};
    if (program_stream) {
        program_stream.seekg(0, std::ios::end);
        program_size = program_stream.tellg();
        program_stream.seekg(0, std::ios::beg);
        program = new char[program_size];
        program_stream.read(program, program_size);
    } program_stream.close();

    std::cout << path << " loaded, occupying " << program_size << " bytes." << std::endl;
    ch8::Memory memory {reinterpret_cast<ch8::byte*>(program), program_size};
    delete[] program; // Program already copied.
    return memory; // Should use RVO semantics.
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0]
                  << " <rom path>" << std::endl;
        return 1;
    }

    ch8::Memory memory {load(argv[1])}; // Loads specified ROM with program.
    ch8::Processor processor {memory}; // Processor needs to know about memory.
    return 0;
}
