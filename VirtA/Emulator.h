//
//  Emulator.h
//  Interface for Emulator
//
//  Created by Lionel Pinkhard on 2017/07/14.
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

#ifdef __cplusplus
#include <iostream>
#include <memory>

#include <cstdio>

#include "MemoryMap.h"
#include "MOS6502.h"
#include "Motorola6820.h"
#include "PETIO.h"
#include "ASCIIKeyboard.h"
#include "VideoOutput.h"
#include "Terminal.h"
#include "Apple1VideoTerminal.h"
#include "VideoMemory.h"
#include "PETDisplay.h"
#include "TelnetServer.h"
#include "ACI.h"
#endif

#import <Foundation/Foundation.h>

@class MainViewController;

@interface Emulator : NSObject

- (id) initWithViewController: (MainViewController *) viewController;
- (void) render: (NSRect) bounds;
- (void) reshape: (NSRect) bounds;
- (void) keyInput: (NSString *) characters;
- (void) reset;
- (NSString *) getCharacters;

@end
