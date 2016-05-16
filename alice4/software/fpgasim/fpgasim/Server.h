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

- (void)setWindowTitle:(NSString *)title;
- (void)clear:(vec3ub)color;
- (void)swapBuffers;
- (void)triangle:(screen_vertex *)v;
- (NSPoint)getMousePosition;

@end

@interface Server : NSObject<NSStreamDelegate>

- (id)initWithDelegate:(id<ServerDelegate>)delegate;

@end
