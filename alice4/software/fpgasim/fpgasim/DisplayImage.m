//
//  DisplayImage.m
//  fpgasim
//
//  Created by Lawrence Kesteloot on 5/13/16.
//  Copyright © 2016 Earwig, Inc. All rights reserved.
//

#import "DisplayImage.h"

#define WIDTH 800
#define HEIGHT 600
#define TILE 50

@interface DisplayImage () {
    NSBitmapImageRep *rep;
    NSGraphicsContext *backContext;
}

@end

@implementation DisplayImage

- (id)init {
    self = [super init];

    if (self) {
	[self newBuffer];
    }

    return self;
}

- (void)newBuffer {
    rep = [[NSBitmapImageRep alloc]
	   initWithBitmapDataPlanes:NULL
	   pixelsWide:WIDTH
	   pixelsHigh:HEIGHT
	   bitsPerSample:8
	   samplesPerPixel:4
	   hasAlpha:YES
	   isPlanar:NO
	   colorSpaceName:NSDeviceRGBColorSpace
	   bitmapFormat:NSAlphaFirstBitmapFormat
	   bytesPerRow:0
	   bitsPerPixel:0];
    backContext = [NSGraphicsContext graphicsContextWithBitmapImageRep:rep];

    // We'll eventually want to remove this, since the hardware won't do it:
    [self fillCheckerboard];
}

- (NSColor *)colorFromBuffer:(vec3ub)color {
    return [NSColor colorWithRed:color[0]/255.0 green:color[1]/255.0 blue:color[2]/255.0 alpha:1.0];
}

// Returns new front buffer.
- (NSBitmapImageRep *)swapBuffers {
    // I shouldn't have to have an autorelease here. There should be one in the main loop,
    // which we're using for everything (graphics and network). Without this autoreleasepool,
    // we leak tons of memory.
    @autoreleasepool {
	NSBitmapImageRep *newFrontBuffer = rep;

	[self newBuffer];

	return newFrontBuffer;
    }
}

- (void)fillCheckerboard {
    [NSGraphicsContext setCurrentContext:backContext];
    NSRect rect = NSMakeRect(0, 0, WIDTH, HEIGHT);
    [[NSColor colorWithRed:.5 green:.5 blue:.5 alpha:1.0] set];
    NSRectFill(rect);

    [[NSColor colorWithRed:.6 green:.6 blue:.6 alpha:1.0] set];
    for (int y = 0; y < rect.size.height/TILE; y++) {
	for (int x = 0; x < rect.size.width/TILE; x++) {
	    if ((x + y) % 2 == 0) {
		NSRect tile = NSMakeRect(x*TILE, y*TILE, TILE, TILE);
		NSRectFill(tile);
	    }
	}
    }
}

- (void)clear:(vec3ub)color {
    [NSGraphicsContext setCurrentContext:backContext];
    NSRect rect = NSMakeRect(0, 0, WIDTH, HEIGHT);
    [[self colorFromBuffer:color] set];
    NSRectFill(rect);
}

- (void)triangle:(screen_vertex *)v {
    @autoreleasepool {
    [NSGraphicsContext setCurrentContext:backContext];
    NSBezierPath *path = [NSBezierPath bezierPath];
    if (/* DISABLES CODE */ (NO)) {
	NSLog(@"Triangle: (%d,%d), (%d,%d), (%d,%d)",
	      v[0].x, v[0].y,
	      v[1].x, v[1].y,
	      v[2].x, v[2].y);
    }
    [path moveToPoint:NSMakePoint(v[0].x, v[0].y)];
    [path lineToPoint:NSMakePoint(v[1].x, v[1].y)];
    [path lineToPoint:NSMakePoint(v[2].x, v[2].y)];
    [path closePath];
    [[NSColor colorWithRed:v[0].r/255.0 green:v[0].g/255.0 blue:v[0].b/255.0 alpha:v[0].a/255.0] set];
    [path fill];
    }
}

@end
