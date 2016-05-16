//
//  ViewController.m
//  fpgasim
//
//  Created by Lawrence Kesteloot on 5/12/16.
//  Copyright © 2016 Earwig, Inc. All rights reserved.
//

#import "ViewController.h"
#import "DisplayImage.h"
#import "DisplayView.h"

@interface ViewController ()

@property (nonatomic) Server *server;
@property (nonatomic) DisplayImage *displayImage;

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];

    _server = [[Server alloc] initWithDelegate:self];
    _displayImage = [[DisplayImage alloc] init];

    [self swapBuffers];
}

// For ServerDelegate:
- (void)setWindowTitle:(NSString *)title {
    // This isn't right, we don't want all windows, but mainWindow is nil here.
    for (NSWindow *window in [NSApplication sharedApplication].windows) {
	window.title = title;
    }
}

// For ServerDelegate:
- (void)clear:(vec3ub)color {
    [self.displayImage clear:color];
}

// For ServerDelegate:
- (void)swapBuffers {
    NSImage *newFrontBuffer = [self.displayImage swapBuffers];

    DisplayView *displayView = (DisplayView *) self.view;
    displayView.image = newFrontBuffer;
}

// For ServerDelegate:
- (void)triangle:(screen_vertex *)v {
    [self.displayImage triangle:v];
}

// For ServerDelegate:
- (NSPoint)getMousePosition {
    NSWindow *window = [self view].window;
    return [window mouseLocationOutsideOfEventStream];
}

@end
