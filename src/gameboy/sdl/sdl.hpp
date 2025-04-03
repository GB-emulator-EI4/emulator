#pragma once

#include <array>
#include <cstdint>

#include "SDL2/SDL.h"

#include "../gameboy.hpp"

#include "../../constants/constants.hpp"

typedef array<array<uint8_t, SCREEN_WIDTH>, SCREEN_HEIGHT> FrameBuffer; // Define a type for the framebuffer

class SDLRenderer {
    public:
        SDLRenderer();
        ~SDLRenderer();

        // Initialize SDL
        bool initialize();

        // Render framebuffer (from ppu)
        void render(const FrameBuffer &framebuffer);

        // SDL events
        void handleEvents();

        // Clean up
        void cleanup();

        // Key states
        const Uint8* keyStates;

        // Getter
        const Uint8* getKeyStates() { 
            // Update key states
            SDL_PumpEvents();
            return this->keyStates; 
        }

    private:
        // Gameboy instance
        Gameboy* gameboy;
        
        // Window and renderer
        SDL_Window* window;
        SDL_Renderer* renderer;
        SDL_Texture* texture;

        // Color palette 
        // 0: white
        // 1: light
        // 2: dark
        // 3: black

        uint8_t palette[4][3] = {
            { 255, 255, 255 },  
            { 192, 192, 192 },  
            { 96, 96, 96 },     
            { 0, 0, 0 }         
        };
};