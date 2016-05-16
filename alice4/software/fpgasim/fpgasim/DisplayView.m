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

- (void)setRep:(NSBitmapImageRep *)rep {
    _rep = rep;
    [self setNeedsDisplay:YES];

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
}

- (void)drawRect:(NSRect)rect {
    [super drawRect:rect];
    if (self.rep != nil) {
	[self.rep drawInRect:NSMakeRect(0, 0, self.rep.size.width, self.rep.size.height)];
    }

    if (fps != 0) {
	[[NSString stringWithFormat:@"%d FPS", fps] drawAtPoint:NSMakePoint(10, 10) withAttributes:textAttributes];
    }
}

@end
