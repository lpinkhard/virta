//
//  CPU.cpp
//  Implementation of a generic CPU
//  Subclassed for specific implementations
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

#include <iostream>

#include "CPU.h"

using namespace std;

/**
 * Creates a CPU instance linked to a specific MemoryMap instance
 */
CPU::CPU(shared_ptr<MemoryMap> memoryMap) : memoryMap(memoryMap) {
    stopping = false;
}

/**
 * Loops to process instructions for the CPU
 */
void CPU::process() {
    while (!stopping) {
        execute();  // Pure virtual method to loop
    }
}

/**
 * Starts the CPU processing thread
 */
void CPU::start() {
    cpuThread = thread(&CPU::process, this);
}

/**
 * Stops the CPU processing thread
 */
void CPU::stop() {
    stopping = true;
    cpuThread.join();
}
