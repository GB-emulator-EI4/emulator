#pragma once

#include <array>
#include <cstdint>

#include "SDL2/SDL.h"

#include "../ppu/ppu.hpp"

#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 144

typedef array<array<uint8_t, SCREEN_WIDTH>, SCREEN_HEIGHT> FrameBuffer; // Define a type for the framebuffer

class SDLRenderer {
public:

    SDLRenderer(int scale = 2);
    ~SDLRenderer();

   //init sdl adn create window
    bool initialize();

    // render framebuffer (from ppu)
    void render(const FrameBuffer framebuffer);

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