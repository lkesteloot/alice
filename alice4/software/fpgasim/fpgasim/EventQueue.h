//
//  EventQueue.h
//  fpgasim
//
//  Created by Lawrence Kesteloot on 5/17/16.
//  Copyright © 2016 Earwig, Inc. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface EventQueue : NSObject

- (id)init;

// Setup
- (void)reset;
- (void)qdevice:(uint32_t)device;
- (void)tie:(uint32_t)button val1:(uint32_t)val1 val2:(uint32_t)val2;

// Queue handling.
- (void)addEvent:(uint32_t)device value:(uint16_t)value mouseX:(uint16_t)x mouseY:(uint16_t)y;
// Returns YES if there's an event, NO otherwise.
- (BOOL)getEvent:(uint32_t *)device value:(uint16_t *)value;

@end
