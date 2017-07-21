//
//  Display.cpp
//  Provides properties for different types of displays
//
//  Created by Lionel Pinkhard on 2017/07/13.
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

#include "Display.h"

Display Display::getDisplay(DisplayType displayType) {
    Display display;
    
    display.color.r = 0xf8;
    display.color.g = 0xf8;
    display.color.b = 0xf8;
    
    display.bgColor.r = 0x16;
    display.bgColor.g = 0x16;
    display.bgColor.b = 0x16;
    
    switch (displayType) {
        case DISPLAY_NTSC_BLUE:     // Commodore PET blue
            display.color.r = 0xb2;
            display.color.g = 0xef;
            display.width = 584;
            display.height = 440;
            display.lines = 486;
            break;
        case DISPLAY_NTSC_GREEN:    // Green NTSC CRT monitor
            display.color.r = 0x0;
            display.color.b = 0xae;
            display.bgColor.r = 0x0;
            display.bgColor.b = 0x0c;
        case DISPLAY_NTSC_WHITE:    // White NTSC CRT TV/monitor
            display.width = 584;
            display.height = 440;
            display.lines = 486;
            break;
    }
    
    return display;
}
