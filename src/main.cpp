#include <iostream>
#include <fstream>
#include <string>
#include <SDL.h>

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

    if(SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init failed: "
            << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window {SDL_CreateWindow("chip-8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 320, 0)};
    if (window == nullptr) {
        std::cerr << "SDL_CreateWindow failed: "
            << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer {SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)};
    if (renderer == nullptr) {
        std::cerr << "SDL_CreateRenderer failed: "
            << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    bool force_exit {false};
    while (processor.running() && !force_exit) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_KEYDOWN: force_exit = true; break;
            default: break;
            }
        }

        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
