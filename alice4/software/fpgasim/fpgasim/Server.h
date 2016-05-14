//
//  Server.h
//  fpgasim
//
//  Created by Lawrence Kesteloot on 5/13/16.
//  Copyright © 2016 Earwig, Inc. All rights reserved.
//

#import <Foundation/Foundation.h>

@protocol ServerDelegate

- (void)setWindowTitle:(NSString *)title;
- (void)clear;
- (void)swapBuffers;

@end

@interface Server : NSObject<NSStreamDelegate>

- (id)initWithDelegate:(id<ServerDelegate>)delegate;

@end
