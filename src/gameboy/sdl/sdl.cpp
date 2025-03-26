#include <iostream>

#include "../ppu/ppu.hpp"

#include <string>
#include <filesystem>

SDLRenderer::SDLRenderer() : window(nullptr), renderer(nullptr), texture(nullptr) {
    this->gameboy = Gameboy::getInstance();
}

SDLRenderer::~SDLRenderer() {
    this->cleanup();
}

bool SDLRenderer::initialize() {
    // Inti sdl
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // Create window
    int flags = SDL_WINDOW_SHOWN | SDL_WINDOW_BORDERLESS;
    this->window = SDL_CreateWindow("GameBoy", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH * SCREEN_SCALE, SCREEN_HEIGHT * SCREEN_SCALE, flags);
    if(!this->window) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        
        cleanup();
        return false;
    }

    if(!DISPLAY_WINDOW) SDL_MinimizeWindow(this->window );

    // renderer
    this->renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(!this->renderer) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;

        cleanup();
        return false;
    }

    // Rendering scale quality
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");  // Use nearest neighbor scaling for pixelated look

    // texture that will be used to update the screen
    this->texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
    if(!this->texture) {
        std::cerr << "Texture could not be created! SDL_Error: " << SDL_GetError() << std::endl;

        cleanup();
        return false;
    }

    return true;
}

void SDLRenderer::render(const FrameBuffer &framebuffer) {
    // Temporary buffer
    uint32_t pixelData[SCREEN_WIDTH * SCREEN_HEIGHT];
    
    // Convert 2-bit color indices 
    for(uint8_t y = 0; y < SCREEN_HEIGHT; y++) {
        for(uint8_t x = 0; x < SCREEN_WIDTH; x++) {
            uint8_t* color = this->palette[framebuffer[y][x]];
            pixelData[y * SCREEN_WIDTH + x] = (color[0] << 16) | (color[1] << 8) | color[2];
        }
    }

    // Update texture 
    SDL_UpdateTexture(this->texture, NULL, pixelData, SCREEN_WIDTH * sizeof(uint32_t));
    
    // Clear screen
    SDL_RenderClear(this->renderer);
    
    // Render texture
    SDL_RenderCopy(this->renderer, this->texture, NULL, NULL);
    
    // Update screen
    SDL_RenderPresent(this->renderer);

    // Handle SDL events
    this->handleEvents();
}

void SDLRenderer::handleEvents() {
    SDL_Event e;

    while(SDL_PollEvent(&e)) {
        if(e.type == SDL_QUIT) {
            this->gameboy->stop();
        }

        // Check ESC key
        else if(e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
            this->gameboy->stop();
        }
    }
}

void SDLRenderer::cleanup() {
    if (texture) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }
    
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    
    SDL_Quit();
}