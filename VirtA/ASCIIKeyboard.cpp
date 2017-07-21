//
//  ASCIIKeyboard.cpp
//  Implementation of ASCIIKeyboard
//  Simulates an ASCII keyboard connected to a PIA
//
//  Created by Lionel Pinkhard on 2017/07/12.
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

#include <cstdio>

#include "ASCIIKeyboard.h"

/**
 * Construction of an ASCIIKeyboard instance
 */
ASCIIKeyboard::ASCIIKeyboard() : Peripheral() { }

/**
 * Destruction of an ASCIIKeyboard instance
 */
ASCIIKeyboard::~ASCIIKeyboard() {
    
}

/**
 * Reads a character from the data register
 */
uint8_t ASCIIKeyboard::read() {
    return PDR;
}

/**
 * Writes to the keyboard
 * Does nothing
 */
void ASCIIKeyboard::write(uint8_t value) {
    
}

/**
 * Simulates a key press
 */
void ASCIIKeyboard::keypress(uint8_t keycode) {
    if (keycode == 0xa || keycode == 0xd)     // CR
        keycode = 0x8d;
    if (keycode == 0x7f)   // Backspace
        keycode = 0xdf;
    
    if ((keycode & 0x60) == 0x60)   // Change lowercase to uppercase
        keycode &= 0xdf;
    
    irq1 = true;            // Set interrupt line 1
    PDR = keycode | 0x80;   // Set high bit
}

/**
 * Handles an array of key presses
 * For practical purposes, only the last key press is useful (PDR is overwritten by keypress method)
 */
void ASCIIKeyboard::textInput(const char *text) {
    int i = 0;
    while (text[i] != 0) {
        keypress(text[i++]);
    }
}
