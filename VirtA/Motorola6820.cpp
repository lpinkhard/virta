//
//  Motorola6820.cpp
//  Implementation for Motorola6820
//  Emulates a Motorola 6820 Peripheral Interface Adapter
//
//  Created by Lionel Pinkhard on 2017/07/07.
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

#include <iostream>

#include <cstdint>

#include "Motorola6820.h"
#include "MemoryMap.h"

using namespace std;

/**
 * Sets up a 6820 at the given address with the given peripherals
 */
Motorola6820::Motorola6820(uint16_t startAddress, shared_ptr<Peripheral> portA, shared_ptr<Peripheral> portB)
    : MemoryInterface(startAddress, 2048), portA(portA), portB(portB) {
    reset();
}

/**
 * Empty destructor
 */
Motorola6820::~Motorola6820() { }

/**
 * Writes a byte to the 6820
 */
void Motorola6820::writeByte(uint16_t address, uint8_t value) {
    uint16_t maskedAddress = address & 0x1f;    // Get relevant bits of address
    
    switch(maskedAddress) {
        case 0x10:  // Write data (port A)
            if ((registers.CRB & CR_FLAG_DDR) == CR_FLAG_DDR) {     // Check DDR line
                if (portA != NULL)
                    portA->write(value & registers.DDRA);
            } else {
                registers.DDRB = value;
            }
            break;
        case 0x11:  // Write CRA
            registers.CRA = value;
            break;
        case 0x12:  // Write data (port B)
            if ((registers.CRB & CR_FLAG_DDR) == CR_FLAG_DDR) {     // Check DDR line
                if (portB != NULL)
                    portB->write(value & registers.DDRB);
            } else {
                registers.DDRB = value;
            }
            break;
        case 0x13:  // Write CRB
            registers.CRB = value;
            break;
        default:
            break;
    }
}

/**
 * Reads a byte from the 6820
 */
uint8_t Motorola6820::readByte(uint16_t address) {
    uint16_t maskedAddress = address & 0x1f;    // Get relevant bits of address
    uint8_t result = 0;
    
    // Update CRs with IRQs
    if (portA != NULL && portA->interrupt1())
        registers.CRA |= CR_FLAG_IRQ1;
    if (portA != NULL && portA->interrupt2())
        registers.CRA |= CR_FLAG_IRQ2;
    if (portB != NULL && portB->interrupt1())
        registers.CRB |= CR_FLAG_IRQ1;
    if (portB != NULL && portB->interrupt2())
        registers.CRB |= CR_FLAG_IRQ2;
    
    switch(maskedAddress) {
        case 0x10:  // Read receive data
            if ((registers.CRA & CR_FLAG_DDR) == CR_FLAG_DDR) {     // Check DDR line
                if (portA != NULL)
                    result = portA->read() & ~registers.DDRA;  // Return keypress
                registers.CRA &= ~CR_FLAG_IRQ1;     // Clear IRQ1
            } else {
                result = registers.DDRA;
            }
            break;
        case 0x11:  // Read receive status
            return registers.CRA & ~CR_FLAG_CX1;    // CA1 is input only
            break;
        case 0x12:  // Read display status
            if ((registers.CRB & CR_FLAG_DDR) == CR_FLAG_DDR) {     // Check DDR line
                if (portB != NULL)
                    result = portB->read() & ~registers.DDRB;     // 0x0 Ready    0x80 Not Ready
                registers.CRA &= ~CR_FLAG_IRQ1;
            } else {
                result = registers.DDRB;
            }
            break;
        case 0x13:
            return registers.CRB & ~CR_FLAG_CX1;    // CB1 is input only
            break;
        default:    // Invalid address
            break;
    }
    
    return result;
}

/**
 * Sets the default register values on the 6820
 */
void Motorola6820::reset() {
    registers.CRA = 0x0;
    registers.CRB = 0x0;
    registers.DDRA = 0x0;
    registers.DDRB = 0x0;
}
