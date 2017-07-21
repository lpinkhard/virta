//
//  Peripheral.cpp
//  Implementation of a generic Peripheral
//  Subclassed for specific implementations
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

#include "Peripheral.h"

/**
 * Sets up a peripheral
 */
Peripheral::Peripheral() {
    irq1 = false;
    irq2 = false;
}

/**
 * Checks and resets interrupt line 1
 */
bool Peripheral::interrupt1() {
    bool result = irq1;
    irq1 = false;
    return result;
}

/**
 * Checks and resets interrupt line 2
 */
bool Peripheral::interrupt2() {
    bool result = irq2;
    irq2 = false;
    return result;
}
