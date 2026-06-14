# Gameboy Emulator

A Game Boy emulator written in C.

## Features

- Fully-functioning CPU with full Game Boy instruction set (passes all blargg tests)
- Full color PPU - passes all dmg-acid2 tests (original gameboy) and cgb-acid2 tests (color gameboy) 
- Utilizes SDL2 library for window and audio support
- Functioning button i/o
- Supports multiple cartridge MBC types: ROM-only, MBC1, MBC3, MBC5
- Supports save states
- Visualization of 8-bit audio waveforms for all four audio channels
- Ability to alter emulator speed from 0x to 8x
- Offers both Windows and Linux support
- Navigatable ROM menu to select which game to play

## How to set up - Linux

1. Clone the repository from Github
2. Download any Game Boy ROMs into the rom/ folder (can find ROMs at https://vimm.net/vault/GB or https://vimm.net/vault/GBC)
3. Install SDL2 and SDL2_ttf libraries onto your computer
   - On Ubuntu: sudo apt install libsdl2-dev libsdl2-ttf-dev
4. Navigate to project directory and run 'make run'

## How to play

- How to select ROM: Use <kbd>&uarr;</kbd> and <kbd>&darr;</kbd> keys to navigate ROM menu - press <kbd>Enter</kbd> to select which ROM to play
- Game Boy Buttons:
  - A: <kbd>X</kbd>, B: <kbd>Z</kbd>, Start: <kbd>Esc</kbd>, Select: <kbd>Tab</kbd>
  - D-pad: <kbd>&uarr;</kbd>, <kbd>&darr;</kbd>, <kbd>&larr;</kbd>, <kbd>&rarr;</kbd> or <kbd>W</kbd>, <kbd>S</kbd>, <kbd>A</kbd>, <kbd>D</kbd>
- How to save: Save as normal in game, then press <kbd>Ctrl</kbd> + <kbd>Shift</kbd> + <kbd>B</kbd> to write a binary save file to the save/ folder - the emulator will load that save file whenever the game tries to read a save
- How to quit: Press <kbd>Ctrl</kbd> + <kbd>Q</kbd> to quit the current game and return to the ROM menu - Close the window to quit the emulator
- How to change emulator speed: Press <kbd>Ctrl</kbd> + <kbd>Shift</kbd> + <kbd><</kbd> or <kbd>></kbd> to decrease or increase emulator speed, respectively - speed can be any integer from 0x to 8x
