#include "sdl.hpp"
#include <iostream>

SDLRenderer::SDLRenderer(int scale)
    : window(nullptr), renderer(nullptr), texture(nullptr), scale(scale), initialized(false) {
}

SDLRenderer::~SDLRenderer() {
    cleanup();
}

bool SDLRenderer::initialize() {
    // inti sdl
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // window
    window = SDL_CreateWindow("GameBoy",SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,SCREEN_WIDTH * scale, SCREEN_HEIGHT * scale,SDL_WINDOW_SHOWN);

    if (!window) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        cleanup();
        return false;
    }

    // renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        cleanup();
        return false;
    }

    //rendering scale quality
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");  // Use nearest neighbor scaling for pixelated look

    // texture that will be used to update the screen
    texture = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_RGBA8888,SDL_TEXTUREACCESS_STREAMING,SCREEN_WIDTH, SCREEN_HEIGHT);

    if (!texture) {
        std::cerr << "Texture could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        cleanup();
        return false;
    }

    initialized = true;
    return true;
}

void SDLRenderer::render(const PPU& ppu) {
    if (!initialized) return;

    const auto& framebuffer = ppu.getFramebuffer();
    
    //temporary buffer
    uint32_t pixelData[SCREEN_WIDTH * SCREEN_HEIGHT];
    
    // Convert 2-bit color indices 
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            uint8_t colorIndex = framebuffer[y][x];
            SDL_Color color = palette[colorIndex];

            pixelData[y * SCREEN_WIDTH + x] = 
                (color.r << 24) | (color.g << 16) | (color.b << 8) | color.a;
        }
    }
    
    // update texture 
    SDL_UpdateTexture(texture, NULL, pixelData, SCREEN_WIDTH * sizeof(uint32_t));
    
    //clear screen
    SDL_RenderClear(renderer);
    
    //render texture
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    
    //update screen
    SDL_RenderPresent(renderer);
}

bool SDLRenderer::handleEvents() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            return false; 
        }
        // other events ? keyboard WASD ?
    }
    return true; 
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
    
    if (initialized) {
        SDL_Quit();
        initialized = false;
    }
}