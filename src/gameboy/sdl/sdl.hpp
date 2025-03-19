#pragma once

#include <array>
#include <cstdint>

#include "SDL.h"

#include "../ppu/ppu.hpp"

class SDLRenderer {
public:

    SDLRenderer(int scale = 3);
    ~SDLRenderer();

   //init sdl adn create window
    bool initialize();

    // render framebuffer (from ppu)
    void render(const PPU& ppu);

    // SDL events
    bool handleEvents();

    // clean up
    void cleanup();

private:
    // Window and renderer
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;

    // Scale factor for the window 
    int scale;

    //color palette 
    // Color 0: white
    // Color 1: light
    // Color 2: dark
    // Color 3: black
    SDL_Color palette[4] = {
        {224, 248, 208, 255},  
        {136, 192, 112, 255},  
        {52, 104, 86, 255},  
        {8, 24, 32, 255}      
    };

    // flag to track if initialization was successful
    bool initialized;
};