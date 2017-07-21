//
//  MemoryMap.cpp
//  Implementation of MemoryMap
//  Simulates the entire memory map for a system
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

#include "MemoryMap.h"

#include <iostream>
#include <cassert>
#include <cstdint>

using namespace std;

/**
 * Initalizes a memory map for the computer. RAM size is specified in kilobytes.
 */
MemoryMap::MemoryMap(uint_fast8_t ramSize, uint16_t himem) {
    // Size of memory must be a power of two
    assert(((ramSize != 0) && !(ramSize & (ramSize - 1))));
    
    ram = new RAM(ramSize, himem);
}

/**
 * Frees the memory map.
 */
MemoryMap::~MemoryMap() {
    interfaces.clear();
    roms.clear();
    delete ram;
}

/**
 * Loads a ROM into the memory map.
 */
void MemoryMap::loadROM(uint16_t startAddress, string filename) {
    roms.push_back(unique_ptr<ROM>(new ROM(startAddress, filename)));
}

/**
 * Loads data into RAM.
 */
void MemoryMap::loadRAM(uint16_t startAddress, string filename) {
    ram->loadFile(startAddress, filename);
}

/**
 * Registers a memory watcher.
 */
void MemoryMap::registerInterface(MemoryInterface *interface) {
    interface->setMemoryMap(this);
    interfaces.push_back(interface);
}

/**
 * Reads a byte from RAM or ROM.
 */
uint8_t MemoryMap::readByte(uint16_t address) {
    // Check whether an interface needs to be notified
    for(auto const& interface: interfaces) {
        if (interface->isInRange(address)) {
            return interface->readByte(address);
        }
    }
    
    // Check whether address is occupied by a ROM
    for(auto const& rom: roms) {
        if (rom->isInRange(address)) {
            if (rom->isBankedOut()) {
                return ram->readByte(address);
            } else {
                return rom->readByte(address);
            }
        }
    }
    
    return ram->readByte(address);
}

/**
 * Reads a word from RAM or ROM.
 */
uint16_t MemoryMap::readWord(uint16_t address) {
    return static_cast<uint16_t>(readByte(address)) |
        (static_cast<uint16_t>(readByte((address + 1) & 0xffff)) << 8);
}

/**
 * Writes a byte to RAM.
 */
void MemoryMap::writeByte(uint16_t address, uint8_t value) {
    ram->writeByte(address, value);
    
    // Check whether a watcher needs to be notified
    for(auto const& interface: interfaces) {
        if (interface->isInRange(address)) {
            interface->writeByte(address, value);
        }
    }
}

/**
 * Writes a word to RAM
 */
void MemoryMap::writeWord(uint16_t address, uint16_t value) {
    writeByte(address, static_cast<uint8_t>(value & 0xff));
    writeByte(address, static_cast<uint8_t>((value & 0xff00) >> 8));
}

void MemoryMap::dumpMonitor(uint16_t address, int length) {
    int index = 0;
    cout << endl;
    for (uint16_t ptr = address; ptr < address + length; ptr++) {
        cout << hex << (int) readByte(ptr) << " ";
        index++;
        if (index % 20 == 0)
            cout << endl;
    }
    cout << endl;
}
