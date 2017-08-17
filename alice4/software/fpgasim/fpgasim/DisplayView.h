//
//  DisplayView.h
//  fpgasim
//
//  Created by Lawrence Kesteloot on 5/13/16.
//  Copyright © 2016 Earwig, Inc. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@protocol DisplayViewDelegate

- (void)gotViewEvent:(NSEvent *)event;

@end

@interface DisplayView : NSView

@property (nonatomic) id<DisplayViewDelegate> delegate;
@property (nonatomic) NSBitmapImageRep *rep;

@end
