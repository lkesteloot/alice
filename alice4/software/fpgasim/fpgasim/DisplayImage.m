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
#define BYTES_PER_PIXEL 4
#define STRIDE (WIDTH*4)
#define TILE 50
#define COCOA_TRIANGLE 0
#define NUM_VERTICES 3

@interface DisplayImage () {
    NSBitmapImageRep *rep;
    unsigned char *data;
    NSGraphicsContext *context;
}

@end

@implementation DisplayImage

- (id)init {
    self = [super init];

    if (self) {
	data = (unsigned char *) malloc(WIDTH*HEIGHT*4);
	rep = [self makeRepFromData];
	context = [NSGraphicsContext graphicsContextWithBitmapImageRep:rep];

	// We'll eventually want to remove this, since the hardware won't do it:
	[self fillCheckerboard];
    }

    return self;
}

- (NSBitmapImageRep *)rep {
    return [self makeRepFromData];
}

- (NSBitmapImageRep *)makeRepFromData {
    return [[NSBitmapImageRep alloc]
	    initWithBitmapDataPlanes:&data
	    pixelsWide:WIDTH
	    pixelsHigh:HEIGHT
	    bitsPerSample:8
	    samplesPerPixel:3
	    hasAlpha:NO
	    isPlanar:NO
	    colorSpaceName:NSDeviceRGBColorSpace
	    bitmapFormat:0
	    bytesPerRow:0
	    bitsPerPixel:BYTES_PER_PIXEL*8];
}

- (NSColor *)colorFromBuffer:(vec3ub)color {
    return [NSColor colorWithRed:color[0]/255.0 green:color[1]/255.0 blue:color[2]/255.0 alpha:1.0];
}

- (void)fillCheckerboard {
    [NSGraphicsContext setCurrentContext:context];
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
    [NSGraphicsContext setCurrentContext:context];
    NSRect rect = NSMakeRect(0, 0, WIDTH, HEIGHT);
    [[self colorFromBuffer:color] set];
    NSRectFill(rect);
}

// Returns on which side of the line (a,b) is the vertex (x,y).
int orientation(screen_vertex *a, screen_vertex *b, int x, int y) {
    return (b->x - a->x)*(y - a->y) - (b->y - a->y)*(x - a->x);
}

// Returns whether this edge is the top (horizontal) edge or a left
// edge of the triangle, as defined by D3D11.
// https://msdn.microsoft.com/en-us/library/windows/desktop/cc627092(v=vs.85).aspx#Triangle
//
// Since our vertices are always in counter-clockwise order, we can
// make some assumptions here.
bool isTopLeft(screen_vertex *a, screen_vertex *b) {
    if (a->y == b->y) {
	// If it's horizontal, then it's the top one if it's going left.
	return a->x < b->x;
    }

    // Else it's left if it's going down.
    return a->y > b->y;
}

- (void)triangle:(screen_vertex *)vs {
    if (/* DISABLES CODE */ (NO)) {
	NSLog(@"Triangle: (%d,%d), (%d,%d), (%d,%d)",
	      vs[0].x, vs[0].y,
	      vs[1].x, vs[1].y,
	      vs[2].x, vs[2].y);
    }

#if COCOA_TRIANGLE
    @autoreleasepool {
	[NSGraphicsContext setCurrentContext:context];
	NSBezierPath *path = [NSBezierPath bezierPath];
	[path moveToPoint:NSMakePoint(vs[0].x, vs[0].y)];
	[path lineToPoint:NSMakePoint(vs[1].x, vs[1].y)];
	[path lineToPoint:NSMakePoint(vs[2].x, vs[2].y)];
	[path closePath];
	[[NSColor colorWithRed:v[0].r/255.0 green:v[0].g/255.0 blue:v[0].b/255.0 alpha:v[0].a/255.0] set];
	[path fill];
    }
#else
    // Find bounding box of triangle on screen.
    int minX = WIDTH - 1;
    int minY = HEIGHT - 1;
    int maxX = 0;
    int maxY = 0;
    for (int i = 0; i < NUM_VERTICES; i++) {
	screen_vertex *v = &vs[i];

	// Temporary: Invert screen because libgl is giving us 0,0 in lower-left, but it's upper-left for us.
	v->y = HEIGHT - 1 - v->y;

	if (v->x < minX) minX = v->x;
	if (v->x > maxX) maxX = v->x;
	if (v->y < minY) minY = v->y;
	if (v->y > maxY) maxY = v->y;
    }

    // Reverse triangle if necessary.
    if (orientation(&vs[0], &vs[1], vs[2].x, vs[2].y) < 0) {
	screen_vertex tmp = vs[0];
	vs[0] = vs[1];
	vs[1] = tmp;
    }

    // Set up edge equations.
    int x01 = vs[0].y - vs[1].y;
    int y01 = vs[1].x - vs[0].x;
    int x12 = vs[1].y - vs[2].y;
    int y12 = vs[2].x - vs[1].x;
    int x20 = vs[2].y - vs[0].y;
    int y20 = vs[0].x - vs[2].x;
    int w0_row = orientation(&vs[1], &vs[2], minX, minY);
    int w1_row = orientation(&vs[2], &vs[0], minX, minY);
    int w2_row = orientation(&vs[0], &vs[1], minX, minY);

    // The comparison value.
    int bias0 = isTopLeft(&vs[1], &vs[2]) ? 0 : 1;
    int bias1 = isTopLeft(&vs[2], &vs[0]) ? 0 : 1;
    int bias2 = isTopLeft(&vs[0], &vs[1]) ? 0 : 1;

    unsigned char *row_pixel = &data[(minY*WIDTH + minX)*BYTES_PER_PIXEL];
    for (int y = minY; y <= maxY; y++) {
	unsigned char *p = row_pixel;
	int w0 = w0_row;
	int w1 = w1_row;
	int w2 = w2_row;

	for (int x = minX; x <= maxX; x++) {
	    if (w0 >= bias0 && w1 >= bias1 && w2 >= bias2) {
		p[0] = vs[0].r;
		p[1] = vs[0].g;
		p[2] = vs[0].b;
	    }

	    w0 += x12;
	    w1 += x20;
	    w2 += x01;
	    p += BYTES_PER_PIXEL;
	}

	w0_row += y12;
	w1_row += y20;
	w2_row += y01;
	row_pixel += STRIDE;
    }
#endif
}

@end
