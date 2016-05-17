//
//  EventQueue.m
//  fpgasim
//
//  Created by Lawrence Kesteloot on 5/17/16.
//  Copyright © 2016 Earwig, Inc. All rights reserved.
//

#import "EventQueue.h"
#import "device.h"

@interface QueuedDevice : NSObject

@property (nonatomic) long device;
@property (nonatomic) long val1;  // Optional for tie() command.
@property (nonatomic) long val2;

@end

@implementation QueuedDevice

- (id)init:(long)device val1:(long)val1 val2:(long)val2 {
    self = [super init];

    if (self) {
	_device = device;
	_val1 = val1;
	_val2 = val2;
    }

    return self;
}

@end

// -----------------------------------------------------------------------------

@interface Event : NSObject

@property (nonatomic) long device;
@property (nonatomic) short value;

@end

@implementation Event

- (id)init:(long)device value:(short)value {
    self = [super init];

    if (self) {
	_device = device;
	_value = value;
    }

    return self;
}

@end

// -----------------------------------------------------------------------------

@interface EventQueue () {
    NSMutableArray *_queuedDevices;
    NSMutableArray *_events;
}

@end

@implementation EventQueue

- (id)init {
    self = [super init];

    if (self) {
	[self reset];
    }

    return self;
}

- (QueuedDevice *)getQueuedDevice:(long)device {
    // Linear search is fine, we won't have that many of these.
    for (QueuedDevice *queuedDevice in _queuedDevices) {
	if (queuedDevice.device == device) {
	    return queuedDevice;
	}
    }

    return nil;
}

- (void)reset {
    _queuedDevices = [NSMutableArray array];
    _events = [NSMutableArray array];
}

- (void)qdevice:(long)device {
    QueuedDevice *queuedDevice = [self getQueuedDevice:device];
    if (queuedDevice == nil) {
	[_queuedDevices addObject:[[QueuedDevice alloc] init:device val1:0 val2:0]];
    }
}

- (void)tie:(long)button val1:(long)val1 val2:(long)val2 {
    QueuedDevice *queuedDevice = [self getQueuedDevice:button];
    if (queuedDevice == nil) {
	// Not sure if we're supposed to qdevice here.
	[_queuedDevices addObject:[[QueuedDevice alloc] init:button val1:val1 val2:val2]];
    } else {
	queuedDevice.val1 = val1;
	queuedDevice.val2 = val2;
    }
}

- (void)addEvent:(long)device value:(short)value mouseX:(short)x mouseY:(short)y {
    QueuedDevice *queuedDevice = [self getQueuedDevice:device];
    if (queuedDevice != nil) {
	[_events addObject:[[Event alloc] init:device value:value]];

	// For tie().
	if (queuedDevice.val1 == MOUSEX) {
	    [_events addObject:[[Event alloc] init:MOUSEX value:x]];
	} else if (queuedDevice.val1 == MOUSEY) {
	    [_events addObject:[[Event alloc] init:MOUSEY value:y]];
	}

	// For tie().
	if (queuedDevice.val2 == MOUSEX) {
	    [_events addObject:[[Event alloc] init:MOUSEX value:x]];
	} else if (queuedDevice.val2 == MOUSEY) {
	    [_events addObject:[[Event alloc] init:MOUSEY value:y]];
	}
    }
}

- (BOOL)getEvent:(long *)device value:(short *)value {
    if (_events.count == 0) {
	return NO;
    } else {
	Event *event = [_events firstObject];
	*device = event.device;
	*value = event.value;
	[_events removeObjectAtIndex:0];
	return YES;
    }
}

@end
