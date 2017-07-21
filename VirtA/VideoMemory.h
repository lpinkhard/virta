//
//  VideoMemory.h
//  Interface for VideoMemory
//
//  Not used at present
//
//  Created by Lionel Pinkhard on 2017/07/12.
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

#ifndef VideoMemory_H
#define VideoMemory_H

#include <cstdint>

#include "MemoryInterface.h"
#include "VideoOutput.h"

class VideoMemory: public MemoryInterface, public VideoOutput {
public:
    VideoMemory(uint16_t startAddress, uint_fast32_t size);
    virtual ~VideoMemory() { };
    virtual uint8_t readByte(uint16_t address) = 0;
    virtual void writeByte(uint16_t address, uint8_t value) = 0;
    virtual void timer() = 0;
    virtual float timerDuration() = 0;
    virtual void render(int width, int height) = 0;
    virtual void reshape(int width, int height) = 0;
    virtual void clear() = 0;
};
#endif /* VideoMemory_H */

#endif
