#include <iostream>

using namespace std;

#include "../../constants/constants.hpp"

#include "ppu.hpp"

PPU::PPU(Gameboy* gameboy) : gameboy(gameboy), logger(Logger::getInstance()->getLogger("PPU")), currentLY(0), currentMode(Mode::OAMSearch) {
    // Initialize framebuffer
    for (auto& row : framebuffer) {
        row.fill(0);
    }
}

PPU::~PPU() {
    // Destructor
}

void PPU::step() {
    //handle mode transitions based on current T-cycle
    switch (currentMode) {
        case Mode::OAMSearch:
            if (this->gameboy->getTcycles() == 0) {
                // OAM Search mode
                checkSTATInterrupts(); 
            }
            
            if (this->gameboy->getTcycles() >= 80) {
                currentMode = Mode::Drawing;
            }
            break;
            
        case Mode::Drawing:
            if (this->gameboy->getTcycles() >= 252) {
                currentMode = Mode::HBlank;
                renderScanline(); 
                
                uint8_t& stat = (uint8_t&) this->gameboy->memory->fetch8(STAT);
                stat = (stat & 0xFC) | static_cast<uint8_t>(currentMode);
                
                checkSTATInterrupts();
            }
            break;
            
        case Mode::HBlank:
            // changes handled at T-cycle 456/0
            break;
            
        case Mode::VBlank:
            if(this->gameboy->getTcycles() == 0 && currentLY == 144) {
                //entered VBlank
                this->gameboy->cpu->triggerInterrupt(Interrupt::VBlank);
                checkSTATInterrupts();
            }
            break;
    }
    
    if (this->gameboy->getTcycles() == 455) {
        currentLY++;
        uint8_t& nLY = (uint8_t&) this->gameboy->memory->fetch8(LY);
        nLY = currentLY;
        
        checkLYCInterrupt();
        
        if (currentLY == 144) {
            // Prepare for VBlank
        } else if (currentLY > 153) {
            currentLY = 0;
            nLY = currentLY;
        }
    } else if (this->gameboy->getTcycles() == 0) {
        if (currentLY == 144) {
            currentMode = Mode::VBlank;

            this->gameboy->cpu->triggerInterrupt(Interrupt::VBlank);
        } else if (currentLY == 0 || currentLY < 144) {
            currentMode = Mode::OAMSearch;
            checkSTATInterrupts(); 
        }
    
        if(ENABLE_LOGGING) {
            // Copy framebuffer into another var
            FrameBuffer copyFramebuffer;

            for (int i = 0; i < SCREEN_HEIGHT; i++) {
                for (int j = 0; j < SCREEN_WIDTH; j++) {
                    copyFramebuffer[i][j] = framebuffer[i][j];
                }
            }

            // Add a colored pixel to where the LY is
            if(currentLY < SCREEN_HEIGHT) {
                for (int i = 0; i < SCREEN_WIDTH; i++) {
                    copyFramebuffer[currentLY][i] = 0xFF;
                }
            }

            // Render the framebuffer
            this->gameboy->renderer->render(copyFramebuffer);
        } else {
           if (currentLY == 0) {
                this->gameboy->renderer->render(framebuffer);
            }
        }
    }
    
    //update STAT register in mem
    uint8_t& stat = (uint8_t&) this->gameboy->memory->fetch8(STAT);
    stat = (stat & 0xFC) | static_cast<uint8_t>(currentMode);
}

void PPU::checkLYCInterrupt() {
    uint8_t& stat = (uint8_t&) this->gameboy->memory->fetch8(STAT);
    uint8_t& lyc = (uint8_t&) this->gameboy->memory->fetch8(LYC);

    if (lyc == currentLY){
        stat |= 0x04;
        if (stat & 0x40){
            this->gameboy->cpu->triggerInterrupt(Interrupt::LCD);
        }
    } else {
        stat &= 0xFB;
    }
    //update STAT register in mem
    uint8_t& nSTAT = (uint8_t&) this->gameboy->memory->fetch8(STAT);
    nSTAT = stat;
}


void PPU::checkSTATInterrupts() {
    uint8_t stat = this->gameboy->memory->fetch8(STAT);
    bool interruptTriggered = false;
    
    if ((stat & 0x08) && currentMode == Mode::HBlank) {
        interruptTriggered = true;
    }
    
    if ((stat & 0x10) && currentMode == Mode::VBlank) {
        interruptTriggered = true;
    }
    
    if ((stat & 0x20) && currentMode == Mode::OAMSearch) {
        interruptTriggered = true;
    }

    if ((stat & 0x40) && (stat & 0x04)) {
        interruptTriggered = true;
    }
    
    if (interruptTriggered) {
        this->gameboy->cpu->triggerInterrupt(Interrupt::LCD);
    }
}

//check bit 0 of LCDC to know if the background is enabled
inline bool PPU::isBGEnabled() const {
    uint8_t lcdc = this->gameboy->memory->fetch8(LCDC);
    return lcdc & 0x01;
}

//check bit 5 of LCDC to know if window is enbled
inline bool PPU::isWDEnabled() const {
    uint8_t lcdc = this->gameboy->memory->fetch8(LCDC);
    return lcdc & 0x20;
}

//check bit 1 of LCDC to know if sprites are enabled
inline bool PPU::areSpritesEnabled() const {
    uint8_t lcdc = this->gameboy->memory->fetch8(LCDC);
    return lcdc & 0x02;
}


void PPU::renderScanline(){
    if (isBGEnabled()){
        drawBackground();
    }
    if (isWDEnabled()){
        drawWindow();
    }
    if (areSpritesEnabled()){
        drawSprites();
    }
}





void PPU::drawBackground() {
    // Draw the background layer for the current scanline
    fetchBackgroundTileData();
}

void PPU::drawWindow() {
    // Draw the window layer for the current scanline
    fetchWindowTileData();
}

void PPU::drawSprites() {
    // Draw the sprites for the current scanline
    fetchSpriteData();
}

void PPU::fetchBackgroundTileData() {
    uint8_t lcdc = this->gameboy->memory->fetch8(LCDC); // LCDC register
    uint8_t scx = this->gameboy->memory->fetch8(SCX);  // Scroll X
    uint8_t scy = this->gameboy->memory->fetch8(SCY);  // Scroll Y
    uint8_t bgp = this->gameboy->memory->fetch8(BGP);  // Background palette

    // determine quel bg tile map to use (bit 3 of lcdc)    
    uint16_t tileMapBase = (lcdc & 0x08) ? 0x9C00 : 0x9800;

    // determine tile data mode (bit 4 of lcdc) signed indexing ou non
    bool tileDataMode = (lcdc & 0x10) != 0; // True -> 0x8000 mode, False -> 0x8800 mode

    // calculate the starting Y position in the background
    int tileRow = (((currentLY + scy) & 0xFF) / TILE_SIZE) & 0x1F;// &x1F it's like doing mod 32 (pour rester dasn la bonne plage 0-31)


    for (int x = 0; x < SCREEN_WIDTH; x++) {
        //calculer la starting x position in the background
        int tileCol = ((x + scx) / TILE_SIZE) & 0x1F;


        //ici on trouve le tile index dans le background tile map
        uint16_t tileAddress = tileMapBase + (tileRow * 32) + tileCol;
        uint8_t tileIndex = this->gameboy->memory->fetch8(tileAddress);

        // calculer l'address du tile data
        uint16_t tileDataAddress;
        if (tileDataMode) {
            tileDataAddress = 0x8000 + (tileIndex * 16); //“$8000 method” uses $8000 as its base pointer and uses an unsigned addressing
        } else {
            int8_t signedTileIndex = static_cast<int8_t>(tileIndex);
            tileDataAddress = 0x9000 + (signedTileIndex * 16); //“$8800 method” uses $9000 as its base pointer and uses a signed addressing
        }

        // determiner la ligne dans la tile to render
        int tileY = 2*(((currentLY + scy) & 7)); // c'est comme faire mod 8

        uint16_t rowAddress = tileDataAddress + tileY;

        // Fetch the two bytes representing the pixel row
        uint8_t lowByte = this->gameboy->memory->fetch8(rowAddress);
        uint8_t highByte = this->gameboy->memory->fetch8(rowAddress + 1);

        // Compute the pixel's color for the current screen X
        int tileX = (x + scx) & 7;
        int bit = 7 - tileX; // Pixels are stored in MSB order
        uint8_t colorIndex = ((highByte >> bit) & 0x01) << 1 | ((lowByte >> bit) & 0x01);

        // Apply the BGP palette
        uint8_t color = (bgp >> (colorIndex * 2)) & 0x03;

        // Store the pixel in the framebuffer
        bgBuffer[currentLY][x] = color;
        framebuffer[currentLY][x] = color;

        if(color != 0) {
            *logger << "Background color: " + to_string(color) + ", X: " + to_string(x) + ", Y: " + to_string(currentLY);
        }
    }
}


void PPU::fetchWindowTileData() {
    uint8_t lcdc = this->gameboy->memory->fetch8(LCDC);
    uint8_t wx = this->gameboy->memory->fetch8(WX);
    uint8_t wy = this->gameboy->memory->fetch8(WY);
    uint8_t bgp = this->gameboy->memory->fetch8(BGP);

    if (!(lcdc & 0x20) || currentLY < wy || wy>143 || wx < 7 || wx > 166) return;

    uint16_t tileMapBase = (lcdc & 0x40) ? 0x9C00 : 0x9800;

    bool tileDataMode = (lcdc & 0x10) != 0;

    int tileRow = (((currentLY - wy) / TILE_SIZE) & 0x1F);

    for (int x = 0; x < SCREEN_WIDTH; x++){
        int windowX = x - wx + 7;
        if (windowX < 0 ) continue;

        int tileCol = (windowX / TILE_SIZE) & 0x1F;

        uint16_t tileAddress = tileMapBase + (tileRow * 32) + tileCol;
        uint8_t tileIndex = this->gameboy->memory->fetch8(tileAddress);

        uint16_t tileDataAddress;
        if (tileDataMode) {
            tileDataAddress = 0x8000 + (tileIndex * 16);
        } else {
            int8_t signedTileIndex = static_cast<int8_t>(tileIndex);
            tileDataAddress = 0x9000 + (signedTileIndex * 16);
        }

        int tileY = 2 * ((currentLY - wy) & 7);

        uint16_t rowAddress = tileDataAddress + tileY;

        uint8_t lowByte = this->gameboy->memory->fetch8(rowAddress);
        uint8_t highByte = this->gameboy->memory->fetch8(rowAddress + 1);

        int tileX = windowX & 7;
        int bit = 7 - tileX;
        uint8_t colorIndex = ((highByte >> bit) & 0x01) << 1 | ((lowByte >> bit) & 0x01);

        uint8_t color = (bgp >> (colorIndex * 2)) & 0x03;

        framebuffer[currentLY][x] = color;

        if(color != 0) {
            *logger << "Background color: " + to_string(color) + ", X: " + to_string(x) + ", Y: " + to_string(currentLY);
        }
    }

}



void PPU::fetchSpriteData() {
    uint8_t lcdc = this->gameboy->memory->fetch8(LCDC); 
    // check the bit 2 of lcdc to know sprite size
    bool spriteSize = lcdc & 0x04; // sprite size can be 8x8 or 8x16
    int spriteHeight = spriteSize ? 16 : 8;

    //on peut avoir max 10 sprites par scanline
    const int SPRITES_PER_LINE_LIMIT = 10;
    // int spritesRendered = 0;

    struct SpriteInfo {
        int index;
        uint8_t x;
    };
    SpriteInfo visibleSprites[40]; //pour les sprites visibles
    int visibleSpriteCount = 0;

    //Sprite data is stored in the OAM section of memory which can fit up to 40 sprites.
    for (int i = 0; i < 40 ; i++) {
        uint8_t yPos = this->gameboy->memory->fetch8(OAM_OFFSET + i * 4) - 16;
        
        if (yPos > 144 || currentLY < yPos || currentLY >= yPos + spriteHeight) 
        continue;
        
        uint8_t xPos = this->gameboy->memory->fetch8(OAM_OFFSET + i * 4 + 1) - 8;

        if (visibleSpriteCount < 40) {
            visibleSprites[visibleSpriteCount].index = i;
            visibleSprites[visibleSpriteCount].x = xPos;
            visibleSpriteCount++;
        }
    }

    for (int i = 0; i < visibleSpriteCount - 1; i++) {
        for (int j = 0; j < visibleSpriteCount - i - 1; j++) {
            if (visibleSprites[j].x > visibleSprites[j + 1].x) {
                SpriteInfo temp = visibleSprites[j];
                visibleSprites[j] = visibleSprites[j + 1];
                visibleSprites[j + 1] = temp;
            }
        }
    }

    int spritesToRender = (visibleSpriteCount > SPRITES_PER_LINE_LIMIT) ? SPRITES_PER_LINE_LIMIT : visibleSpriteCount;

    // pour que les sprites avec le plus grand x ecrasent les autres
    for (int i = spritesToRender - 1; i >= 0; i--) {
        int spriteIndex = visibleSprites[i].index;
        uint8_t yPos = this->gameboy->memory->fetch8(OAM_OFFSET + spriteIndex * 4) - 16;
        uint8_t xPos = this->gameboy->memory->fetch8(OAM_OFFSET + spriteIndex * 4 + 1) - 8;
        uint8_t tileIndex = this->gameboy->memory->fetch8(OAM_OFFSET + i * 4 + 2);
        uint8_t attributes = this->gameboy->memory->fetch8(OAM_OFFSET + i * 4 + 3);

        int tileY = currentLY - yPos;

        if (attributes & 0x40) { // Vertical flip
            tileY = spriteHeight - 1 - tileY;
        }

        if (spriteSize) {
            tileIndex &= 0xFE; //quand on est en 8x16 mode, le bit 0 est ignored

            if (tileY >= 8) {
                tileIndex ++; // On prend le deuxième sprite de la paire
                tileY -= 8; // On ajuste la position Y pour le deuxième sprite
            }
        }

        uint16_t tileDataAddress = 0x8000 + (tileIndex * 16) + (tileY * 2);
        uint8_t lowByte = this->gameboy->memory->fetch8(tileDataAddress);
        uint8_t highByte = this->gameboy->memory->fetch8(tileDataAddress + 1);

        uint8_t palette = (attributes & 0x10) ? this->gameboy->memory->fetch8(OBP1) : this->gameboy->memory->fetch8(OBP0);


        for (int x = 0; x < 8; x++) {
            if (xPos + x < 0 || xPos + x >= SCREEN_WIDTH) continue;

            int bit = attributes & 0x20 ? x : 7 - x; // Horizontal flip

            uint8_t colorIndex = ((highByte >> bit) & 0x01) << 1 | ((lowByte >> bit) & 0x01);
            if (colorIndex == 0) continue; // Transparent

            uint8_t color = (palette >> (colorIndex * 2)) & 0x03;

            int pixelX = xPos + x;
            uint8_t bgPixel = bgBuffer[currentLY][pixelX];

            bool drawSprite = true;

            if (attributes & 0x80) {
                drawSprite = (bgPixel == 0);
            }

            if (drawSprite) {
                framebuffer[currentLY][pixelX] = color;
                if (logger != nullptr && color != 0) {
                    *logger << "Sprite color: " + std::to_string(color) + 
                                ", X: " + std::to_string(pixelX) + 
                                ", Y: " + std::to_string(currentLY);
                }
            }
        }
    }
}