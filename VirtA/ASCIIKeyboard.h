//
//  ASCIIKeyboard.h
//  Interface for ASCIIKeyboard
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

#ifndef ASCIIKeyboard_H
#define ASCIIKeyboard_H

#include <cstdint>

#include <thread>

#include "Peripheral.h"

class ASCIIKeyboard: public Peripheral {
    uint8_t PDR;
    
public:
    ASCIIKeyboard();
    ~ASCIIKeyboard();
    
    uint8_t read();
    void write(uint8_t value);
    void keypress(uint8_t keycode);
    void textInput(const char *text);
};
#endif /* ASCIIKeyboard_H */
