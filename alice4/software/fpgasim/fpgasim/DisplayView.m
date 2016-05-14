//
//  DisplayView.m
//  fpgasim
//
//  Created by Lawrence Kesteloot on 5/13/16.
//  Copyright © 2016 Earwig, Inc. All rights reserved.
//

#import "DisplayView.h"

@implementation DisplayView

- (void)awakeFromNib {
    _image = nil;
}

- (void)setImage:(NSImage *)image {
    _image = image;
    [self setNeedsDisplay:YES];
}

- (void)drawRect:(NSRect)rect {
    [super drawRect:rect];
    if (self.image != nil) {
	[self.image drawInRect:NSMakeRect(0, 0, self.image.size.width, self.image.size.height)];
    }
}

@end
