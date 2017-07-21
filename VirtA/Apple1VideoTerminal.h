//
//  Apple1VideoTerminal.h
//  Interface for Apple1VideoTerminal class
//
//  Created by Lionel Pinkhard on 2017/07/12.
//  Copyright © 2017 Lionel Pinkhard. All rights reserved.
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

#ifndef Apple1VideoTerminal_H
#define Apple1VideoTerminal_H

#include <chrono>
#include <mutex>
#include <string>
#include <vector>

#include <cstdint>

#include "Display.h"
#include "Terminal.h"

class Apple1VideoTerminal: public Terminal {
    uint8_t *tempBuffer;
    uint8_t *displayData;
    
    std::vector<uint8_t> displayCharacters;
    std::vector<char> outputCharacters;
    std::vector<int> sockets;
    std::mutex displayCharactersMutex;
    std::mutex socketsMutex;
    bool displayReady;
    uint8_t cursorBlink;
    uint8_t cursorRow;
    uint8_t cursorColumn;
    
    Display display;
    
    void update();
    void getCharacter(uint8_t index, uint8_t *character);
    
    void writeSockets(uint8_t value);
    void (*callback)();
    
public:
    Apple1VideoTerminal(void (*callback)());
    ~Apple1VideoTerminal();
    
    uint8_t read();
    void write(uint8_t value);
    
    void render(int width, int height);
    void reshape(int width, int height);
    void timer();
    float timerDuration();
    std::string getCharacters();
    
    void addSocket(int sock);
    std::mutex *getSocketsMutex();
};

#endif /* Apple1VideoTerminal_H */
