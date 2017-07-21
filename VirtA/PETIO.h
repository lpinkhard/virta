//
//  PETIO.h
//  Interface for PETIO
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

#ifndef PETIO_H
#define PETIO_H

#include <cstdint>
#include <memory>

#include "MemoryInterface.h"
#include "Peripheral.h"
#include "VideoMemory.h"

class PETIO : public MemoryInterface {
    std::shared_ptr<Peripheral> keyboard;
    std::shared_ptr<VideoMemory> display;
    
    struct registers {
        uint8_t CRA;    // Control Register A
        uint8_t CRB;    // Control Register B
        uint8_t DDRA;   // Data Direction Register A
        uint8_t DDRB;   // Data Direction Register B
    };
    
    registers PIA1registers;
    registers PIA2registers;
    
    /**
     * Control register flags.
     */
    const uint8_t CR_FLAG_IRQ1 = 0x80;  // IRQ1 flag
    const uint8_t CR_FLAG_IRQ2 = 0x40;  // IRQ2 flag
    const uint8_t CR_FLAG_CX2 = 0x38;   // Cx2 control flag
    const uint8_t CR_FLAG_DDR = 0x4;    // DDR flag
    const uint8_t CR_FLAG_CX1 = 0x3;    // Cx1 control flag
    
public:
    PETIO(uint16_t startAddress, std::shared_ptr<Peripheral> keyboard, std::shared_ptr<VideoMemory> display);
    ~PETIO();
    
    void writeByte(uint16_t address, uint8_t value);
    uint8_t readByte(uint16_t address);
    void reset();
};


#endif /* PETIO_H */

#endif
