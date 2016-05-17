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
@property (nonatomic) DisplayImage *frontBuffer;
@property (nonatomic) DisplayImage *backBuffer;

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];

    _server = [[Server alloc] initWithDelegate:self];
    _frontBuffer = [[DisplayImage alloc] init];
    _backBuffer = [[DisplayImage alloc] init];

    // So that it's displayed:
    [self swapBuffers];
}

// For ServerDelegate:
- (void)setWindowTitle:(NSString *)title {
    self.view.window.title = title;
}

// For ServerDelegate:
- (void)clear:(vec3ub)color {
    [self.backBuffer clear:color];
}

// For ServerDelegate:
- (void)swapBuffers {
    DisplayImage *tmp = self.backBuffer;
    self.backBuffer = self.frontBuffer;
    self.frontBuffer = tmp;

    DisplayView *displayView = (DisplayView *) self.view;
    displayView.rep = self.frontBuffer.rep;
}

// For ServerDelegate:
- (void)triangle:(screen_vertex *)v {
    [self.backBuffer triangle:v];
}

// For ServerDelegate:
- (NSPoint)getMousePosition {
    return [self.view.window mouseLocationOutsideOfEventStream];
}

@end
