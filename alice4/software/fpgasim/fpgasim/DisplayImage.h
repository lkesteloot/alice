//
//  DisplayImage.h
//  fpgasim
//
//  Created by Lawrence Kesteloot on 5/13/16.
//  Copyright © 2016 Earwig, Inc. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>
#import "basic_types.h"

@interface DisplayImage : NSObject

@property (nonatomic,readonly) NSBitmapImageRep *rep;

- (void)clear:(vec3ub)color;
- (void)zclear;
- (void)triangle:(screen_vertex *)vs enableZbuffer:(BOOL)enableZbuffer pattern:(uint16_t *)pattern;
- (void)logAndClearStats;

@end
