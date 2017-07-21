//
//  PETDisplay.cpp
//  Implementation of PETDisplay
//
//  Does nothing at present
//
//  Created by Lionel Pinkhard on 2017/07/15.
//
//  MIT License
//
//  Copyright (c) 2017 Lionel Pinkhard
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in all
//  copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//  SOFTWARE.
//

#ifdef DEBUG    // Not production code

#include "PETDisplay.h"

#include <cstdio>

/**
 * Sets up a PET display
 */
PETDisplay::PETDisplay(uint16_t startAddress, void (*callback)()) : VideoMemory(startAddress, 2048), callback(callback) {
    
}

/**
 * Empty destructor
 */
PETDisplay::~PETDisplay() {
    
}

/**
 * Reads a byte from the display
 */
uint8_t PETDisplay::readByte(uint16_t address) {
    return 0;
}

/**
 * Writes a byte to the display
 */
void PETDisplay::writeByte(uint16_t address, uint8_t value) {

}

/**
 * Clears the display
 */
void PETDisplay::clear() {
    
}

/**
 * Timer to update the cursor
 */
void PETDisplay::timer() {
    
}

/**
 * Returns the timer frequency
 */
float PETDisplay::timerDuration() {
    return 0.1;
}

/**
 * Renders a frame of the emulation
 */
void PETDisplay::render(int width, int height) {
    
}

/**
 * Adjusts the display according to the window size
 */
void PETDisplay::reshape(int width, int height) {
    
}

#endif
