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

@property (nonatomic) uint32_t device;
@property (nonatomic) uint32_t val1;  // Optional for tie() command.
@property (nonatomic) uint32_t val2;

@end

@implementation QueuedDevice

- (id)init:(uint32_t)device val1:(uint32_t)val1 val2:(uint32_t)val2 {
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

@property (nonatomic) uint32_t device;
@property (nonatomic) uint16_t value;

@end

@implementation Event

- (id)init:(uint32_t)device value:(uint16_t)value {
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

- (QueuedDevice *)getQueuedDevice:(uint32_t)device {
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

- (void)qdevice:(uint32_t)device {
    QueuedDevice *queuedDevice = [self getQueuedDevice:device];
    if (queuedDevice == nil) {
	[_queuedDevices addObject:[[QueuedDevice alloc] init:device val1:0 val2:0]];
    }
}

- (void)tie:(uint32_t)button val1:(uint32_t)val1 val2:(uint32_t)val2 {
    QueuedDevice *queuedDevice = [self getQueuedDevice:button];
    if (queuedDevice == nil) {
	// Not sure if we're supposed to qdevice here.
	[_queuedDevices addObject:[[QueuedDevice alloc] init:button val1:val1 val2:val2]];
    } else {
	queuedDevice.val1 = val1;
	queuedDevice.val2 = val2;
    }
}

- (void)addEvent:(uint32_t)device value:(uint16_t)value mouseX:(uint16_t)x mouseY:(uint16_t)y {
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

- (BOOL)getEvent:(uint32_t *)device value:(uint16_t *)value {
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
