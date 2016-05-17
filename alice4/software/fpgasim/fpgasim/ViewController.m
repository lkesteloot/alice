//
//  ViewController.m
//  fpgasim
//
//  Created by Lawrence Kesteloot on 5/12/16.
//  Copyright © 2016 Earwig, Inc. All rights reserved.
//

#import "ViewController.h"
#import "DisplayImage.h"
#import "EventQueue.h"
#import "device.h"

@interface ViewController ()

@property (nonatomic) Server *server;
@property (nonatomic) EventQueue *eventQueue;
@property (nonatomic) DisplayImage *frontBuffer;
@property (nonatomic) DisplayImage *backBuffer;

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];

    DisplayView *displayView = (DisplayView *) self.view;
    displayView.delegate = self;

    _server = [[Server alloc] initWithDelegate:self];
    _eventQueue = [[EventQueue alloc] init];
    _frontBuffer = [[DisplayImage alloc] init];
    _backBuffer = [[DisplayImage alloc] init];

    // So that it's displayed:
    [self swapBuffers];
}

// For DisplayViewDelegate:
- (void)gotViewEvent:(NSEvent *)event {
    long device;
    short value;

    switch (event.type) {
	case NSLeftMouseDown:
	    device = LEFTMOUSE;
	    value = 1;
	    break;

	case NSLeftMouseUp:
	    device = LEFTMOUSE;
	    value = 0;
	    break;
	    
	case NSRightMouseDown:
	    device = RIGHTMOUSE;
	    value = 1;
	    break;

	case NSRightMouseUp:
	    device = RIGHTMOUSE;
	    value = 0;
	    break;

	default:
	    device = 0;
	    value = 0;
	    break;
    }

    if (device != 0) {
	[_eventQueue addEvent:device value:value mouseX:event.locationInWindow.x mouseY:event.locationInWindow.y];
    }
}

// For ServerDelegate:
- (void)winOpenWithTitle:(NSString *)title {
    [_eventQueue reset];
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

// For ServerDelegate:
- (void)qdevice:(long)device {
    [_eventQueue qdevice:device];
}

// For ServerDelegate:
- (void)tie:(long)button val1:(long)val1 val2:(long)val2 {
    [_eventQueue tie:button val1:val1 val2:val2];
}

// For ServerDelegate:
- (BOOL)getEvent:(long *)device value:(short *)value {
    return [_eventQueue getEvent:device value:value];
}

@end
