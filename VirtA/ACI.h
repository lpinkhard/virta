//
//  ACI.h
//  Interface for ACI
//
//  Created by Lionel Pinkhard on 2017/07/18.
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

#ifndef ACI_H
#define ACI_H

#include <cstdint>

#include <string>

#ifdef _APPLE_
#include <CoreAudio/CoreAudio.h>
#include <AudioUnit/AudioUnit.h>
#endif

#include "MemoryInterface.h"
#include "ROM.h"

class ACI : public MemoryInterface {
    bool flipflop;
    ROM *rom;
    
    bool outputInit;
    bool outputReady;

#ifdef _APPLE_
    AudioComponentInstance outputInstance;
    AURenderCallbackStruct *outputCallback;
#endif
    
    int16_t outputValue;
    int16_t inputValue;
    
    int16_t readAudio();
    void writeAudio(int16_t value);
    
public:
    ACI(uint16_t startAddress, std::string romfile);
    ~ACI();
    
#ifdef _APPLE_
    static OSStatus playAudioCallback(void *inRefCon, AudioUnitRenderActionFlags *ioActionFlags,
                                      const AudioTimeStamp *inTimeStamp, UInt32 inBusNumber,
                                      UInt32 inNumberFrames, AudioBufferList *ioData);
    OSStatus playAudio(AudioUnitRenderActionFlags *ioActionFlags,
                       const AudioTimeStamp *inTimeStamp, UInt32 inBusNumber,
                       UInt32 inNumberFrames, AudioBufferList *ioData);
#endif
    
    uint8_t readByte(uint16_t address);
    void writeByte(uint16_t address, uint8_t value);
};

#endif /* ACI_H */

#endif
