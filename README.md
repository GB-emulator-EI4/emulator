# Game Boy Emulator

A Game Boy emulator written in C++ that emulates the core components of the original Game Boy hardware, including the CPU, PPU, memory, and more. The emulator also uses SDL2 for rendering the graphics to the screen.

---

## Features

- **CPU Emulation**: Executes Game Boy instructions by fetching, decoding, and executing opcodes.
- **PPU (Pixel Processing Unit)**: Renders the background, window, and sprites scanline by scanline into a framebuffer.
- **Memory Management**: Emulates the Game Boy's memory map, including VRAM, OAM, and registers.
- **Interrupt Handling**: Supports VBlank, LCD, and other hardware interrupts.
- **Timer Emulation**: Simulates the Game Boy's internal timers.
- **Logging System**: Provides detailed logs for debugging, with configurable levels and filters.
- **SDL2 Integration**: Renders the framebuffer to the screen using SDL2.

---

## Components

### **CPU**
- Fetches instructions from ROM, decodes them, and executes them.
- Handles arithmetic, logic, and control operations.
- Manages interrupts and timing.

### **PPU**
- Fetches background, window, and sprite data from memory.
- Renders graphics scanline by scanline into a framebuffer.
- Supports sprite priority, transparency, and palette management.

### **Memory**
- Manages access to the Game Boy's memory map, including:
  - VRAM (Video RAM)
  - OAM (Object Attribute Memory)
  - I/O registers
  - Work RAM and external cartridge RAM.

### **SDL Renderer**
- Fetches pixel data from the framebuffer and renders it to the screen.
- Uses SDL2 for cross-platform rendering.

### **Logger**
- Provides a centralized logging system for debugging.
- Supports configurable log levels (`LOG_LOG`, `LOG_WARNING`, `LOG_ERROR`) and filters for specific domains (e.g., `CPU`, `PPU`, `Memory`).

---

## How It Works

1. **CPU Execution**:
   - The CPU fetches instructions from memory, decodes them, and executes them.
   - It interacts with memory and other components like the PPU and timers.

2. **PPU Rendering**:
   - The PPU processes background, window, and sprite data to render each scanline.
   - The rendered pixels are stored in a framebuffer.

3. **Memory Management**:
   - The memory system provides access to various regions of the Game Boy's memory map.
   - It handles reads and writes to VRAM, OAM, and I/O registers.

4. **SDL Rendering**:
   - The SDL renderer fetches pixel data from the framebuffer and displays it on the screen.

---

## Installation

### Prerequisites
- **C++ Compiler**: A C++17-compatible compiler (e.g., `g++` or `clang`).
- **SDL2**: Install SDL2 development libraries.
- **Make**: Ensure `make` is installed for building the project.

### Build Instructions
1. Clone the repository:
   ```bash
   git clone https://github.com/your-username/gameboy-emulator.git
   cd gameboy-emulator

2. Install SDL2:
   ```bash
   sudo apt install libsdl2-dev 

3. Build and run the project
   ```bash
   make

---
## Usage

To run a Game Boy ROM, go to constants.hpp file and define the path to your ROM.
