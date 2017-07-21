//
//  MemoryInterface.cpp
//  Implements a MemoryInterface
//  Simulates address bus connections for memory mapped devices
//  Subclassed by memory mapped devices
//
//  Created by Lionel Pinkhard on 2017/07/06.
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

#include "MemoryInterface.h"

/**
 * Initializes the bounds of the memory interface.
 */
MemoryInterface::MemoryInterface(uint16_t startAddress, uint_fast32_t size) : startAddress(startAddress), size(size) { }

/**
 * Check whether an address is in the range of this memory interface.
 */
bool MemoryInterface::isInRange(uint16_t address) {
    if (address < startAddress)
        return false;
    if (address >= startAddress + size)
        return false;
    
    return true;
}

/**
 * Sets the memory map this interface is serving.
 */
void MemoryInterface::setMemoryMap(MemoryMap *memoryMap) {
    this->memoryMap = memoryMap;
}

MemoryMap *MemoryInterface::getMemoryMap() {
    return memoryMap;
}

/**
 * Gets the start address of the memory segment.
 */
uint16_t MemoryInterface::getStartAddress() {
    return startAddress;
}
