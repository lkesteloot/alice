//
//  Server.h
//  fpgasim
//
//  Created by Lawrence Kesteloot on 5/13/16.
//  Copyright © 2016 Earwig, Inc. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "basic_types.h"

@protocol ServerDelegate

- (void)winOpenWithTitle:(NSString *)title;
- (void)clear:(vec3ub)color;
- (void)swapBuffers;
- (void)triangle:(screen_vertex *)v;
- (NSPoint)getMousePosition;
- (void)qdevice:(long)device;
- (void)tie:(long)button val1:(long)val1 val2:(long)val2;
- (BOOL)getEvent:(long *)device value:(short *)value;

@end

@interface Server : NSObject<NSStreamDelegate>

- (id)initWithDelegate:(id<ServerDelegate>)delegate;

@end
