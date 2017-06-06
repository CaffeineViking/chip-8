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
    std::ifstream program_stream { path, std::ios::binary };
    if (program_stream) {
        program_stream.seekg(0, std::ios::end);
        program_size = program_stream.tellg();
        program_stream.seekg(0, std::ios::beg);
        program = new char[program_size];
        program_stream.read(program, program_size);
    } program_stream.close();

    std::cout << path << " loaded, occupying " << program_size << " bytes." << std::endl;
    ch8::Memory memory { reinterpret_cast<ch8::byte*>(program), program_size };
    delete[] program; // Program already copied.
    return memory; // Should use RVO semantics.
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0]
            << " <rom path>" << std::endl;
        return 1;
    }

    ch8::Memory memory { load(argv[1]) }; // Loads specified ROM with program.
    ch8::Processor processor { memory }; // Processor needs to know about memory.

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init failed: "
            << SDL_GetError() << std::endl;
        return 1;
    }

    std::string title { "chip-8 @ " + std::string{ argv[1] } };
    SDL_Window* window { SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 320, 0) };
    if (window == nullptr) {
        std::cerr << "SDL_CreateWindow failed: "
            << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, 0); // Nearest-neighbor filtering for textures (when scaled up).
    SDL_Renderer* renderer { SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC) };
    if (renderer == nullptr) {
        std::cerr << "SDL_CreateRenderer failed: "
            << SDL_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    int display_buffer_width = -1; // The size of a row in the texture below (queried by SDL_LockTexture).
    ch8::byte* display_buffer = nullptr; // We will load the buffer of the texture that is created in SDL_CreateTexture below, here.
    SDL_Texture* display_buffer_texture { SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, 64, 32) };
    if (display_buffer_texture == nullptr) {
        std::cerr << "SDL_CreateTexture failed.: "
                  << SDL_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    bool force_exit { false };
    while (processor.running() && !force_exit) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_KEYUP:
                switch (event.key.keysym.sym) {
                case SDLK_1: processor.key_released(0x01); break;
                case SDLK_2: processor.key_released(0x02); break;
                case SDLK_3: processor.key_released(0x03); break;
                case SDLK_4: processor.key_released(0x0C); break;

                case SDLK_q: processor.key_released(0x04); break;
                case SDLK_w: processor.key_released(0x05); break;
                case SDLK_e: processor.key_released(0x06); break;
                case SDLK_r: processor.key_released(0x0D); break;

                case SDLK_a: processor.key_released(0x07); break;
                case SDLK_s: processor.key_released(0x08); break;
                case SDLK_d: processor.key_released(0x09); break;
                case SDLK_f: processor.key_released(0x0E); break;

                case SDLK_z: processor.key_released(0x0A); break;
                case SDLK_x: processor.key_released(0x00); break;
                case SDLK_c: processor.key_released(0x0B); break;
                case SDLK_v: processor.key_released(0x0F); break;
                } break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                case SDLK_1: processor.key_pressed(0x01); break;
                case SDLK_2: processor.key_pressed(0x02); break;
                case SDLK_3: processor.key_pressed(0x03); break;
                case SDLK_4: processor.key_pressed(0x0C); break;

                case SDLK_q: processor.key_pressed(0x04); break;
                case SDLK_w: processor.key_pressed(0x05); break;
                case SDLK_e: processor.key_pressed(0x06); break;
                case SDLK_r: processor.key_pressed(0x0D); break;

                case SDLK_a: processor.key_pressed(0x07); break;
                case SDLK_s: processor.key_pressed(0x08); break;
                case SDLK_d: processor.key_pressed(0x09); break;
                case SDLK_f: processor.key_pressed(0x0E); break;

                case SDLK_z: processor.key_pressed(0x0A); break;
                case SDLK_x: processor.key_pressed(0x00); break;
                case SDLK_c: processor.key_pressed(0x0B); break;
                case SDLK_v: processor.key_pressed(0x0F); break;
                case SDLK_ESCAPE: force_exit = true;  break;
                case SDLK_j: processor.step(); break;
                case SDLK_k: processor.dump(); break;
                case SDLK_l:
                    while (processor.delay_issued()) processor.tick_delay();
                    while (processor.sound_issued()) processor.tick_sound();
                    break;
                } break;
            default: break;
            }
        }

        if (processor.display_updated()) {
            SDL_LockTexture(display_buffer_texture, nullptr,
                            reinterpret_cast<void**>(&display_buffer),
                            &display_buffer_width);
            for (std::size_t i { 0 }; i < 64 * 32; ++i) {
                // Only vary the game screen between black and white for now...
                display_buffer[i*3 + 0] = processor.display_buffer()[i] * 0xFF;
                display_buffer[i*3 + 1] = processor.display_buffer()[i] * 0xFF;
                display_buffer[i*3 + 2] = processor.display_buffer()[i] * 0xFF;
            } SDL_UnlockTexture(display_buffer_texture);

            SDL_RenderClear(renderer);
            // Render the display buffer texture with nearest neighbor scaling.
            SDL_RenderCopy(renderer, display_buffer_texture, nullptr, nullptr);
            SDL_RenderPresent(renderer);
            processor.updated_display();
        }
    }

    // As always, don't forget to free stuff :)
    SDL_DestroyTexture(display_buffer_texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
