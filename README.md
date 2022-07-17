# Tic

_Tic_ is Tic-Tac-Toe game (surprise!) implemented in C++ with unbeatable AI based on Minimax.

# Features

 * Simple GUI based on FLTK
 * Difficulty selector with 4 different difficulties:
   * Easy: Computer has %75 chance of making a random move each turn
   * Medium: Computer has %50 chance of making a random move each turn
   * Hard: Computer has %30 chance of making a random move each turn
   * Unbeatable: Computer will make no mistakes

# Building

## Requirements

 * GNU make
 * GNU C++ Compiler (9.1 and higher)
 * FLTK (inc. FLUID)
 * On Windows: MSYS2

## Installing dependencies

### Linux

#### Debian/Ubuntu

    sudo apt install build-essential libfltk1.3-dev fluid

## Compiling

### Linux

    make -j$(nproc)
    
