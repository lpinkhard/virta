//
//  MOS6502.h
//  Interface for MOS6502
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

#ifndef MOS6502_H
#define MOS6502_H

#include <chrono>
#include <cstdint>

#include <memory>

#include "CPU.h"

class MOS6502 : public CPU {
    /**
     * Registers of the MOS6502 CPU.
     */
    struct {
        uint8_t A;      // accumulator
        uint8_t X;      // X index
        uint8_t Y;      // Y index
        uint8_t S;      // stack pointer
        uint16_t PC;    // program counter
        uint8_t P;      // processor flags (NV-BDIZC)
    } registers;
    
    uint16_t newPC;     // tracks what PC will become
    
    /**
     * Processor flags.
     */
    const uint8_t FLAG_N = 0x80;    // sign flag
    const uint8_t FLAG_V = 0x40;    // overflow flag
    const uint8_t FLAG_B = 0x10;    // breakpoint flag
    const uint8_t FLAG_D = 0x8;     // BCD flag
    const uint8_t FLAG_I = 0x4;     // interrupt flag
    const uint8_t FLAG_Z = 0x2;     // zero flag
    const uint8_t FLAG_C = 0x1;     // carry flag
    
    /**
     * Memory addressing modes.
     */
    enum AddressingMode {
        AM_IMM,         // #immediate
        AM_ACC,         // accumulator
        AM_ABS,         // abs
        AM_ABS_X,       // abs,X
        AM_ABS_Y,       // abs,Y
        AM_ZP,          // zp
        AM_ZP_X,        // zp,X
        AM_ZP_Y,        // zp,Y
        AM_ZP_X_IND,    // (zp,X)
        AM_ZP_IND_X,    // (zp),X
        AM_ZP_IND_Y     // (zp),Y
    };
    
    uint16_t readPtr(AddressingMode addressingMode);
    uint8_t readValue(AddressingMode addressingMode);
    void writeValue(AddressingMode addressingMode, uint8_t value);
    
    void setNZ(uint8_t value);  // Set N and Z flags based on value

    void push(uint8_t value);   // Push value onto the stack
    uint8_t pop();              // Pop value from the stack
    
    void execute();
    
public:
    /**
     * Types of interrupts.
     */
    enum Interrupt {
        INT_NONE,
        INT_BRK,
        INT_IRQ,
        INT_NMI,
        INT_RESET
    };
    
    MOS6502(std::shared_ptr<MemoryMap> memoryMap);
    ~MOS6502();
    
    void interrupt(Interrupt type);
    void reset();
    void irq();
    void nmi();
    
    void dumpState();
    
private:
    Interrupt pendingInterrupt;
    uint_fast8_t getCycles(uint8_t opcode);
};

#endif /* MOS6502_H */
