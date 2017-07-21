//
//  RAM.cpp
//  Implementation of RAM
//  Simulates a system's random access memory
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

#include <algorithm>
#include <fstream>

#include <cassert>
#include <cstdint>

#include "RAM.h"

using namespace std;

/**
 * Initializes internal storage for the memory
 */
RAM::RAM(uint_fast8_t kb, uint16_t himem) : himem(himem) {
    // Size of memory must be a power of two
    assert(((kb != 0) && !(kb & (kb - 1))));
    
    // Compute and initialize memory
    size = kb * 1024;
    memory = new uint8_t[size];
}

/**
 * Frees the internal storage for the memory
 */
RAM::~RAM() {
    // Free the memory
    delete memory;
}

/**
 * Reads a byte from memory
 */
uint8_t RAM::readByte(uint16_t address) {
    // Check whether the range is valid
    if (address >= size) {
        // Check for disjoint memory segment above 1000
        if (!((himem > 0 && size > 0x1000) && (address < himem + size - 0x1000))) {
            return 0;
        }
    }
    
    return memory[address];
}

/**
 * Writes a byte to memory
 */
void RAM::writeByte(uint16_t address, uint8_t value) {
    // Check whether the range is valid
    if (address >= size) {
        // Check for disjoint memory segment above 1000
        if (!((himem > 0 && size >= 0x1000) && (address < himem + size - 0x1000))) {
            return;
        }
    }
    
    // Set the value
    memory[address] = value;
}

void RAM::loadFile(uint16_t startAddress, string filename) {
    // Create a stream pointing to the file
    ifstream input(filename, std::ios::binary | std::ios::ate);
    streamsize size = input.tellg();
    input.seekg(0, std::ios::beg);
    
    // Read and close the file
    input.read(reinterpret_cast<char *>(memory + startAddress), size);
    input.close();
}
