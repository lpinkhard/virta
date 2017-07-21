//
//  RAM.hpp
//  Interface for RAM
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

#ifndef RAM_H
#define RAM_H

#include <string>

#include <cstdint>

#include "Memory.h"

class RAM : public Memory {
    uint8_t *memory;
    uint_fast32_t size;
    uint16_t himem;    // Disjoint high memory area
    
public:
    RAM(uint_fast8_t kb, uint16_t himem);
    ~RAM();
    
    uint8_t readByte(uint16_t address);
    void writeByte(uint16_t address, uint8_t value);
    void loadFile(uint16_t address, std::string filename);
};

#endif /* RAM_H */
