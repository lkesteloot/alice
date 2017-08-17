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

@interface ViewController () {
    uint16_t *_pattern;
    BOOL _patternEnabled;
}

@property (nonatomic) Server *server;
@property (nonatomic) EventQueue *eventQueue;
@property (nonatomic) DisplayImage *frontBuffer;
@property (nonatomic) DisplayImage *backBuffer;
@property (nonatomic) BOOL zbufferEnabled;

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
    _pattern = (uint16_t *) malloc(sizeof(uint16_t)*16);
    [self resetState];

    // So that it's displayed:
    [self swapBuffers];
}

- (void)dealloc {
    free(_pattern);
}

- (void)resetState {
    [_eventQueue reset];
    self.zbufferEnabled = NO;
    _patternEnabled = NO;
}

// For DisplayViewDelegate:
- (void)gotViewEvent:(NSEvent *)event {
    uint32_t device;
    uint16_t value;

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

	case NSEventTypeKeyDown:
	case NSEventTypeKeyUp:
	    value = event.type == NSEventTypeKeyDown ? 1 : 0;

	    switch ([event keyCode]) {
		case 126:
		    device = UPARROWKEY;
		    break;

		case 125:
		    device = DOWNARROWKEY;
		    break;

		case 124:
		    device = RIGHTARROWKEY;
		    break;

		case 123:
		    device = LEFTARROWKEY;
		    break;

		case 53:
		    device = ESCKEY;
		    break;

		default:
		    NSLog(@"Unknown key code: %d", [event keyCode]);
		    device = 0;
		    value = 0;
		    break;
	    }
	    break;

	default:
	    NSLog(@"Unknown event: %04lx", (unsigned long)event.type);
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
    [self resetState];
    self.view.window.title = title;
}

// For ServerDelegate:
- (void)clear:(vec3ub)color {
    [self.backBuffer clear:color];
}

// For ServerDelegate:
- (void)swapBuffers {
    [self.backBuffer logAndClearStats];

    DisplayImage *tmp = self.backBuffer;
    self.backBuffer = self.frontBuffer;
    self.frontBuffer = tmp;

    DisplayView *displayView = (DisplayView *) self.view;
    displayView.rep = self.frontBuffer.rep;
}

// For ServerDelegate:
- (void)triangle:(screen_vertex *)v {
    [self.backBuffer triangle:v enableZbuffer:self.zbufferEnabled pattern:_patternEnabled ? _pattern : nil];
}

// For ServerDelegate:
- (NSPoint)getMousePosition {
    return [self.view.window mouseLocationOutsideOfEventStream];
}

// For ServerDelegate:
- (void)qdevice:(uint32_t)device {
    [_eventQueue qdevice:device];
}

// For ServerDelegate:
- (void)tie:(uint32_t)button val1:(uint32_t)val1 val2:(uint32_t)val2 {
    [_eventQueue tie:button val1:val1 val2:val2];
}

// For ServerDelegate:
- (BOOL)getEvent:(uint32_t *)device value:(uint16_t *)value {
    return [_eventQueue getEvent:device value:value];
}

// For ServerDelegate:
- (void)zbuffer:(BOOL)enable {
    self.zbufferEnabled = enable;
}

// For ServerDelegate:
- (void)zclear {
    [self.backBuffer zclear];
}

// For ServerDelegate:
- (void)setPattern:(uint16_t *)pattern {
    for (int i = 0; i < 16; i++) {
	_pattern[i] = pattern[i];
    }
}

// For ServerDelegate:
- (void)enablePattern {
    _patternEnabled = YES;
}

// For ServerDelegate:
- (void)disablePattern {
    _patternEnabled = NO;
}

@end
