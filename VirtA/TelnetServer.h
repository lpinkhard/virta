//
//  TelnetServer.h
//  Interface for TelnetServer
//
//  Created by Lionel Pinkhard on 2017/07/15.
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

#ifndef TelnetServer_H
#define TelnetServer_H

#include "ASCIIKeyboard.h"
#include "Terminal.h"

#include <memory>
#include <mutex>
#include <thread>
#include <vector>

class TelnetServer {
    std::shared_ptr<ASCIIKeyboard> input;
    std::shared_ptr<Terminal> output;
    const char *port;
    bool stopping;
    std::vector<int> sockets;
    std::mutex *socketsMutex;
    std::thread serverThread;
    
    void process();
    
public:
    TelnetServer(std::shared_ptr<ASCIIKeyboard> input, std::shared_ptr<Terminal> output, const char *port);
    void start();
    void stop();
};

#endif /* TelnetServer_H */
