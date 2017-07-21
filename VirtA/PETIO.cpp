//
//  PETIO.cpp
//  Implementation of PETIO
//  Simulates a Commodore PET's I/O system
//
//  Work in progress
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

#include <cstdint>

#include "PETIO.h"
#include "MemoryMap.h"

using namespace std;

/**
 * Sets up the I/O system
 */
PETIO::PETIO(uint16_t startAddress, shared_ptr<Peripheral> keyboard, shared_ptr<VideoMemory> display)
    : MemoryInterface(startAddress, 2048), keyboard(keyboard), display(display) {
    reset();
}

/**
 * Empty destructor
 */
PETIO::~PETIO() { }

/**
 * Writes a byte to the I/O system
 */
void PETIO::writeByte(uint16_t address, uint8_t value) {
    uint16_t maskedAddress = address & 0x1f;    // Get relevant bits of address
    
    switch(maskedAddress) {
        case 0x10:  // Write data
            if ((PIA1registers.CRA & CR_FLAG_DDR) == CR_FLAG_DDR) {     // Check DDR line
                if (keyboard != NULL)   // Keyboard row select
                    keyboard->write((value & 0xf) & PIA1registers.DDRA);
            } else {
                PIA1registers.DDRA = value;
            }
            break;
        case 0x11:  // Write CRA
            PIA1registers.CRA = value;
            if ((PIA1registers.CRA & CR_FLAG_CX2) == CR_FLAG_CX2) {  // Blank screen
                if (display != NULL)
                    display->clear();
            }
            break;
        case 0x12:  // Write data (keyboard)
            if ((PIA1registers.CRB & CR_FLAG_DDR) == CR_FLAG_DDR) {     // Check DDR line
                
            } else {
                PIA1registers.DDRB = value;
            }
            break;
        case 0x13:  // Write CRB
            PIA1registers.CRB = value;
            break;
        default:
            break;
    }
}

/**
 * Reads a byte from the I/O system
 */
uint8_t PETIO::readByte(uint16_t address) {
    uint16_t maskedAddress = address & 0x1f;    // Get relevant bits of address
    uint8_t result = 0;
    
    switch(maskedAddress) {
        case 0x10:  // Read receive data
            if ((PIA1registers.CRA & CR_FLAG_DDR) == CR_FLAG_DDR) {     // Check DDR line
                PIA1registers.CRA &= ~CR_FLAG_IRQ1;     // Clear IRQ1
            } else {
                result = PIA1registers.DDRA;
            }
            break;
        case 0x11:  // Read receive status
            return PIA1registers.CRA & ~CR_FLAG_CX1;    // CA1 is input only
            break;
        case 0x12:  // Read keyboard
            if ((PIA1registers.CRB & CR_FLAG_DDR) == CR_FLAG_DDR) {     // Check DDR line
                if (keyboard != NULL)
                    result = keyboard->read() & ~PIA1registers.DDRB;     // Keyboard read
                PIA1registers.CRA &= ~CR_FLAG_IRQ2;
            } else {
                result = PIA1registers.DDRB;
            }
            break;
        case 0x13:
            return 0xff;    // Screen retrace on CB1
            break;
        case 0x20:  // Read receive data
            if ((PIA2registers.CRA & CR_FLAG_DDR) == CR_FLAG_DDR) {     // Check DDR line
                
                PIA2registers.CRA &= ~CR_FLAG_IRQ1;     // Clear IRQ1
            } else {
                result = PIA2registers.DDRA;
            }
            break;
        case 0x21:  // Read receive status
            return PIA2registers.CRA;
            break;
        case 0x22:  // Read receive data
            if ((PIA2registers.CRB & CR_FLAG_DDR) == CR_FLAG_DDR) {     // Check DDR line
                
                PIA2registers.CRA &= ~CR_FLAG_IRQ1;
            } else {
                result = PIA2registers.DDRB;
            }
            break;
        case 0x23:
            return PIA2registers.CRB;
            break;
        default:
            break;
    }
    
    return result;
}

/**
 * Resets the I/O system state
 */
void PETIO::reset() {
    PIA1registers.CRA = 0x0;
    PIA1registers.CRB = 0x0;
    PIA1registers.DDRA = 0x0;
    PIA1registers.DDRB = 0x0;
    PIA2registers.CRA = 0x0;
    PIA2registers.CRB = 0x0;
    PIA2registers.DDRA = 0x0;
    PIA2registers.DDRB = 0x0;
}

#endif
