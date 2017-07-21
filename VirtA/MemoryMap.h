//
//  MemoryMap.h
//  Interface for MemoryMap
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

#ifndef MemoryMap_H
#define MemoryMap_H

#include <cstdint>

#include <vector>
#include <string>
#include <memory>

#include "Memory.h"
#include "RAM.h"
#include "ROM.h"
#include "MemoryInterface.h"

class MemoryMap : public Memory {
    RAM *ram;
    std::vector<std::unique_ptr<ROM>> roms;
    std::vector<MemoryInterface *> interfaces;

public:
    MemoryMap(uint_fast8_t ramSize, uint16_t himem);
    ~MemoryMap();
    
    void loadROM(uint16_t startAddress, std::string filename);
    void loadRAM(uint16_t startAddress, std::string filename);
    void registerInterface(MemoryInterface *interface);
    
    uint8_t readByte(uint16_t address);
    uint16_t readWord(uint16_t address);
    void writeByte(uint16_t address, uint8_t value);
    void writeWord(uint16_t address, uint16_t value);
    
    void dumpMonitor(uint16_t address, int length);
};

#endif /* MemoryMap_H */
