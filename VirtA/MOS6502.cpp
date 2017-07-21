//
//  MOS6502.cpp
//  Implementation for MOS6502
//  Simulates a MOS Technology 6502 CPU running at 1MHz
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

#include <algorithm>
#include <chrono>
#include <memory>

#include "MOS6502.h"

using namespace std;
using namespace chrono;

/**
 * Initializes the CPU
 */
MOS6502::MOS6502(shared_ptr<MemoryMap> memoryMap) : CPU(memoryMap) {
    // Default values
    registers.A = 0xaa;
    registers.X = 0xc0;
    registers.Y = 0x00;
    registers.S = 0xbb;
    registers.PC = 0x0;
    registers.P = 0x20 | FLAG_B | FLAG_I | FLAG_Z;  // Bit 5 is always set
    
    reset();    // Trigger a RESET
}

/**
 * Frees memory used by the CPU implementation
 */
MOS6502::~MOS6502() {
    
}

/**
 * Reads a pointer according to the specified addressing mode
 */
uint16_t MOS6502::readPtr(AddressingMode addressingMode) {
    uint16_t value;
    
    switch (addressingMode) {
        case AM_IMM:    // #immediate
            return newPC++;
        case AM_ACC:    // accumulator
            return NULL;    // not possible
        case AM_ABS:    // abs
            value = memoryMap->readWord(newPC++);
            newPC++;     // value is 2 bytes long
            return value;
        case AM_ABS_X:  // abs,X
            value = memoryMap->readWord(newPC++) + registers.X;
            newPC++;     // value is 2 bytes long
            return value;
        case AM_ABS_Y:  // abs,Y
            value = memoryMap->readWord(newPC++) + registers.Y;
            newPC++;     // value is 2 bytes long
            return value;
        case AM_ZP:     // zp
            return static_cast<uint16_t>(memoryMap->readByte(newPC++));
        case AM_ZP_X:   // zp,X
            return (static_cast<uint16_t>(memoryMap->readByte(newPC++)) + static_cast<uint16_t>(registers.X)) & 0xff;
        case AM_ZP_Y:   // zp,Y
            return (static_cast<uint16_t>(memoryMap->readByte(newPC++)) + static_cast<uint16_t>(registers.Y)) & 0xff;
        case AM_ZP_X_IND:   // (zp,X)
            return memoryMap->readWord((static_cast<uint16_t>(memoryMap->readByte(newPC++)) +
                                        static_cast<uint16_t>(registers.X)) & 0xff);
        case AM_ZP_IND_X:   // (zp),X
            return memoryMap->readWord(static_cast<uint16_t>(memoryMap->readByte(newPC++))) +
                                       static_cast<uint16_t>(registers.X);
        case AM_ZP_IND_Y:   // (zp),Y
            return memoryMap->readWord(static_cast<uint16_t>(memoryMap->readByte(newPC++))) +
                                       static_cast<uint16_t>(registers.Y);
    }
}

/**
 * Reads a value according to the specified addressing mode
 */
uint8_t MOS6502::readValue(AddressingMode addressingMode) {
    switch (addressingMode) {
        case AM_ACC:
            return registers.A;
        default:
            return memoryMap->readByte(readPtr(addressingMode));
    }
}

/**
 * Writes a value according to the specified addresing mode
 */
void MOS6502::writeValue(AddressingMode addressingMode, uint8_t value) {
    switch (addressingMode) {
        case AM_IMM:    // #immediate
            break;      // not possible
        case AM_ACC:    // accumulator
            registers.A = value;
            break;
        default:    // abs
            memoryMap->writeByte(readPtr(addressingMode), value);
            break;
    }
}

/**
 * Pushes a value onto the stack
 */
void MOS6502::push(uint8_t value) {
    memoryMap->writeByte(static_cast<uint16_t>(registers.S--) | 0x100, value);
}

/**
 * Pops a value from the stack
 */
uint8_t MOS6502::pop() {
    return memoryMap->readByte(static_cast<uint16_t>(++registers.S) | 0x100);
}

/**
 * Sets N and Z flags according to value
 */
void MOS6502::setNZ(uint8_t value) {
    if ((value & 0x80) == 0x80)   // Check negative
        registers.P |= FLAG_N;
    else
        registers.P &= ~FLAG_N;
    
    if (value == 0x0)   // Check zero
        registers.P |= FLAG_Z;
    else
        registers.P &= ~FLAG_Z;
}

/**
 * Executes a set of CPU instructions.
 */
void MOS6502::execute() {
    // Start time
    static high_resolution_clock::time_point start_time = high_resolution_clock::now();
    
    // Working copy of PC
    newPC = registers.PC;
    
    // Opcode to be executed
    uint8_t opcode;
    
    // Check for an interrupt, ignoring IRQ if interrupt flag is set
    if (pendingInterrupt == INT_NONE || (pendingInterrupt == INT_IRQ && (registers.P & FLAG_I) == FLAG_I))
        opcode = memoryMap->readByte(newPC++);  // Get the next opcode
    else
        opcode = 0x0;   // Simulate BRK for interrupt
    
    uint_fast8_t cycles = getCycles(opcode);    // Minimum 2 cycles
    
    // Break the opcode down into manageable parts
    uint_fast8_t opcodeGroup = opcode & 0x3;
    uint_fast8_t opcodeInstruction = (opcode & 0xe0) >> 5;
    uint_fast8_t opcodeMode = (opcode & 0x1c) >> 2;
        
    AddressingMode addressingMode = AM_IMM;     // Needs to be initialized to something
    
    uint8_t value8;     // Temporary storage for byte
    uint16_t value16;   // Temporary storage for word
    int16_t svalue16;   // Temporary storage for signed word
    bool boolValue;     // Temporary storage for boolean
    
    switch (opcodeGroup) {
        case 0x1:   // Most common opcodes
        case 0x3:   // Both bits set
            switch (opcodeMode) {
                case 0x0:   // (zp,X)
                    addressingMode = AM_ZP_X_IND;
                    break;
                case 0x1:   // zp
                    addressingMode = AM_ZP;
                    break;
                case 0x2:   // #immediate
                    addressingMode = AM_IMM;
                    break;
                case 0x3:   // abs
                    addressingMode = AM_ABS;
                    break;
                case 0x4:   // (zp),Y
                    addressingMode = AM_ZP_IND_Y;
                    break;
                case 0x5:   // zp,X
                    addressingMode = AM_ZP_X;
                    break;
                case 0x6:   // abs,Y
                    addressingMode = AM_ABS_Y;
                    break;
                case 0x7:   // abs,X
                    addressingMode = AM_ABS_X;
                    break;
            }
            
            switch (opcodeInstruction) {
                case 0x0:   // ORA
                    registers.A |= readValue(addressingMode);   // OR A
                    
                    setNZ(registers.A);
                    
                    break;
                case 0x1:   // AND
                    registers.A &= readValue(addressingMode);   // AND A
                    
                    setNZ(registers.A);
                    
                    break;
                case 0x2:   // EOR
                    registers.A ^= readValue(addressingMode);   // XOR A
                    
                    setNZ(registers.A);
                    
                    break;
                case 0x4:   // STA
                    if (opcodeMode != 0x2) {    // #immediate is invalid for STA
                        writeValue(addressingMode, registers.A);
                    }
                    break;
                case 0x5:   // LDA
                    registers.A = readValue(addressingMode);
                    
                    setNZ(registers.A);
                    
                    break;
                case 0x6:   // CMP
                    value8 = readValue(addressingMode);
                    
                    // Store result in a larger variable to determine carry
                    svalue16 = static_cast<int8_t>(registers.A) - static_cast<int8_t>(value8);
                    
                    setNZ(svalue16);
                    
                    if (registers.A < value8) { // Check carry
                        registers.P &= ~FLAG_C;
                    } else {
                        registers.P |= FLAG_C;
                    }
                    
                    break;
            }
            
            // ADC and SBC are fairly complex, yet very similar
            if (opcodeInstruction == 0x3 || opcodeInstruction == 0x7) {     // ADC or SBC
                if (opcodeInstruction == 0x3) { // ADC
                    value8 = static_cast<uint16_t>(readValue(addressingMode));   // Take normal value
                } else {    // SBC
                    value8 = static_cast<uint16_t>(~readValue(addressingMode));  // Take one's complement of value
                }
                
                // Calculation
                value16 = static_cast<uint16_t>(registers.A) + static_cast<uint16_t>(value8);
                
                if ((registers.P & FLAG_C) == FLAG_C) {  // Check carry flag
                    value16++;
                }
                
                if ((registers.P & FLAG_D) == FLAG_D) { // Handle decimal mode
                    // Decimal format conversion
                    if (((registers.A ^ value8 ^ value16) & 0x10) == 0x10)
                        value16 += 0x6;
                    if ((value16 & 0xf0) > 0x90)
                        value16 += 0x60;
                }
                
                if (((registers.A ^ value16) & (value8 ^ value16) & 0x80) == 0x80) { // Check overflow
                    registers.P |= FLAG_V;
                } else {
                    registers.P &= ~FLAG_V;
                }
                
                setNZ(static_cast<uint8_t>(value16));
                
                if ((value16 & 0xff00) == 0) {  // Check carry
                    registers.P &= ~FLAG_C;
                } else {
                    registers.P |= FLAG_C;
                }
                
                registers.A = static_cast<uint8_t>(value16 & 0xff);    // Store result
            }
            break;
    }
    
    switch (opcodeGroup) {
        case 0x2:
        case 0x3:   // Both bits set
            switch (opcodeMode) {
                case 0x0:   // #immediate
                    if (opcodeGroup != 0x2 || opcodeInstruction != 0x0) { // Not for *KIL
                        addressingMode = AM_IMM;
                    }
                    break;
                case 0x1:   // zp
                    addressingMode = AM_ZP;
                    break;
                case 0x2:   // accumulator
                    addressingMode = AM_ACC;
                    break;
                case 0x3:   // abs
                    addressingMode = AM_ABS;
                    break;
                case 0x5:   // zp,X
                    if (opcodeInstruction == 0x4 || opcodeInstruction == 0x5) { // STX and LDX use zp,Y instead
                        addressingMode = AM_ZP_Y;
                    } else {
                        addressingMode = AM_ZP_X;
                    }
                    break;
                case 0x7:   // abs,X
                    if (opcodeInstruction == 0x5) { // LDX uses abs,Y instead
                        addressingMode = AM_ABS_Y;
                    } else {
                        addressingMode = AM_ABS_X;
                    }
                    break;
            }
            if (opcodeMode != 0x0 || opcodeInstruction == 0x5) {    // #immediate is only valid for LDX
                switch (opcodeInstruction) {
                    case 0x0:   // ASL
                        if (addressingMode == AM_ACC) {
                            if ((registers.A & 0x80) == 0x80) { // Check for carry
                                registers.P |= FLAG_C;
                            } else {
                                registers.P &= ~FLAG_C;
                            }
                            
                            registers.A <<= 1;  // Shift left
                            
                            setNZ(registers.A);
                        } else {
                            value16 = readPtr(addressingMode);      // Read from memory
                            value8 = memoryMap->readByte(value16);
                            
                            if ((value8 & 0x80) == 0x80) {  // Check for carry
                                registers.P |= FLAG_C;
                            } else {
                                registers.P &= ~FLAG_C;
                            }
                            
                            value8 <<= 1;   // Shift left
                            
                            memoryMap->writeByte(value16, value8);  // Write back to memory
                            
                            setNZ(value8);
                        }
                        break;
                    case 0x1:   // ROL
                        if (addressingMode == AM_ACC) {
                            if ((registers.A & 0x80) == 0x80) { // Check for carry
                                boolValue = true;
                            } else {
                                boolValue = false;
                            }
                            
                            registers.A = (registers.A << 1) | (registers.P & FLAG_C);  // Shift left with carry
                            
                            if ((registers.A & 0x80) == 0x80) { // Check negative
                                registers.P |= FLAG_N;
                            } else {
                                registers.P &= ~FLAG_N;
                            }
                            
                            if (registers.A == 0x0) {   // Check zero
                                registers.P |= FLAG_Z;
                            } else {
                                registers.P &= ~FLAG_Z;
                            }
                        } else {
                            value16 = readPtr(addressingMode);      // Read from memory
                            value8 = memoryMap->readByte(value16);
                            
                            if ((value8 & 0x80) == 0x80) {  // Check for carry
                                boolValue = true;
                            } else {
                                boolValue = false;
                            }
                            
                            value8 = (value8 << 1) | (registers.P & FLAG_C);  // Shift left with carry
                            
                            memoryMap->writeByte(value16, value8);  // Write back to memory
                            
                            if ((value8 & 0x80) == 0x80) {  // Check negative
                                registers.P |= FLAG_N;
                            } else {
                                registers.P &= ~FLAG_N;
                            }
                            
                            if (value8 == 0x0) {  // Check zero
                                registers.P |= FLAG_Z;
                            } else {
                                registers.P &= ~FLAG_Z;
                            }
                        }
                        
                        if (boolValue) {
                            registers.P |= FLAG_C;  // Set carry
                        } else {
                            registers.P &= ~FLAG_C; // Clear carry
                        }
                        
                        break;
                    case 0x2:   // LSR
                        if (addressingMode == AM_ACC) {
                            if ((registers.A & 0x1) == 0x1) { // Check for carry
                                registers.P |= FLAG_C;
                            } else {
                                registers.P &= ~FLAG_C;
                            }
                            
                            registers.A >>= 1;  // Shift right
                            
                            if ((registers.A & 0x80) == 0x80) { // Check negative
                                registers.P |= FLAG_N;
                            } else {
                                registers.P &= ~FLAG_N;
                            }
                            
                            if (registers.A == 0x0) {   // Check zero
                                registers.P |= FLAG_Z;
                            } else {
                                registers.P &= ~FLAG_Z;
                            }
                        } else {
                            value16 = readPtr(addressingMode);      // Read from memory
                            value8 = memoryMap->readByte(value16);
                            
                            if ((value8 & 0x1) == 0x1) { // Check for carry
                                registers.P |= FLAG_C;
                            } else {
                                registers.P &= ~FLAG_C;
                            }
                            
                            value8 >>= 1;   // Shift right
                            
                            memoryMap->writeByte(value16, value8);  // Write back to memory
                            
                            if (value8 == 0x0) {  // Check zero
                                registers.P |= FLAG_Z;
                            } else {
                                registers.P &= ~FLAG_Z;
                            }
                        }
                        break;
                    case 0x3:   // ROR
                        if (addressingMode == AM_ACC) {
                            if ((registers.A & 0x1) == 0x1) { // Check for carry
                                boolValue = true;
                            } else {
                                boolValue = false;
                            }
                            
                            registers.A = (registers.A >> 1) | ((registers.P & FLAG_C) << 7);  // Shift right with carry
                            
                            if ((registers.A & 0x80) == 0x80) { // Check negative
                                registers.P |= FLAG_N;
                            } else {
                                registers.P &= ~FLAG_N;
                            }
                            
                            if (registers.A == 0x0) {   // Check zero
                                registers.P |= FLAG_Z;
                            } else {
                                registers.P &= ~FLAG_Z;
                            }
                        } else {
                            value16 = readPtr(addressingMode);      // Read from memory
                            value8 = memoryMap->readByte(value16);
                            
                            if ((value8 & 0x1) == 0x1) { // Check for carry
                                boolValue = true;
                            } else {
                                boolValue = false;
                            }
                            
                            value8 = (value8 >> 1) | ((registers.P & FLAG_C) << 7); // Shift right with carry
                            
                            memoryMap->writeByte(value16, value8);  // Write back to memory
                            
                            if ((value8 & 0x80) == 0x80) {  // Check negative
                                registers.P |= FLAG_N;
                            } else {
                                registers.P &= ~FLAG_N;
                            }
                            
                            if (value8 == 0x0) {  // Check zero
                                registers.P |= FLAG_Z;
                            } else {
                                registers.P &= ~FLAG_Z;
                            }
                        }
                        
                        if (boolValue) {
                            registers.P |= FLAG_C;  // Set carry
                        } else {
                            registers.P &= ~FLAG_C; // Clear carry
                        }
                        
                        break;
                    case 0x4:   // STX, TXA or TXS
                        if (opcodeMode == 0x2) {    // TXA
                            registers.A = registers.X;
                            
                            setNZ(registers.X);
                        } else if (opcodeMode == 0x6) { // TXS
                            registers.S = registers.X;
                            
                            setNZ(registers.X);
                        } else if (opcodeMode != 7) { // abs,X mode invalid for STX
                            writeValue(addressingMode, registers.X);
                        }
                        break;
                    case 0x5:   // LDX, TAX or TSX
                        if (opcodeMode == 0x2) {    // TAX
                            registers.X = registers.A;
                            
                            setNZ(registers.A);
                        } else if (opcodeMode == 0x6) { // TSX
                            registers.X = registers.S;
                            
                            setNZ(registers.S);
                        } else {    // LDX
                            registers.X = readValue(addressingMode);
                            
                            setNZ(registers.X);
                        }
                        break;
                    case 0x6:   // DEC
                        if (opcodeMode == 0x2) {    // DEX
                            registers.X--;
                            
                            setNZ(registers.X);
                        } else {    // DEC, accumulator mode is invalid for DEC
                            // Get the address
                            value16 = readPtr(addressingMode);
                            value8 = memoryMap->readByte(value16) - 1;
                            
                            setNZ(value8);
                            
                            // Decrement the value
                            memoryMap->writeByte(value16, value8);
                        }
                        break;
                    case 0x7:   // INC
                        if (opcodeMode == 0x2) {    // NOP
                        } else {    // INC, accumulator mode is invalid for INC
                            // Get the address
                            value16 = readPtr(addressingMode);
                            value8 = memoryMap->readByte(value16) + 1;
                            
                            setNZ(value8);
                            // Increment the value
                            memoryMap->writeByte(value16, value8);
                        }
                        break;
                }
            } else if (opcodeGroup == 0x2 && opcodeMode == 0x0) { // *KIL
                newPC--;     // crash
            }
            
            break;
        case 0x0:
            switch (opcodeMode) {
                case 0x0:   // #immediate
                    if (opcodeInstruction == 0x1) { // JSR (abs) uses abs instead
                        addressingMode = AM_ABS;
                    } else {
                        addressingMode = AM_IMM;
                    }
                    break;
                case 0x1:   // zp
                    addressingMode = AM_ZP;
                    break;
                case 0x3:   // abs
                    addressingMode = AM_ABS;
                    break;
                case 0x4:   // conditional branch instruction
                    switch (opcodeInstruction) {
                        case 0x0:   // BPL
                            if ((registers.P & FLAG_N) == 0x0) { // Negative clear
                                cycles++;
                                value8 = memoryMap->readByte(newPC++);
                                // Check for page boundary cross
                                if (((newPC + static_cast<int8_t>(value8)) & 0xff00) !=
                                    (newPC & 0xff00)) {
                                    cycles++;
                                }
                                newPC += static_cast<int8_t>(value8);
                            } else {
                                newPC++;
                            }
                            
                            break;
                        case 0x1:   // BMI
                            if ((registers.P & FLAG_N) == FLAG_N) { // Negative set
                                cycles++;
                                value8 = memoryMap->readByte(newPC++);
                                // Check for page boundary cross
                                if (((newPC + static_cast<int8_t>(value8)) & 0xff00) !=
                                    (newPC & 0xff00)) {
                                    cycles++;
                                }
                                newPC += static_cast<int8_t>(value8);
                            } else {
                                newPC++;
                            }
                            
                            break;
                        case 0x2:   // BVC
                            if ((registers.P & FLAG_V) == 0x0) { // Overflow clear
                                cycles++;
                                value8 = memoryMap->readByte(newPC++);
                                // Check for page boundary cross
                                if (((newPC + static_cast<int8_t>(value8)) & 0xff00) !=
                                    (newPC & 0xff00)) {
                                    cycles++;
                                }
                                newPC += static_cast<int8_t>(value8);
                            } else {
                                newPC++;
                            }
                            
                            break;
                        case 0x3:   // BVS
                            if ((registers.P & FLAG_V) == FLAG_V) { // Overflow set
                                cycles++;
                                value8 = memoryMap->readByte(newPC++);
                                // Check for page boundary cross
                                if (((newPC + static_cast<int8_t>(value8)) & 0xff00) !=
                                    (newPC & 0xff00)) {
                                    cycles++;
                                }
                                newPC += static_cast<int8_t>(value8);
                            } else {
                                newPC++;
                            }
                            
                            break;
                        case 0x4:   // BCC
                            if ((registers.P & FLAG_C) == 0x0) { // Carry clear
                                cycles++;
                                value8 = memoryMap->readByte(newPC++);
                                // Check for page boundary cross
                                if (((newPC + static_cast<int8_t>(value8)) & 0xff00) !=
                                    (newPC & 0xff00)) {
                                    cycles++;
                                }
                                newPC += static_cast<int8_t>(value8);
                            } else {
                                newPC++;
                            }
                            
                            break;
                        case 0x5:   // BCS
                            if ((registers.P & FLAG_C) == FLAG_C) { // Carry set
                                cycles++;
                                value8 = memoryMap->readByte(newPC++);
                                // Check for page boundary cross
                                if (((newPC + static_cast<int8_t>(value8)) & 0xff00) !=
                                    (newPC & 0xff00)) {
                                    cycles++;
                                }
                                newPC += static_cast<int8_t>(value8);
                            } else {
                                newPC++;
                            }
                            
                            break;
                        case 0x6:   // BNE
                            if ((registers.P & FLAG_Z) == 0x0) { // Zero clear
                                cycles++;
                                value8 = memoryMap->readByte(newPC++);
                                // Check for page boundary cross
                                if (((newPC + static_cast<int8_t>(value8)) & 0xff00) !=
                                    (newPC & 0xff00)) {
                                    cycles++;
                                }
                                newPC += static_cast<int8_t>(value8);
                            } else {
                                newPC++;
                            }
                            
                            break;
                        case 0x7:   // BEQ
                            if ((registers.P & FLAG_Z) == FLAG_Z) { // Zero set
                                cycles++;
                                value8 = memoryMap->readByte(newPC++);
                                // Check for page boundary cross
                                if (((newPC + static_cast<int8_t>(value8)) & 0xff00) !=
                                    (newPC & 0xff00)) {
                                    cycles++;
                                }
                                newPC += static_cast<int8_t>(value8);
                            } else {
                                newPC++;
                            }
                            
                            break;
                    }
                    break;
                case 0x5:   // zp,X
                    addressingMode = AM_ZP_X;
                    break;
                case 0x7:   // abs,X
                    addressingMode = AM_ABS_X;
                    break;
            }
            if (opcodeMode != 0x4) {    // conditional branching groups into addressing mode
                switch (opcodeInstruction) {
                    case 0x0:   // BRK
                        if (opcodeMode == 0x0) {    // BRK only valid with no mode
                            if (pendingInterrupt == INT_RESET) {
                                registers.S -= 3;   // Fake stack pushes for RESET
                            } else {
                                if (pendingInterrupt == INT_NONE) { // software interrupt, BRK
                                    registers.PC += 2;  // BRK advances PC by 2
                                }
                                
                                push((registers.PC & 0xff00) >> 8);     // Push PC high
                                push(registers.PC & 0xff);              // Push PC low
                            
                                uint8_t newP = registers.P;
                                if (pendingInterrupt == INT_NONE) {     // Check for software interrupt (BRK)
                                    newP |= FLAG_B;     // Set B flag
                                }
                                push(newP);     // Push processor flags
                            }
                            
                            registers.P |= FLAG_I;  // Set I flag
                            
                            switch (pendingInterrupt) {
                                case INT_NONE:
                                case INT_BRK:
                                case INT_IRQ:
                                    newPC = memoryMap->readWord(0xfffe);
                                    break;
                                case INT_NMI:
                                    newPC = memoryMap->readWord(0xfffa);
                                    break;
                                case INT_RESET:
                                    newPC = memoryMap->readWord(0xfffc);
                                    break;
                            }
                            
                            pendingInterrupt = INT_NONE;    // Clear the pending interrupt
                        } else if (opcodeMode == 0x1) { // *NOP
                        } else if (opcodeMode == 0x2) { // PHP fits this opcode
                            push(registers.P);  // Push processor flags
                        } else if (opcodeMode == 0x6) { // CLC
                            registers.P &= ~FLAG_C; // Clear carry flag
                        }
                        break;
                    case 0x1:   // BIT
                        if (opcodeMode == 0x0) {    // JSR (abs) instead of BIT
                            // Get the destination address
                            value16 = readPtr(addressingMode);
                            
                            push(((registers.PC + 3) & 0xff00) >> 8);     // Push PC high
                            push((registers.PC + 3) & 0xff);              // Push PC low
                            
                            newPC = value16;    // Set new address
                        } else if (opcodeMode == 0x2) { // PLP instead of BIT
                            registers.P = pop();    // Pop processor flags
                        } else if (opcodeMode == 0x6) { // SEC
                            registers.P |= FLAG_C;  // Set carry flag
                        } else {    // BIT
                            value8 = readValue(addressingMode);
                            if ((value8 & FLAG_N) == FLAG_N) { // Negative set
                                registers.P |= FLAG_N;
                            } else {
                                registers.P &= ~FLAG_N;
                            }
                            if ((value8 & FLAG_V) == FLAG_V) { // Overflow set
                                registers.P |= FLAG_V;
                            } else {
                                registers.P &= ~FLAG_V;
                            }
                            if ((value8 & registers.A) == 0) {  // Logical AND with accumulator
                                registers.P |= FLAG_Z;  // Set zero
                            } else {
                                registers.P &= ~FLAG_Z; // Clear zero
                            }
                        }

                        break;
                    case 0x2:   // JMP
                        if (opcodeMode == 0x0) {    // RTI instead of JMP
                            // Get the processor flags
                            registers.P = pop();
                            
                            // Get the return address
                            value16 = pop();                                // Pop PC low
                            value16 |= static_cast<uint16_t>(pop()) << 8;   // Pop PC high
                            
                            newPC = value16;    // Set new address
                        } else if (opcodeMode == 0x2) { // PHA
                            push(registers.A);  // Push accumulator
                        } else if (opcodeMode == 0x6) { // CLI
                            registers.P &= ~FLAG_I; // Clear interrupt flag
                        } else {
                            // Update PC to new address
                            newPC = readPtr(addressingMode);
                        }
                        break;
                    case 0x3:   // JMP (abs)
                        if (opcodeMode == 0x0) {    // RTS instead of JMP (abs)
                            // Get the return address
                            value16 = pop();                                // Pop PC low
                            value16 |= static_cast<uint16_t>(pop()) << 8;   // Pop PC high
                            
                            newPC = value16;    // Set new address
                        } else if (opcodeMode == 0x2) { // PLA instead of JMP (abs)
                            registers.A = pop();    // Pop accumulator
                            
                            if ((registers.A & 0x80) == 0x80) { // Check negative
                                registers.P |= FLAG_N;
                            } else {
                                registers.P &= ~FLAG_N;
                            }
                            
                            if (registers.A == 0x0) {   // Check zero
                                registers.P |= FLAG_Z;
                            } else {
                                registers.P &= ~FLAG_Z;
                            }
                        } else if (opcodeMode == 0x6) { // SEI
                            registers.P |= FLAG_I;  // Set interrupt flag
                        } else {    // JMP (abs)
                            // Update PC to new address
                            newPC = memoryMap->readWord(readPtr(addressingMode));
                            
                            memoryMap->dumpMonitor(0x20, 2);
                        }
                        break;
                    case 0x4:   // STY, TYA or DEY
                        if (opcodeMode == 0x2) {    // DEY instead of STY
                            registers.Y--;
                            
                            setNZ(registers.Y);
                        } else if (opcodeMode == 0x6) { // TYA
                            registers.A = registers.Y;
                            
                            setNZ(registers.Y);
                        } else {
                            writeValue(addressingMode, registers.Y);
                        }
                        break;
                    case 0x5:   // LDY
                        if (opcodeMode == 0x2) {    // TAY instead of LDY
                            registers.Y = registers.A;
                            
                            setNZ(registers.A);
                        } else if (opcodeMode == 0x6) { // CLV
                            registers.P &= ~FLAG_V; // Clear overflow flag
                        } else {    // LDY
                            registers.Y = readValue(addressingMode);
                            
                            setNZ(registers.Y);
                        }
                        break;
                    case 0x6:   // CPY, INY or CLD
                        if (opcodeMode == 0x2) {    // INY instead of CPY
                            registers.Y++;
                            
                            setNZ(registers.Y);
                        } else if (opcodeMode == 0x6) { // CLD
                            registers.P &= ~FLAG_D; // Clear BCD flag
                        } else {    // CPY
                            value8 = readValue(addressingMode);
                            
                            // Store result in a larger variable to determine carry
                            svalue16 = static_cast<int8_t>(registers.Y) - static_cast<int8_t>(value8);
                            
                            if (svalue16 < 0)   // Check negative
                                registers.P |= FLAG_N;
                            else
                                registers.P &= ~FLAG_N;
                            
                            if (svalue16 == 0)  // Check zero
                                registers.P |= FLAG_Z;
                            else
                                registers.P &= ~FLAG_Z;
                            
                            if (svalue16 < value8) { // Check carry
                                registers.P &= ~FLAG_C;
                            } else {
                                registers.P |= FLAG_C;
                            }
                        }
                        break;
                    case 0x7:   // CPX
                        if (opcodeMode == 0x2) {    // INX instead of CPX
                            registers.X++;
                            
                            setNZ(registers.X);
                        } else if (opcodeMode == 0x6) { // SED
                            registers.P |= FLAG_D;  // Set BCD flag
                        } else {    // CPX
                            value8 = readValue(addressingMode);
                            
                            // Store result in a larger variable to determine carry
                            svalue16 = static_cast<int8_t>(registers.X) - static_cast<int8_t>(value8);
                            
                            setNZ(svalue16);
                            
                            /* if ((svalue16 & 0x100) == 0) {  // Check carry
                             registers.P &= ~FLAG_C;
                             } else {
                             registers.P |= FLAG_C;
                             } */
                            
                            if (registers.A < value8) { // Check carry
                                registers.P &= ~FLAG_C;
                            } else {
                                registers.P |= FLAG_C;
                            }
                        }
                        break;
                }
            }
            break;
    }
    
    // Apply changes to PC
    registers.PC = newPC;
    
    // CPU timing
    high_resolution_clock::time_point end_time = high_resolution_clock::now();
    nanoseconds exec_time = duration_cast<nanoseconds>(end_time - start_time);
    
    // CPU takes 1000 nanoseconds per cycle

    high_resolution_clock::time_point goal_time = start_time + nanoseconds(cycles * 1000);
    while (start_time + exec_time < goal_time) {
        //this_thread::sleep_for(nanoseconds(1000));    // Usually sleeps way too long
        exec_time = duration_cast<nanoseconds>(high_resolution_clock::now() - start_time);
    }
    
    // Set start time to when it should have started
    start_time = goal_time;
}

/**
 * Triggers an interrupt.
 */
void MOS6502::interrupt(Interrupt type) {
    pendingInterrupt = type;
}

/**
 * Resets the CPU.
 */
void MOS6502::reset() {
    registers.P |= FLAG_I;  // Set interrupt flag
    interrupt(INT_RESET);   // Trigger a RESET interrupt
}

/**
 * Triggers an IRQ.
 */
void MOS6502::irq() {
    interrupt(INT_IRQ);
}

/**
 * Triggers an NMI.
 */
void MOS6502::nmi() {
    interrupt(INT_NMI);
}

/**
 * Returns the number of cycles consumed by a particular opcode.
 * Cycle counts are stored in an array for easy lookup.
 */
uint_fast8_t MOS6502::getCycles(uint8_t opcode) {
    static const uint_fast8_t cycleMap[] = {7, 6, 0, 8, 3, 3, 5, 5, 3, 2, 2, 2, 4, 4, 6, 6, 3, 5, 0, 8, 4, 4, 6, 6, 2, 4, 2,
        7, 4, 4, 7, 7, 6, 6, 0, 8, 3, 3, 5, 5, 4, 2, 2, 2, 4, 4, 6, 6, 2, 5, 0, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7, 6, 6,
        0, 8, 3, 3, 5, 5, 3, 2, 2, 2, 3, 4, 6, 6, 3, 5, 0, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7, 6, 6, 0, 8, 3, 3, 5, 5, 4,
        2, 2, 2, 5, 4, 6, 6, 2, 5, 0, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7, 2, 6, 2, 6, 3, 3, 3, 3, 2, 2, 2, 2, 4, 4, 4, 4,
        3, 6, 0, 6, 4, 4, 4, 4, 2, 5, 2, 5, 5, 5, 5, 5, 2, 6, 2, 6, 3, 3, 3, 3, 2, 2, 2, 2, 4, 4, 4, 4, 2, 5, 0, 5, 4, 4, 4,
        4, 2, 4, 2, 4, 4, 4, 4, 4, 2, 6, 2, 8, 3, 3, 5, 5, 2, 2, 2, 2, 4, 4, 6, 6, 3, 5, 0, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4,
        7, 7, 2, 6, 2, 8, 3, 3, 5, 5, 2, 2, 2, 2, 4, 4, 6, 6, 2, 5, 0, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7};
    
    return cycleMap[opcode];
}
