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
- (void)qdevice:(uint32_t)device;
- (void)tie:(uint32_t)button val1:(uint32_t)val1 val2:(uint32_t)val2;
- (BOOL)getEvent:(uint32_t *)device value:(uint16_t *)value;
- (void)zbuffer:(BOOL)enable;
- (void)zclear;
- (void)setPattern:(uint16_t *)pattern; // Always 16 shorts.
- (void)enablePattern;
- (void)disablePattern;

@end

@interface Server : NSObject<NSStreamDelegate>

- (id)initWithDelegate:(id<ServerDelegate>)delegate;

@end
