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

@interface DisplayImage ()

@property (nonatomic) NSImage *image;

@end

@implementation DisplayImage

- (id)init {
    self = [super init];

    if (self) {
	_image = nil;
	[self swapBuffers];
    }

    return self;
}

- (NSImage *)swapBuffers {
    NSImage *oldImage = self.image;

    self.image = [[NSImage alloc] initWithSize:NSMakeSize(WIDTH, HEIGHT)];
    [self fillCheckerboard];

    return oldImage;
}

- (void)fillCheckerboard {
    [self.image lockFocus];
    NSRect rect = NSMakeRect(0, 0, self.image.size.width, self.image.size.height);
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
    [self.image unlockFocus];
}

- (void)clear {
    [self.image lockFocus];
    NSRect rect = NSMakeRect(0, 0, self.image.size.width, self.image.size.height);
    [[NSColor colorWithRed:1 green:0 blue:0 alpha:1.0] set];
    NSRectFill(rect);
    [self.image unlockFocus];
}

@end
