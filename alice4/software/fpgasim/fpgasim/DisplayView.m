//
//  DisplayView.m
//  fpgasim
//
//  Created by Lawrence Kesteloot on 5/13/16.
//  Copyright © 2016 Earwig, Inc. All rights reserved.
//

#import "DisplayView.h"

@interface DisplayView () {
    NSDate *previousFrame;
    int fps;
    NSMutableDictionary *textAttributes;
}

@end

@implementation DisplayView

- (void)awakeFromNib {
    _rep = nil;
    previousFrame = nil;
    fps = 0;

    textAttributes = [NSMutableDictionary dictionary];
    [textAttributes setObject:[NSFont systemFontOfSize:24] forKey:NSFontAttributeName];
    [textAttributes setObject:[NSColor whiteColor] forKey:NSForegroundColorAttributeName];
}

// So that we can get key events.
- (BOOL)acceptsFirstResponder {
    return YES;
}

- (void)mouseDown:(NSEvent *)event {
    [self.delegate gotViewEvent:event];
}

- (void)mouseUp:(NSEvent *)event {
    [self.delegate gotViewEvent:event];
}

- (void)keyDown:(NSEvent *)event {
    [self.delegate gotViewEvent:event];
}

- (void)keyUp:(NSEvent *)event {
    [self.delegate gotViewEvent:event];
}

- (void)setRep:(NSBitmapImageRep *)rep {
    _rep = rep;

    // Compute FPS.
    if (previousFrame != nil) {
	NSTimeInterval elapsed = -[previousFrame timeIntervalSinceNow];
	if (elapsed <= 0) {
	    fps = 0;
	} else {
	    fps = 1/elapsed;
	}
    }
    previousFrame = [NSDate date];

    // Tell the system to redraw the window.
    [self setNeedsDisplay:YES];
}

- (void)drawRect:(NSRect)rect {
    [super drawRect:rect];

    [NSGraphicsContext saveGraphicsState];
    if (self.rep != nil) {
	[self.rep draw];
    }

    if (fps != 0) {
	[[NSString stringWithFormat:@"%d FPS", fps] drawAtPoint:NSMakePoint(10, 10) withAttributes:textAttributes];
    }
    [NSGraphicsContext restoreGraphicsState];
}

@end
