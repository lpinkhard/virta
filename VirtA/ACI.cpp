//
//  ACI.cpp
//  Implementation of an ACI
//  Emulates Apple Cassette Interface
//
//  Not working
//  Audio input not implemented
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

#include <string>

#include <cstdio>

#ifdef _APPLE_
#include <CoreAudio/CoreAudio.h>
#include <AudioUnit/AudioUnit.h>
#endif

#include "MemoryInterface.h"
#include "ACI.h"

using namespace std;

/**
 * Sets up ACI and audio system
 */
ACI::ACI(uint16_t startAddress, string romfile) : MemoryInterface(startAddress, 512) {
    flipflop = false;
    rom = new ROM(startAddress + 0x100, romfile);
    outputInit = false;
    outputReady = false;
    outputValue = 0;
    inputValue = 0;
    
#ifdef _APPLE_
    outputCallback = NULL;

    // Set up Core Audio
    AudioComponentDescription desc;
    desc.componentType = kAudioUnitType_Output;
    desc.componentSubType = kAudioUnitSubType_DefaultOutput;
    desc.componentFlags = 0;
    desc.componentFlagsMask = 0;
    desc.componentManufacturer = kAudioUnitManufacturer_Apple;
    
    AudioComponent outputComponent = AudioComponentFindNext(nullptr, &desc);
    if (outputComponent != NULL) {
        if (!AudioComponentInstanceNew(outputComponent, &outputInstance)) {
            outputInit = true;
        }
    }
    
    // Set up Audio Unit
    if (outputInit) {
        if (!AudioUnitInitialize(outputInstance)) {
            AudioStreamBasicDescription streamFormat = {0};
            streamFormat.mSampleRate = 16000;
            streamFormat.mFormatID = kAudioFormatLinearPCM;
            streamFormat.mFormatFlags = kAudioFormatFlagIsSignedInteger;
            streamFormat.mFramesPerPacket = 1;
            streamFormat.mChannelsPerFrame = 1;
            streamFormat.mBitsPerChannel = 16;
            streamFormat.mBytesPerPacket = 2;
            streamFormat.mBytesPerFrame = 2;
            
            outputCallback = new AURenderCallbackStruct();
            outputCallback->inputProc = &ACI::playAudioCallback;
            outputCallback->inputProcRefCon = this;
            
            if (!AudioUnitSetProperty (outputInstance, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Input, 0, &streamFormat, sizeof(streamFormat)))
            {
                if (!AudioUnitSetProperty(outputInstance, kAudioUnitProperty_SetRenderCallback, kAudioUnitScope_Input,
                                          0, outputCallback, sizeof(AURenderCallbackStruct)))
                {
                    if (!AudioOutputUnitStart(outputInstance))
                    {
                        outputReady = true;
                    }
                }
            }
        }
    }
#endif
    
}

/**
 * Frees up memory used by the ACI
 */
ACI::~ACI() {

#ifdef _APPLE_
    // Destroy the Audio Unit output callback
    if (outputCallback != NULL) {
        delete outputCallback;
        outputCallback = NULL;
    }
    
    // Shut down Audio Unit and Core Audio
    if (outputInit) {
        if (outputReady) {
            AudioOutputUnitStop(outputInstance);
            outputReady = false;
        }
        AudioComponentInstanceDispose(outputInstance);
        outputInit = false;
    }
#endif
    
    delete rom;
    rom = NULL;
}

#ifdef _APPLE_

/**
 * Renders a set of audio frames for Audio Unit
 */
OSStatus ACI::playAudio(AudioUnitRenderActionFlags *ioActionFlags,
                   const AudioTimeStamp *inTimeStamp, UInt32 inBusNumber,
                   UInt32 inNumberFrames, AudioBufferList *ioData) {
    
    // Iterate buffers
    for (int i = 0; i < ioData->mNumberBuffers; i++) {
        AudioBuffer buffer = ioData->mBuffers[i];
        int16_t *outputData = static_cast<int16_t *>(buffer.mData);
        
        // Iterate frames
        for (UInt32 j = 0; j < inNumberFrames; j++) {
            outputData[j] = outputValue;
        }
    }
    
    return noErr;
}

/**
 * Callback for Audio Unit
 */
OSStatus ACI::playAudioCallback(void *inRefCon, AudioUnitRenderActionFlags *ioActionFlags,
                                const AudioTimeStamp *inTimeStamp, UInt32 inBusNumber,
                                UInt32 inNumberFrames, AudioBufferList *ioData) {
    ACI *self = static_cast<ACI *>(inRefCon);
    return self->playAudio(ioActionFlags, inTimeStamp, inBusNumber, inNumberFrames, ioData);
}

#endif

/**
 * Reads a byte from an ACI memory address
 */
uint8_t ACI::readByte(uint16_t address) {
    // Toggle flip-flop and output audio
    flipflop = !flipflop;
    writeAudio(flipflop ? INT16_MAX : INT16_MIN);
    
    uint16_t maskedAddress = address & 0x1ff;
    if (maskedAddress >= 0x100) {
        return rom->readByte(address);          // Read from ACI ROM
    } else if (maskedAddress <= 0x7f) {
        return rom->readByte(address + 0x100);  // Read from ACI ROM
    } else {
        if (readAudio() > 0.0) {    // Audio operations
            return rom->readByte((address + 0x100) | (maskedAddress & ~1));
        } else {
            return rom->readByte((address + 0x100) | (maskedAddress));
        }
    }
    
    return 0;
}

/**
 * Writes a byte to an ACI memory address
 */
void ACI::writeByte(uint16_t address, uint8_t value) {
    // Toggle flip-flop and output audio
    flipflop = !flipflop;
    writeAudio(flipflop ? INT16_MAX : INT16_MIN);
}

/**
 * Reads the current value from the audio input
 */
int16_t ACI::readAudio() {
    return inputValue;
}

/**
 * Writes the current value for audio output
 */
void ACI::writeAudio(int16_t value) {
    outputValue = value;
}

#endif
