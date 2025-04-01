#include <iostream>

#include "../ppu/ppu.hpp"

#include <string>
#include <filesystem>

extern bool runMinishell;

SDLRenderer::SDLRenderer() : window(nullptr), renderer(nullptr), texture(nullptr) {
    this->gameboy = Gameboy::getInstance();
}

SDLRenderer::~SDLRenderer() {
    this->cleanup();
}

bool SDLRenderer::initialize() {
    if(!ENABLE_RENDERING) return true;
    else {
        // Inti sdl
        if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
            std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
            return false;
        }

        // Create window
        int flags = SDL_WINDOW_BORDERLESS | SDL_WINDOW_HIDDEN;
        this->window = SDL_CreateWindow("GameBoy", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH * SCREEN_SCALE, SCREEN_HEIGHT * SCREEN_SCALE, flags);
        if(!this->window) {
            std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
            
            cleanup();
            return false;
        }

        // Create renderer
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

        // Pre render empty
        SDL_RenderClear(this->renderer);
        SDL_RenderPresent(this->renderer);

        return true;
    }
}

void SDLRenderer::render(const FrameBuffer &framebuffer) {
    if(!ENABLE_RENDERING) return;
    else {
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

        // Show window
        SDL_ShowWindow(this->window);
        
        // Update screen
        SDL_RenderPresent(this->renderer);

        // Handle SDL events
        this->handleEvents();
    }
}

void SDLRenderer::handleEvents() {
    SDL_Event e;
    // up -> up arrow on qwerty
    // down -> down arrow on qwerty
    // left -> left arrow on qwerty
    // right -> right arrow on qwerty

    // a -> z on qwerty
    // b -> x on qwerty
    // start -> enter on qwerty
    // select -> s on qwerty
    while(SDL_PollEvent(&e)) {
        // Joypad input handling
        uint8_t& joypadRegister = (uint8_t&)this->gameboy->memory->fetch8(0xFF00);

        // Key down events
        if (e.type == SDL_KEYDOWN) {
            // if (e.type = SDL_QUIT){
            //     this->gameboy->stop();
            // }
            switch (e.key.keysym.sym) {
                //esc key
                case SDLK_ESCAPE:
                    runMinishell = false;
                    this->gameboy->stop();
                    break;
                //space bar
                case SDLK_SPACE:
                    this->gameboy->stop();
                    break;
                // Direction keys
                case SDLK_DOWN:    // DOWN
                    if (!(joypadRegister & 0x10)) joypadRegister &= ~0x08; 
                    break;
                case SDLK_UP:  // UP
                    if (!(joypadRegister & 0x10)) joypadRegister &= ~0x04; 
                    break;
                case SDLK_LEFT:  // Left
                    if (!(joypadRegister & 0x10)) joypadRegister &= ~0x02; 
                    break;
                case SDLK_RIGHT: // Right
                    if (!(joypadRegister & 0x10)) joypadRegister &= ~0x01; 
                    break;

                // Action keys
                case SDLK_RETURN: // Start button
                    if (!(joypadRegister & 0x20)) joypadRegister &= ~0x08; 
                    break;
                case SDLK_s:     // Select button
                    if (!(joypadRegister & 0x20)) joypadRegister &= ~0x04; 
                    break;
                case SDLK_x:     // B button
                    if (!(joypadRegister & 0x20)) joypadRegister &= ~0x02; 
                    break;    
                case SDLK_z:     // A button
                    if (!(joypadRegister & 0x20)) joypadRegister &= ~0x01; 
                    break;



            }
        }
        
        // Key up events
        if (e.type == SDL_KEYUP) {
            switch (e.key.keysym.sym) {
                // Direction keys
                case SDLK_DOWN:
                    if (!(joypadRegister & 0x10)) joypadRegister |= 0x08;
                    break;
                case SDLK_UP:
                    if (!(joypadRegister & 0x10)) joypadRegister |= 0x04;
                    break;
                case SDLK_LEFT:
                    if (!(joypadRegister & 0x10)) joypadRegister |= 0x02;
                    break;
                case SDLK_RIGHT:
                    if (!(joypadRegister & 0x10)) joypadRegister |= 0x01;
                    break;

                // Action keys
                case SDLK_RETURN: // Start button
                    if (!(joypadRegister & 0x20)) joypadRegister |= 0x08;
                    break;
                case SDLK_s:     // Select button
                    if (!(joypadRegister & 0x20)) joypadRegister |= 0x04;
                    break;
                case SDLK_x:     // B button
                    if (!(joypadRegister & 0x20)) joypadRegister |= 0x02;
                    break;
                case SDLK_z:     // A button
                if (!(joypadRegister & 0x20)) joypadRegister |= 0x01;
                break;
            }
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