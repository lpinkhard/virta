//
//  ROM.cpp
//  Implementation of ROM
//  Simulates a chunk of read only memory
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

#include <cassert>
#include <cstdint>

#include <fstream>
#include <algorithm>

#include "ROM.h"

using namespace std;

/**
 * Loads the ROM into internal storage
 */
ROM::ROM(uint16_t startAddress, string filename) : startAddress(startAddress) {
    // Create a stream pointing to the file
    ifstream input(filename, std::ios::binary | std::ios::ate);
    streamsize ssize = input.tellg();
    input.seekg(0, std::ios::beg);
    
    // Compute and initialize memory, checking bounds of addressable memory
    size = min(static_cast<uint32_t>(ssize), static_cast<uint32_t>(0x10000 - startAddress));
    memory = new uint8_t[size];

    // Read and close the file
    input.read(reinterpret_cast<char *>(memory), size);
    input.close();
    
    // ROM is not banked out by default
    bankedOut = false;
}

/**
 * Frees the internal storage for the memory
 */
ROM::~ROM() {
    // Free the memory
    delete memory;
}

/**
 * Reads a byte from memory
 */
uint8_t ROM::readByte(uint16_t address) {
    return memory[address - startAddress];
}

/**
 * Do nothing
 * It's not possible to write to ROM
 */
void ROM::writeByte(uint16_t address, uint8_t value) {
}

/**
 * Check whether an address is in the range of this ROM
 */
bool ROM::isInRange(uint16_t address) {
    if (address < startAddress)
        return false;
    if (address >= startAddress + size)
        return false;
    
    return true;
}

/**
 * Check whether the ROM is banked out
 */
bool ROM::isBankedOut() {
    return bankedOut;
}

/**
 * Bank out the ROM, making it inaccessible
 */
void ROM::bankOut() {
    bankedOut = true;
}

/**
 * Bank in the ROM, making it accessible
 */
void ROM::bankIn() {
    bankedOut = false;
}
