//
//  DisplayView.m
//  fpgasim
//
//  Created by Lawrence Kesteloot on 5/13/16.
//  Copyright © 2016 Earwig, Inc. All rights reserved.
//

#import "DisplayView.h"

#define WIDTH 800
#define HEIGHT 600
#define TILE 50

@interface DisplayView ()

@property (nonatomic) NSImage *image;

@end

@implementation DisplayView

- (void)awakeFromNib {
    NSLog(@"awakeFromNib");

    _image = [self newDisplayImage];
}

- (void)drawRect:(NSRect)rect {
    [super drawRect:rect];
    NSLog(@"drawRect");

    [self.image drawInRect:NSMakeRect(0, 0, WIDTH, HEIGHT)];
}

- (NSImage *)newDisplayImage {
    NSImage *image = [[NSImage alloc] initWithSize:NSMakeSize(WIDTH, HEIGHT)];

    [self fillCheckerboard:image];

    return image;
}

- (void)fillCheckerboard:(NSImage *)image {
    [image lockFocus];
    NSRect rect = NSMakeRect(0, 0, image.size.width, image.size.height);
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
    [image unlockFocus];
}

@end
