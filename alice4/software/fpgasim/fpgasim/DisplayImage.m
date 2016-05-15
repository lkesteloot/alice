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
- (NSImage *)swapBuffers {
    NSImage *image = [[NSImage alloc] initWithSize:NSMakeSize(WIDTH, HEIGHT)];
    [image addRepresentation:rep];

    [self newBuffer];

    return image;
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
		NSRect tile = NSMakeRect(x*TILE, y*TILE, 50, 50);
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
    [NSGraphicsContext setCurrentContext:backContext];
    NSBezierPath *path = [NSBezierPath bezierPath];
    NSLog(@"Triangle: (%d,%d), (%d,%d), (%d,%d)",
	  v[0].x, v[0].y,
	  v[1].x, v[1].y,
	  v[2].x, v[2].y);
    [path moveToPoint:NSMakePoint(v[0].x, v[0].y)];
    [path lineToPoint:NSMakePoint(v[1].x, v[1].y)];
    [path lineToPoint:NSMakePoint(v[2].x, v[2].y)];
    [path closePath];
    [[NSColor colorWithRed:1 green:0 blue:0 alpha:.1] set];
    [path fill];
}

@end
