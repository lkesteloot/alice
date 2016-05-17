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
- (void)qdevice:(long)device;
- (void)tie:(long)button val1:(long)val1 val2:(long)val2;

// Queue handling.
- (void)addEvent:(long)device value:(short)value mouseX:(short)x mouseY:(short)y;
// Returns YES if there's an event, NO otherwise.
- (BOOL)getEvent:(long *)device value:(short *)value;

@end
