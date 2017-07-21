 //
//  MainViewController.m
//  Implementation of the view for the emulated display
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

#include <OpenGL/gl.h>

#import "MainViewController.h"
#import "Emulator.h"

@implementation MainViewController
{
    // emulation
    Emulator *_emulator;
    
    // display timer
    NSTimer *animationTimer;
}

/**
 * Sets up the view and starts the emulation
 */
- (void) prepareOpenGL
{
    _emulator = [[Emulator alloc] initWithViewController:self];
}

- (void) reshape
{
    NSRect bounds = [self bounds];
    
    [_emulator reshape:bounds];
}

/**
 * Draws a frame from the emulation
 */
- (void) drawRect: (NSRect) bounds {
    glClear(GL_COLOR_BUFFER_BIT);
    
    [_emulator render:bounds];
    
    glFlush();

    if (![NSGraphicsContext currentContextDrawingToScreen]) {
        NSMutableDictionary *attr = [[NSMutableDictionary alloc] init];
        [attr setValue:[NSFont fontWithName:@"Monaco" size:20] forKey:NSFontAttributeName];
        [attr setValue:[NSColor blackColor] forKey:NSForegroundColorAttributeName];
        
        NSString *outputCharacters = [_emulator getCharacters];
        NSString *output = [NSString stringWithFormat:@"%@", outputCharacters];
        [output drawInRect:bounds withAttributes:attr];
    }
}

/**
 * Triggers a display update
 */
- (void) updateView
{
    [self setNeedsDisplay: YES];
}

/**
 * Tells the operating system that we accept being first responder
 */
- (BOOL) acceptsFirstResponder
{
    return YES;
}

/**
 * Handles key presses
 */
- (void) keyDown: (NSEvent*) event
{
    [_emulator keyInput:[event characters]];
}

/**
 * Prints the view
 */
- (void) print:(id) sender {
    [[NSPrintOperation printOperationWithView:self] runOperation];
}

/**
 * Triggers a reset
 */
- (IBAction) resetSystem: (id) sender {
    [_emulator reset];
}

@end
