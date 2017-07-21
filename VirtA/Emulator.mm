//
//  Emulator.mm
//  Implementation for Emulator
//  Simulates a complete system
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

#import "Emulator.h"
#import "MainViewController.h"

using namespace std;
using namespace chrono;

static MainViewController *_viewController;

void displayCallback() {
    [_viewController updateView];
}

@implementation Emulator
{
    shared_ptr<MemoryMap> memoryMap;
    shared_ptr<CPU> cpu;
    shared_ptr<ASCIIKeyboard> keyboard;
    shared_ptr<Terminal> terminal;
    shared_ptr<VideoOutput> output;
    shared_ptr<TelnetServer> telnetServer;
    MemoryInterface *io;
    NSTimer *displayTimer;

#ifdef DEBUG
    shared_ptr<VideoMemory> videoMemory;
    ACI *aci;
#endif

}

/**
 * Triggers the display timer to update the cursor
 */
- (void) displayTimerTrigger: (NSTimer *) timer {
    output->timer();
}

/**
 * Prepares the view
 */
- (id) initWithViewController: (MainViewController *) viewController {
    self = [super init];
    _viewController = viewController;
    
    if (self) {
        // Apple I emulation
        memoryMap = shared_ptr<MemoryMap>(new MemoryMap(8, 0xe000));
        
        // wozmon.rom assembled from Jeff Tranter's code at https://github.com/jefftranter/6502/tree/master/asm/wozmon
        // Original code by Stephen Wozniak (http://www.woz.org)
        NSString *wmPath = [[NSBundle mainBundle] pathForResource:@"wozmon" ofType:@"rom"];
        memoryMap->loadROM(0xff00, [wmPath UTF8String]);
        
        keyboard = shared_ptr<ASCIIKeyboard>(new ASCIIKeyboard());
        
        terminal = shared_ptr<Terminal>(new Apple1VideoTerminal(displayCallback));
        output = terminal;
        
        io = new Motorola6820(0xd000, keyboard, terminal);
        memoryMap->registerInterface(io);
        
#ifdef DEBUG
        // wozaci.rom assembled from Jeff Tranter's code at https://github.com/jefftranter/6502/tree/master/asm/wozaci
        // Original code by Stephen Wozniak (http://www.woz.org)
        NSString *waPath = [[NSBundle mainBundle] pathForResource:@"wozaci" ofType:@"rom"];
        aci = new ACI(0xc000, [waPath UTF8String]); // ACI emulation doesn't work
        memoryMap->registerInterface(aci);
#endif
        
        telnetServer = shared_ptr<TelnetServer>(new TelnetServer(keyboard, terminal, "2121"));
        telnetServer->start();
        
        cpu = shared_ptr<CPU>(new MOS6502(memoryMap));
        cpu->start();
        
        displayTimer = [NSTimer scheduledTimerWithTimeInterval:output->timerDuration() target:self selector:@selector(displayTimerTrigger:) userInfo:nil repeats:YES];
    }
    
    return self;
}

/**
 * Shuts down the emulation
 */
- (void) dealloc {
    cpu->stop();
    telnetServer->stop();
    
#ifdef DEBUG
    delete aci;
#endif
    
    delete io;
}

/**
 * Accomodates window resizing
 */
- (void) reshape: (NSRect) bounds {
    output->reshape(bounds.size.width, bounds.size.height);
}

/**
 * Renders a frame of the emulation
 */
- (void) render: (NSRect) bounds {
    output->render(bounds.size.width, bounds.size.height);
}

/**
 * Handles keyboard input
 */
- (void) keyInput: (NSString *) characters {
    keyboard->textInput([characters UTF8String]);
}

/**
 * Simulates pressing the reset button
 */
- (void) reset {
    cpu->reset();
}

/**
 * Returns character buffer
 */
- (NSString *) getCharacters {
    return [NSString stringWithUTF8String:terminal->getCharacters().c_str()];
}

@end
