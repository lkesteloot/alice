//
//  DisplayView.m
//  fpgasim
//
//  Created by Lawrence Kesteloot on 5/13/16.
//  Copyright © 2016 Earwig, Inc. All rights reserved.
//

#import "DisplayView.h"

@interface DisplayView ()

@property (nonatomic) NSImage *image;

@end

@implementation DisplayView

- (void)awakeFromNib {
    NSLog(@"awakeFromNib");

    _image = [[NSImage alloc] initWithSize:NSMakeSize(800, 600)];
}

- (void)drawRect:(NSRect)rect {
    [super drawRect:rect];
    NSLog(@"drawRect");

    NSBezierPath *bezPath = [NSBezierPath bezierPath];

    [self.image lockFocus];
    [[NSColor colorWithRed:0 green:1 blue:1 alpha:1.0] set];
    [bezPath moveToPoint:NSMakePoint(10,10)];
    [bezPath lineToPoint:NSMakePoint(10,190)];
    [bezPath lineToPoint:NSMakePoint(100,100)];
    [bezPath stroke];
    [self.image unlockFocus];

    [self.image drawInRect:NSMakeRect(0, 0, 800, 600)];
}

@end
